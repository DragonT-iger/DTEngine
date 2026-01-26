#include "pch.h" 
#include "DX11Renderer.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <wrl/client.h>
#include <cassert>
#include <stdexcept>
#include <iostream>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/CommonStates.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")


#include "Simplemathhelper.h"
#include "Transform.h"
#include "Light.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "InputManager.h"
#include "DXHelper.h"
#include "Shader.h"
#include "Font.h"


using Microsoft::WRL::ComPtr;

DX11Renderer::DX11Renderer() = default;
DX11Renderer::~DX11Renderer() { Destroy(); }

bool DX11Renderer::Initialize(HWND hwnd, int width, int height, bool vsync)
{
    m_hwnd = hwnd; m_width = width; m_height = height; m_vsync = vsync;
    if (!m_hwnd) return false;
    if (!CreateDeviceAndSwapchain()) return false;

    CreateConstantBuffers();
    CreateBackbuffers(width, height);
    CreateSamplers();


    m_states = std::make_unique<DirectX::DX11::CommonStates>(m_device.Get());
    m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(m_context.Get());

    try {
        m_font = std::make_unique<DirectX::DX11::SpriteFont>(m_device.Get(), L"Assets/Fonts/The Jamsil 2 Light.spritefont");
    }
    catch (...) {
        std::cout << "[Warning] Failed to load font: Assets/Fonts/The Jamsil 2 Light.spritefont\n";
    }

    //CreateShadowMap(16376, 16376); // max 왜 이러지

    CreateShadowMap(4096, 4096);



    m_postProcessManager = std::make_unique<PostProcessManager>();
    m_postProcessManager->Initialize(width, height);

    m_resolvedSceneRT = std::make_unique<RenderTexture>();
    m_resolvedSceneRT->Initialize(width, height, RenderTextureType::Tex2D, false);

    
    return true;
}

void DX11Renderer::Resize(int width, int height)
{
    if (!m_swapchain || width <= 0 || height <= 0) return;
    if (width == m_width && height == m_height)    return;

    ReleaseBackbuffers();

    UINT swapChainFlags = 0;

    if (!GetVsync())
    {
        swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    }

    const HRESULT hr = m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, swapChainFlags);
    DXHelper::ThrowIfFailed(hr);

    CreateBackbuffers(width, height);
    m_width = width; m_height = height;

    if (m_postProcessManager)
        m_postProcessManager->Resize(width, height);

    if (m_resolvedSceneRT)
        m_resolvedSceneRT->Resize(width, height);
}
 
//이거 상수버퍼 인터페이스 만든 다음에, 템플릿으로 처리해도 깔끔할듯 함 
// ㅇㅇ dx renderer가 무거워 진다면 따로 처리 해도 될듯?
// 구조 예쁘게 할 시간 있으면 이건 해도 될듯. 
void DX11Renderer::UpdateObject_CBUFFER(const Matrix& Worrld, const Matrix& WorldTranspose)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_world_M.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    CBuffer_Object_Data* dataPtr = static_cast<CBuffer_Object_Data*>(mappedData.pData);
    dataPtr->WorldTM = Worrld.Transpose();
    dataPtr->WorldInverseTransposeTM = WorldTranspose.Transpose();
    m_context->Unmap(m_cbuffer_world_M.Get(), 0);
    
    //binding은 되어있으니 걱정말라구!
}

void DX11Renderer::UpdateFrame_CBUFFER(const Matrix& viewTM, const Matrix& projectionTM)
{
    m_viewTM = viewTM;
    m_projTM = projectionTM;

    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_frame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    CBuffer_Frame_Data* dataPtr = static_cast<CBuffer_Frame_Data*>(mappedData.pData);

    dataPtr->ViewTM = viewTM.Transpose();
    dataPtr->ProjectionTM = projectionTM.Transpose();

    m_context->Unmap(m_cbuffer_frame.Get(), 0);

}
void DX11Renderer::UpdateLights_CBUFFER(const std::vector<Light*>& lights, Camera* camera)
{
    if (!m_cbuffer_lights) return;

    if (!m_context) return;

    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_lights.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    if (FAILED(hr)) return;

    CBuffer_GlobalLight* data = static_cast<CBuffer_GlobalLight*>(mappedData.pData);

    int count = std::min((int)lights.size(), MAX_LIGHTS);
    data->ActiveCount = count;

    data->CameraPos = camera->GetCamPos();     
    data->CameraDir = camera->GetCamFor();     
    data->IsOrtho = camera->IsOrthographic() ? 1.0f : 0.0f;
    data->LightViewProjScale = m_lightViewProjScale.Transpose();
    float w = m_shadowViewport.Width;
    float h = m_shadowViewport.Height;
    data->ShadowMapInfo = Vector4(1.0f / w, 1.0f / h, w, h);

    for (int i = 0; i < count; ++i)
    {
        Light* light = lights[i];
        Transform* tf = light->GetTransform();

        Vector3 pos = tf->GetPosition();
        data->Lights[i].PositionRange = Vector4(pos.x, pos.y, pos.z, light->m_range);

        Vector3 dir = -tf->Forward();
        data->Lights[i].DirectionType = Vector4(dir.x, dir.y, dir.z, (float)light->m_type);

        data->Lights[i].ColorIntensity = Vector4(light->m_color.x, light->m_color.y, light->m_color.z, light->m_intensity);
    }

    for (int i = count; i < MAX_LIGHTS; ++i)
    {
        data->Lights[i].PositionRange = Vector4(0, 0, 0, 0);
        data->Lights[i].DirectionType = Vector4(0, 0, 0, 0);
        data->Lights[i].ColorIntensity = Vector4(0, 0, 0, 0);
    }

    m_context->Unmap(m_cbuffer_lights.Get(), 0);

}

void DX11Renderer::UpdateMaterial_CBUFFER(const MaterialData& M_Data)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_material.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    MaterialData* dataPtr = static_cast<MaterialData*>(mappedData.pData);
    dataPtr->Color = M_Data.Color;
    
    dataPtr->UVTransform = M_Data.UVTransform;
    dataPtr->UseTexture = M_Data.UseTexture;
    dataPtr->metallicFactor = M_Data.metallicFactor;
    dataPtr->roughnessFactor = M_Data.roughnessFactor;

    m_context->Unmap(m_cbuffer_material.Get(), 0);
}

void DX11Renderer::UpdateTextureFlag_CBUFFER(uint32_t Flags)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_Texture_flags.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    TextureFlag* dataPtr = static_cast<TextureFlag*>(mappedData.pData);
    dataPtr->Use_Texture_flags = Flags;
 
    m_context->Unmap(m_cbuffer_Texture_flags.Get(), 0);

}

void DX11Renderer::UpdateMatrixPallette_CBUFFER(std::vector<Matrix>& matrix)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_matrix_pallette.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    Matrix_Pallette* dataPtr = static_cast<Matrix_Pallette*>(mappedData.pData);

    for (int i = 0; i < matrix.size(); i++)
    {
        dataPtr->Matrix_Pallette[i] = matrix[i].Transpose();
    }

    m_context->Unmap(m_cbuffer_matrix_pallette.Get(), 0);
}

void DX11Renderer::UpdateSkyBox_CBUFFER(SkyBox& data)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};

    HRESULT hr = m_context->Map(m_cbuffer_SkyBox.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    SkyBox* dataPtr = static_cast<SkyBox*>(mappedData.pData);
    dataPtr->SkyBox_Color = data.SkyBox_Color;
    
    m_context->Unmap(m_cbuffer_SkyBox.Get(), 0);
}

void DX11Renderer::UpdateEffect_CBUFFER(EffectParams& data)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};

    HRESULT hr = m_context->Map(m_cbuffer_Effect.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    EffectParams* dataPtr = static_cast<EffectParams*>(mappedData.pData);
    *dataPtr = data;

    m_context->Unmap(m_cbuffer_Effect.Get(), 0);
}

void DX11Renderer::BeginUIRender(float renderWidth, float renderHeight)
{

    float scaleX = renderWidth  / m_refWidth;
    float scaleY = renderHeight / m_refHeight;

    Matrix uiScaleMatrix = Matrix::CreateScale(scaleX, scaleY, 1.0f);


    if (m_spriteBatch)
    {
        m_spriteBatch->Begin(
            DirectX::DX11::SpriteSortMode_Deferred,
            m_states->NonPremultiplied(),
            nullptr,                                    // SamplerState
            nullptr,                                    // DepthStencilState
            nullptr,                                    // RasterizerState
            nullptr,                                    // setCustomShaders
            uiScaleMatrix
        );
    }
    Camera* mainCam = SceneManager::Instance().GetActiveScene()->GetMainCamera();
    if (mainCam == nullptr) return;

    static float orthoSize = DX11Renderer::Instance().GetRefHeight(); 

    static float orthoHeight = orthoSize;
    float aspectRatio = mainCam->GetAspectRatio();
    float orthoWidth = orthoHeight * aspectRatio;

    static float nearZ = -20.0f;
    static float farZ = 1000;
    // 이 값들 때문에 UI의 Z값이 이상해지면 클립되는거임

    Matrix uiProj = DirectX::XMMatrixOrthographicLH(orthoWidth, orthoHeight, nearZ, farZ);

    UpdateFrame_CBUFFER(SimpleMathHelper::IdentityMatrix(), uiProj);

    m_context->OMSetDepthStencilState(m_states->DepthNone(), 0);

}

void DX11Renderer::EndUIRender()
{
    if (m_spriteBatch)
    {
        m_spriteBatch->End();
    }

    //Camera* mainCam = SceneManager::Instance().GetActiveScene()->GetMainCamera(); 
    //if (mainCam == nullptr) return;

    //mainCam->SetIsOrthographic(m_isOrthoBackup);

    //mainCam->SetProjectionOrthographic();
    
    

    ResetRenderState();
}

void DX11Renderer::CreateShadowMap(int width, int height)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    DXHelper::ThrowIfFailed(m_device->CreateTexture2D(&texDesc, nullptr, m_shadowMapTex.GetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    // dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    DXHelper::ThrowIfFailed(m_device->CreateDepthStencilView(m_shadowMapTex.Get(), &dsvDesc, m_shadowDSV.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    // srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    srvDesc.Texture2D.MipLevels = 1;

    DXHelper::ThrowIfFailed(m_device->CreateShaderResourceView(m_shadowMapTex.Get(), &srvDesc, m_shadowSRV.GetAddressOf()));

    m_shadowViewport.TopLeftX = 0.0f;
    m_shadowViewport.TopLeftY = 0.0f;
    m_shadowViewport.Width = static_cast<float>(width);
    m_shadowViewport.Height = static_cast<float>(height);
    m_shadowViewport.MinDepth = 0.0f;
    m_shadowViewport.MaxDepth = 1.0f;                                             
}

void DX11Renderer::BeginShadowPass(const Vector3& lightPos, const Vector3& lightDir, bool isDirectional, float size)
{
    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_context->PSSetSamplers(10, 1, m_shadowSampler.GetAddressOf());
    m_context->PSSetShaderResources(10, 1, &nullSRV); // 얘는 바깥으로 빼버릴까 싶어 

    ID3D11RenderTargetView* nullRTV = nullptr;
    m_context->OMSetRenderTargets(0, &nullRTV, m_shadowDSV.Get());
    m_context->ClearDepthStencilView(m_shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    m_context->RSSetViewports(1, reinterpret_cast<const D3D11_VIEWPORT*>(&m_shadowViewport));

    Vector3 target = lightPos + lightDir;
    Vector3 up = Vector3(0, 1, 0);

    Matrix lightView = XMMatrixLookAtLH(lightPos, target, up);
    Matrix lightProj;

    //if (isDirectional)
    //{
    lightProj = DirectX::XMMatrixOrthographicLH(size, size, 0.1f, 100.0f);
    //}
    //else
    //{
    //    lightProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 0.1f, 100.0f);
    //}

    Matrix textureScaleBias = Matrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    );

    m_lightViewProjScale = lightView * lightProj * textureScaleBias;

    UpdateFrame_CBUFFER(lightView, lightProj);
}



void DX11Renderer::DrawUI(Texture* texture, const Vector2& position, const Vector2& size, const Vector4& color)
{
    if (!m_spriteBatch || !texture) return;

    RECT destRect;
    destRect.left   = static_cast<LONG>(position.x);
    destRect.top    = static_cast<LONG>(position.y);
    destRect.right  = static_cast<LONG>(position.x + size.x);
    destRect.bottom = static_cast<LONG>(position.y + size.y);

    m_spriteBatch->Draw(texture->GetSRV(), destRect, color);
}

void DX11Renderer::DrawString(const std::wstring& text, const Vector2& position, const float& m_fontSize, const Vector4& color)
{
    if (!m_spriteBatch || !m_font) return;

    m_font->DrawString(
        m_spriteBatch.get(),
        text.c_str(),
        position,
        color,
        0.0f,                    
        DirectX::XMFLOAT2(0, 0), 
        m_fontSize
    );
}

void DX11Renderer::DrawString(Font* Font, const std::wstring& text, const Vector2& position, const float& fontSize, const Vector4& color)
{
    auto SpriteFont = Font->GetSpriteFont();

    if (!SpriteFont || !m_spriteBatch) return;

    SpriteFont->DrawString(m_spriteBatch.get(),
        text.c_str(),
        position,
        color,
        0.0f,
        DirectX::XMFLOAT2(0, 0),
        fontSize
    );

}


void DX11Renderer::SetBlendMode(BlendMode mode)
{
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };

    if (mode == BlendMode::AlphaBlend)
    {
        m_context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);
    }
    else
    {
        m_context->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
    }
}

void DX11Renderer::SetCullMode(CullMode mode)
{
    if (!m_context) return;

    ID3D11RasterizerState* state = nullptr;
    switch (mode)
    {
        case CullMode::Back:  state = m_rsCullBack.Get(); break;
        case CullMode::Front: state = m_rsCullFront.Get(); break;
        case CullMode::None:  state = m_rsCullNone.Get(); break;
    }

    m_context->RSSetState(state);
}

void DX11Renderer::BeginFrame(const float clearColor[4])
{


    assert(m_context && m_msaaTargetRTV);

    if (m_defaultDepthStencilState)
    {
        m_context->OMSetDepthStencilState(m_defaultDepthStencilState.Get(), 0);
    }

    bool clearDepth = true;

    ID3D11RenderTargetView* rtvs[1] = { m_msaaTargetRTV.Get() };
    m_context->OMSetRenderTargets(1, rtvs, clearDepth ? m_dsv.Get() : nullptr);
    m_context->ClearRenderTargetView(m_msaaTargetRTV.Get(), clearColor);
    if (clearDepth && m_dsv)
        m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(m_width);
    vp.Height = static_cast<float>(m_height);
    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);

    //BindGlobalResources(); //상수 버퍼 Sampler etc 


    // 이런 전역적인 데이터는 Renderer에서 따로 관리해도 괜찮을 거 같음. Scene에서 꺼내오는 건 가능하겠다만. 
    // RenderTarget data처럼 입출력을 동시에 불가능 한 경우도 있으니. 예상 가능한 범위에서 조작하는 게 나아보여. 
    //const std::string path = "Assets/Models/Env/Cube/Forest/Forest.dds";

        const std::string path = "Assets/Models/Env/Cube/SkyBox/skybox.dds";

    Texture* temp = ResourceManager::Instance().Load<Texture>(path);
    ID3D11ShaderResourceView* CubeMap = temp->GetSRV();
    m_context->PSSetShaderResources(9, 1, &CubeMap);


}
//  ★
void DX11Renderer::BindGlobalResources()
{
    //상수 버퍼 Binding

    //Sampler는 더 봐야 할 듯 
    //m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //VS
    m_context->VSSetConstantBuffers(1, 1, m_cbuffer_frame.GetAddressOf());
    m_context->VSSetConstantBuffers(2, 1, m_cbuffer_world_M.GetAddressOf());

    m_context->VSSetConstantBuffers(3, 1, m_cbuffer_lights.GetAddressOf());
    m_context->VSSetConstantBuffers(4, 1, m_cbuffer_material.GetAddressOf());

    m_context->VSSetConstantBuffers(5, 1, m_cbuffer_Texture_flags.GetAddressOf());
    m_context->VSSetConstantBuffers(6, 1, m_cbuffer_matrix_pallette.GetAddressOf());

    m_context->VSSetConstantBuffers(7, 1, m_cbuffer_SkyBox.GetAddressOf());
    m_context->VSSetConstantBuffers(8, 1, m_cbuffer_Effect.GetAddressOf());
    //PS
    m_context->PSSetConstantBuffers(1, 1, m_cbuffer_frame.GetAddressOf());
    m_context->PSSetConstantBuffers(2, 1, m_cbuffer_world_M.GetAddressOf());

    m_context->PSSetConstantBuffers(3, 1, m_cbuffer_lights.GetAddressOf());
    m_context->PSSetConstantBuffers(4, 1, m_cbuffer_material.GetAddressOf());

    m_context->PSSetConstantBuffers(5, 1, m_cbuffer_Texture_flags.GetAddressOf());
    m_context->PSSetConstantBuffers(6, 1, m_cbuffer_matrix_pallette.GetAddressOf());

    m_context->PSSetConstantBuffers(7, 1, m_cbuffer_SkyBox.GetAddressOf());
    m_context->PSSetConstantBuffers(8, 1, m_cbuffer_Effect.GetAddressOf());


    m_context->PSSetShaderResources(10, 1, m_shadowSRV.GetAddressOf());
    m_context->PSSetSamplers(10, 1, m_shadowSampler.GetAddressOf());
}
//  ★
void DX11Renderer::CreateConstantBuffers()
{

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    //해당 자료형에서 16 byte 정렬 되어있음. 

    //b0
    bd.ByteWidth = sizeof(CBuffer_Frame_Data);
    DXHelper::ThrowIfFailed (m_device->CreateBuffer(&bd, nullptr, m_cbuffer_frame.GetAddressOf()));
    //b1
    bd.ByteWidth = sizeof(CBuffer_Object_Data);
    DXHelper::ThrowIfFailed (m_device->CreateBuffer(&bd, nullptr, m_cbuffer_world_M.GetAddressOf()));
    //b2
    bd.ByteWidth = sizeof(CBuffer_GlobalLight);
    DXHelper::ThrowIfFailed(m_device->CreateBuffer(&bd, nullptr, m_cbuffer_lights.GetAddressOf()));
    //b3
    bd.ByteWidth = sizeof(MaterialData);
    DXHelper::ThrowIfFailed (m_device->CreateBuffer(&bd, nullptr, m_cbuffer_material.GetAddressOf()));
    //b4
    bd.ByteWidth = sizeof(TextureFlag);
    DXHelper::ThrowIfFailed(m_device->CreateBuffer(&bd, nullptr, m_cbuffer_Texture_flags.GetAddressOf()));
    //b5
    bd.ByteWidth = sizeof(Matrix_Pallette);
    DXHelper::ThrowIfFailed(m_device->CreateBuffer(&bd, nullptr, m_cbuffer_matrix_pallette.GetAddressOf()));


    bd.ByteWidth = sizeof(SkyBox);
    DXHelper::ThrowIfFailed(m_device->CreateBuffer(&bd, nullptr, m_cbuffer_SkyBox.GetAddressOf()));

    bd.ByteWidth = sizeof(EffectParams);
    DXHelper::ThrowIfFailed(m_device->CreateBuffer(&bd, nullptr, m_cbuffer_Effect.GetAddressOf()));



      // D3D11_BUFFER_DESC boneDesc = {};
    // boneDesc.ByteWidth = sizeof(CBuffer_BoneData);
    // boneDesc.Usage = D3D11_USAGE_DYNAMIC;
    // boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // boneDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // hr = m_device->CreateBuffer(&boneDesc, nullptr, m_cbuffer_bones.GetAddressOf());
    // DXHelper::ThrowIfFailed(hr);

}

void DX11Renderer::EndFrame()
{
    if (m_msaaTargetTex && m_backbufferTex)
    {
        m_context->ResolveSubresource(
            m_backbufferTex.Get(), 0,      
            m_msaaTargetTex.Get(), 0,      
//#ifdef _DEBUG
            DXGI_FORMAT_R8G8B8A8_UNORM
//#else
            //DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
//#endif
        );
    }


    //일단 

    Camera* mainCam = SceneManager::Instance().GetActiveScene()->GetMainCamera();

    uint32_t effectMask = (mainCam != nullptr) ? mainCam->GetPostProcessMask() : 0;

    if (m_msaaTargetTex && m_resolvedSceneRT)
    {
        m_context->ResolveSubresource(
            m_resolvedSceneRT->GetTexture(), 0,
            m_msaaTargetTex.Get(), 0,
            DXGI_FORMAT_R8G8B8A8_UNORM
        );

        if (m_postProcessManager)
        {
            m_postProcessManager->Execute(m_resolvedSceneRT.get(), m_rtv.Get(), effectMask);
        }
        else
        {
            m_context->ResolveSubresource(
                m_backbufferTex.Get(), 0,
                m_msaaTargetTex.Get(), 0,
                DXGI_FORMAT_R8G8B8A8_UNORM
            );
        }
    }
}

void DX11Renderer::Present()
{
    if (!m_swapchain) return;

    UINT interval = GetVsync() ? 1 : 0;

    UINT flags = 0;

    BOOL isFullscreen = FALSE;
    if (m_swapchain)
    {
        m_swapchain->GetFullscreenState(&isFullscreen, nullptr);
    }

    if (!GetVsync() && !isFullscreen)
    {
        flags |= DXGI_PRESENT_ALLOW_TEARING; // 창모드에서만 켜줘야함
    }

    HRESULT hr = m_swapchain->Present(interval, flags);

}

void DX11Renderer::SetFullscreen(bool enable)
{
    if (!m_swapchain) return;
    m_swapchain->SetFullscreenState(enable ? TRUE : FALSE, nullptr);
}

void DX11Renderer::Destroy()
{
    ReleaseBackbuffers();
    ReleaseCB();

    m_swapchain.Reset();
    m_context.Reset();
    m_device.Reset();

    m_spriteBatch.reset();
    m_font.reset();
    m_states.reset();

    m_hwnd = nullptr;
}

void DX11Renderer::SetRenderTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	//assert(rtv != nullptr && "RTV cannot be null");

    ID3D11RenderTargetView* rtvs[1] = { rtv };
    m_context->OMSetRenderTargets(1, rtvs, dsv);
}

void DX11Renderer::SetViewport(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY)
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = minDepth;
    viewport.MaxDepth = maxDepth;
    viewport.TopLeftX = topLeftX;
    viewport.TopLeftY = topLeftY;

    if (m_context)
    {
        m_context->RSSetViewports(1, &viewport);
    }
}

HWND DX11Renderer::GetHwnd()
{
    return m_hwnd;
}

ID3D11Device* DX11Renderer::GetDevice() const { return m_device.Get(); }
ID3D11DeviceContext* DX11Renderer::GetContext() const { return m_context.Get(); }
ID3D11RenderTargetView* DX11Renderer::GetBackbufferRTV() const { return m_rtv.Get(); }



void DX11Renderer::ClearCache()
{
    m_currentShaderID = 0;
    for (int i = 0; i < 16; ++i) m_currentSRVs[i] = nullptr;
}



void DX11Renderer::ResetRenderState()
{
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    m_context->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
    m_context->OMSetDepthStencilState(nullptr, 0);
    m_context->RSSetState(nullptr);
    m_context->RSSetState(m_rsCullBack.Get());
}

//  ★
void DX11Renderer::BindShader(Shader* shader)
{
    if (!shader) return;

   
    if (m_currentShaderID == shader->GetID())
    {
        return;
    }

    shader->Bind(); 
    m_currentShaderID = shader->GetID(); 
}

//Slot 하드코딩은 좀 그렇긴 한데 일단 16개 정도로 맞춰놓음 
void DX11Renderer::BindTexture(int slot, ID3D11ShaderResourceView* srv)
{
    if (slot < 0 || slot >= 16) return;

  /*  if (m_currentSRVs[slot] == srv)
    {
        return;
    }*/

    m_context->PSSetShaderResources(slot, 1, &srv);
    m_currentSRVs[slot] = srv; 
}

ID3D11SamplerState* DX11Renderer::GetSampler(FilterMode filter, WrapMode wrap)
{
    int index = (int)filter * 2 + (int)wrap;
    if (index < 0 || index >= 6) return m_samplers[1].Get(); 
    return m_samplers[index].Get();
}

void DX11Renderer::OffPS()
{
    m_context->PSSetShader(nullptr, nullptr, 0);
}

void DX11Renderer::DrawFullScreenQuad()
{
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->IASetInputLayout(nullptr);
    m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

    m_context->Draw(3, 0);
}

bool DX11Renderer::CreateDeviceAndSwapchain()
{
    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL outLevel{};

    ComPtr<ID3D11Device> dev;
    ComPtr<ID3D11DeviceContext> ctx;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        levels, _countof(levels), D3D11_SDK_VERSION,
        dev.GetAddressOf(), &outLevel, ctx.GetAddressOf());

    DXHelper::ThrowIfFailed(hr);

    ComPtr<IDXGIDevice>   dxgiDev;  dev.As(&dxgiDev);
    ComPtr<IDXGIAdapter>  adapter;  dxgiDev->GetAdapter(&adapter);
    ComPtr<IDXGIFactory2> factory;  adapter->GetParent(__uuidof(IDXGIFactory2), &factory);

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;

    hr = dev->CreateDepthStencilState(&dsDesc, m_defaultDepthStencilState.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);


    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;

    rsDesc.MultisampleEnable = TRUE;       // MSAA 활성화 (필수)
    rsDesc.AntialiasedLineEnable = TRUE;

    // Cull Back 
    rsDesc.CullMode = D3D11_CULL_BACK;
    hr = dev->CreateRasterizerState(&rsDesc, m_rsCullBack.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    // Cull Front 
    rsDesc.CullMode = D3D11_CULL_FRONT;
    hr = dev->CreateRasterizerState(&rsDesc, m_rsCullFront.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    // Cull None 
    rsDesc.CullMode = D3D11_CULL_NONE;
    hr = dev->CreateRasterizerState(&rsDesc, m_rsCullNone.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = dev->CreateBlendState(&blendDesc, m_alphaBlendState.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);


    DXGI_SWAP_CHAIN_DESC1 scd{};
    scd.Width = 0;
    scd.Height = 0;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc = { 1, 0 };
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // 최신 플립 모델 - 창모드 시 Vsync가 자동으로 켜짐
    //scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;        // Bitblt방식 직접 복사 - 창모드 시 Vsync 안켜짐
    scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;



    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd{};
    fsd.Windowed = TRUE;

    ComPtr<IDXGISwapChain1> sc1;
    hr = factory->CreateSwapChainForHwnd(dev.Get(), m_hwnd, &scd, &fsd, nullptr, sc1.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    // Alt+Enter 기본 전체화면 토글 막기(직접 처리)
    //factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    m_device = std::move(dev);
    m_context = std::move(ctx);
    m_swapchain = std::move(sc1);

    //if (m_device)
    //{
    //    m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, m_msaa, &m_msaaQuality);
    //    // 품질 레벨이 0이면 해당 샘플 수를 지원하지 않는 것이므로 1(No AA)로 강제하거나 예외 처리 필요
    //    if (m_msaaQuality == 0) m_msaa = 1;
    //} // 그냥 에러 띄우는게 낫지

    return true;
}

void DX11Renderer::CreateBackbuffers(int width, int height)
{
    assert(m_swapchain && m_device);
    ReleaseBackbuffers();

    // Backbuffer RTV
    ComPtr<ID3D11Texture2D> backTex;
    HRESULT hr = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(backTex.GetAddressOf()));
    DXHelper::ThrowIfFailed(hr);

    m_backbufferTex = std::move(backTex);

//#ifdef _DEBUG
    hr = m_device->CreateRenderTargetView(m_backbufferTex.Get(), nullptr, m_rtv.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

//#else
    //D3D11_RENDER_TARGET_VIEW_DESC rtvViewDesc = {};
    //rtvViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;        // 백버퍼 감마코렉션 나눠주는거
    //rtvViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    //rtvViewDesc.Texture2D.MipSlice = 0;

    //hr = m_device->CreateRenderTargetView(m_backbufferTex.Get(), &rtvViewDesc, m_rtv.GetAddressOf());
    //DXHelper::ThrowIfFailed(hr);

    

   

//#endif // _DEBUG
    
    D3D11_TEXTURE2D_DESC msaaDesc = {};
    m_backbufferTex->GetDesc(&msaaDesc);                    // 백버퍼 설정 복사
    msaaDesc.SampleDesc.Count = m_msaa;                     // 샘플 수 (예: 4)
    msaaDesc.SampleDesc.Quality = m_msaaQuality - 1;        // 품질
    msaaDesc.BindFlags = D3D11_BIND_RENDER_TARGET;          // 렌더 타겟
//#ifndef _DEBUG
    //msaaDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
//#endif
    hr = m_device->CreateTexture2D(&msaaDesc, nullptr, m_msaaTargetTex.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

//#ifdef _DEBUG

    hr = m_device->CreateRenderTargetView(m_msaaTargetTex.Get(), nullptr, m_msaaTargetRTV.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);
//#else
    //D3D11_RENDER_TARGET_VIEW_DESC msaaRtvDesc = {};
    //msaaRtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; 

    //if (m_msaa > 1)
    //{
    //    msaaRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    //}
    //else
    //{
    //    msaaRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    //    msaaRtvDesc.Texture2D.MipSlice = 0;
    //}

    //hr = m_device->CreateRenderTargetView(m_msaaTargetTex.Get(), &msaaRtvDesc, m_msaaTargetRTV.GetAddressOf());
    //DXHelper::ThrowIfFailed(hr);

//#endif
    

    D3D11_TEXTURE2D_DESC rtvDesc;
    m_backbufferTex->GetDesc(&rtvDesc);
    // Depth/Stencil
    D3D11_TEXTURE2D_DESC ds{};
    ds.Width = rtvDesc.Width; ds.Height = rtvDesc.Height; ds.MipLevels = 1; ds.ArraySize = 1;
    ds.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds.SampleDesc.Count = m_msaa;              
    ds.SampleDesc.Quality = m_msaaQuality - 1; 
    ds.Usage = D3D11_USAGE_DEFAULT;
    ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_device->CreateTexture2D(&ds, nullptr, m_depthTex.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    hr = m_device->CreateDepthStencilView(m_depthTex.Get(), nullptr, m_dsv.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    m_width = rtvDesc.Width;
    m_height = rtvDesc.Height;



}

void DX11Renderer::ReleaseBackbuffers()
{
    m_dsv.Reset();
    m_depthTex.Reset();
    m_rtv.Reset();
    m_backbufferTex.Reset();

    m_msaaTargetRTV.Reset();
    m_msaaTargetTex.Reset();
}

void DX11Renderer::ReleaseCB()
{
    m_cbuffer_frame.Reset(); 
    m_cbuffer_world_M.Reset(); 
    m_cbuffer_lights.Reset();
    m_cbuffer_material.Reset();

    m_cbuffer_Texture_flags.Reset();
    m_cbuffer_matrix_pallette.Reset();
    m_cbuffer_SkyBox.Reset();
    m_cbuffer_SkyBox.Reset();
}


void DX11Renderer::CreateSamplers()
{
    if (!m_device) return;

    for (int f = 0; f < 3; ++f) // Point, Bilinear, Trilinear
    {
        for (int w = 0; w < 2; ++w) // Repeat, Clamp
        {
            D3D11_SAMPLER_DESC desc = {};

            switch ((FilterMode)f)
            {
                case FilterMode::Point:     desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
                case FilterMode::Bilinear:  desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; break;
                case FilterMode::Trilinear: desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
            }


            //desc.Filter = D3D11_FILTER_ANISOTROPIC;
            //desc.MaxAnisotropy = 8;



            D3D11_TEXTURE_ADDRESS_MODE addr = (w == 0) ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressU = desc.AddressV = desc.AddressW = addr;

            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            desc.MinLOD = 0;
            desc.MaxLOD = D3D11_FLOAT32_MAX;
            desc.MipLODBias = 0;

            int index = f * 2 + w;
            m_device->CreateSamplerState(&desc, m_samplers[index].GetAddressOf());
        }
    }


    D3D11_SAMPLER_DESC shadowDesc = {};
    shadowDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    shadowDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    shadowDesc.ComparisonFunc = D3D11_COMPARISON_LESS; 
    shadowDesc.MinLOD = 0;
    shadowDesc.MaxLOD = D3D11_FLOAT32_MAX;

    shadowDesc.BorderColor[0] = 1.0f;
    shadowDesc.BorderColor[1] = 1.0f;
    shadowDesc.BorderColor[2] = 1.0f;
    shadowDesc.BorderColor[3] = 1.0f;

    DXHelper::ThrowIfFailed(m_device->CreateSamplerState(&shadowDesc, m_shadowSampler.GetAddressOf()));


}
