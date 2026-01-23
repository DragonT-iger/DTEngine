#include "pch.h"
#include "GrayScaleEffect.h"
#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "RenderTexture.h"

void GrayScaleEffect::Initialize()
{
    m_vs = ResourceManager::Instance().Load<Shader>("Assets/Shaders/PostProcess_VS.hlsl");
    m_ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/GrayScale_PS.hlsl");
}

void GrayScaleEffect::Render(RenderTexture* src, RenderTexture* dest)
{
    if (!m_vs || !m_ps || !src || !dest) return;

    auto& renderer = DX11Renderer::Instance();
    auto context = renderer.GetContext();

    dest->Bind();

    renderer.SetCullMode(CullMode::None);
    renderer.SetBlendMode(BlendMode::Opaque);

    m_vs->Bind();
    m_ps->Bind();

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    renderer.DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    dest->Unbind();

    renderer.SetCullMode(CullMode::Back);
}