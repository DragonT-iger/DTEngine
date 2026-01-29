#include "pch.h"
#include "CircleMask.h"


#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "RenderTexture.h"
#include "Camera.h"


struct CircleMaskCbuffere
{
    float ViewPortW;
    float ViewPortH;
    float Time;
    float pad;


    Vector2 Center;
    Vector2 Width_Height;
};


void CircleMaskEffect::Initialize()
{
    m_ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/CircleMask_PS.hlsl");

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CircleMaskCbuffere);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    auto device = DX11Renderer::Instance().GetDevice();
    HRESULT hr = device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
}

void CircleMaskEffect::Render(RenderTexture* src, RenderTexture* dest, const Camera* camera)
{
    if (!m_ps || !src || !dest) return;

    auto& renderer = DX11Renderer::Instance();
    auto context = renderer.GetContext();


    if (camera && m_constantBuffer)
    {
        CircleMaskCbuffere data;
        data.Center = camera->GetCircleCenter();
        data.Width_Height = camera->GetCircleWidthHeight();

        data.ViewPortH = dest->GetHeight();
        data.ViewPortW = dest->GetWidth();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
    }

    dest->Bind();

    dest->Clear(0.0f, 0.0f, 1.0f, 1.0f);


    //m_vs->Bind();
    m_ps->Bind(); // 지금 어짜피 Bind 함수 자체가 vs, ps 둘다 바인딩 하도록 되어있음.

    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    renderer.DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    dest->Unbind();
}
