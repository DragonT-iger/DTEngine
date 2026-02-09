#include "pch.h"
#include "PostProcessManager.h"
#include "RenderTexture.h"
#include "DX11Renderer.h"
#include "DXHelper.h"
#include "ResourceManager.h" // 쉐이더 로드용
#include "Shader.h"

using namespace Microsoft::WRL;

PostProcessManager::PostProcessManager() = default;
PostProcessManager::~PostProcessManager() = default;

struct ToneMappingCBuffer
{
    float Exposure = 1;
    float pad[3];
};


void PostProcessManager::Initialize(int width, int height)
{
    for (int i = 0; i < 2; ++i)
    {
        m_tempRT[i] = std::make_unique<RenderTexture>();
        m_tempRT[i]->Initialize(width, height, RenderTextureType::Tex2D, false, false, true);
    }

    auto device = DX11Renderer::Instance().GetDevice();
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // point보다 리니어가 낫네 이게 좀 자연스럽게 보간되네 지글지글 거림이나 계단현상 완화됨
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_pointSampler.GetAddressOf());




    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(ToneMappingCBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());

    DXHelper::ThrowIfFailed(hr);
}

void PostProcessManager::Resize(int width, int height)
{
    for (int i = 0; i < 2; ++i)
    {
        if (m_tempRT[i]) m_tempRT[i]->Resize(width, height);
    }
}

void PostProcessManager::Execute(RenderTexture* sceneTexture, ID3D11RenderTargetView* backBufferRTV, uint32_t activeEffectsMask, Camera* camera, int width, int height)
{
    RenderTexture* currentSrc = sceneTexture;
    RenderTexture* currentDest = m_tempRT[0].get();

    if (m_tempRT[0]->GetWidth() != width || m_tempRT[0]->GetHeight() != height) {
        Resize(width, height);
    }

    int pingPongIndex = 0;

    for (auto& effect : m_effects)
    {
        uint32_t effectType = static_cast<uint32_t>(effect->GetType());

        if ((activeEffectsMask & effectType) == 0)
            continue;

        effect->Render(currentSrc, currentDest, camera);

        currentSrc = currentDest;
        pingPongIndex = 1 - pingPongIndex;
        currentDest = m_tempRT[pingPongIndex].get();
    }


    RenderToneMapping(currentSrc, backBufferRTV, width, height); //기존 blit 에서 ToneMapping 포함해서 처리, 얘는 순서가 마지막이어야 해서;
   // Blit(currentSrc, backBufferRTV, width, height);
}

void PostProcessManager::RenderToneMapping(RenderTexture* src, ID3D11RenderTargetView* destRTV, int width, int height)
{
    auto context = DX11Renderer::Instance().GetContext();

    DX11Renderer::Instance().SetCullMode(CullMode::None);

    ID3D11RenderTargetView* rtvs[1] = { destRTV };
    context->OMSetRenderTargets(1, rtvs, nullptr); 

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0; vp.TopLeftY = 0;
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);

    Shader* ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/ToneMapping_PS.hlsl");
    if (ps) ps->Bind();

    if (m_constantBuffer)
    {
        ToneMappingCBuffer data;
        data.Exposure =1.0f;
        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);

        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetSamplers(0, 1, m_pointSampler.GetAddressOf());

    DX11Renderer::Instance().DrawFullScreenQuad();


    //flush
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    ID3D11Buffer* nullCB = nullptr;
    context->PSSetConstantBuffers(0, 1, &nullCB);

    DX11Renderer::Instance().OffPS();
}

void PostProcessManager::Blit(RenderTexture* src, ID3D11RenderTargetView* destRTV, int width, int height)
{
    auto context = DX11Renderer::Instance().GetContext();

    ID3D11RenderTargetView* rtvs[1] = { destRTV };

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0; vp.TopLeftY = 0;

    vp.Width = (float)width;
    vp.Height = (float)height;

    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);
    

    //Shader* vs = ResourceManager::Instance().Load<Shader>("Assets/Shaders/PostProcess_VS.hlsl");
    //if (vs) vs->Bind(); // 어짜피 의미가 없어 지금 코드 논리상

    Shader* ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/Copy_PS.hlsl");
    //Shader* ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/Gamma_Correction_PS.hlsl");

    //Shader* ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/GrayScale_PS.hlsl");
    if (ps) ps->Bind();

    context->OMSetDepthStencilState(nullptr, 0);
	context->OMSetRenderTargets(1, rtvs, nullptr);

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetSamplers(0, 1, m_pointSampler.GetAddressOf());

    DX11Renderer::Instance().DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    DX11Renderer::Instance().OffPS();
}
