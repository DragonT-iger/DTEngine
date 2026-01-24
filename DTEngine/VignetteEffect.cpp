#include "pch.h"
#include "VignetteEffect.h"
#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "RenderTexture.h"


void VignetteEffect::Initialize()
{
    m_ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/Vignette_PS.hlsl");
}

void VignetteEffect::Render(RenderTexture* src, RenderTexture* dest)
{
    if (!m_ps || !src || !dest) return;

    auto& renderer = DX11Renderer::Instance();
    auto context = renderer.GetContext();

    dest->Bind();

    dest->Clear(0.0f, 0.0f, 1.0f, 1.0f);


    //m_vs->Bind();
    m_ps->Bind(); // 지금 어짜피 Bind 함수 자체가 vs, ps 둘다 바인딩 하도록 되어있음.

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    renderer.DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    dest->Unbind();
}