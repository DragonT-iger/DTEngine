#include "pch.h"
#include "Vignette.h"
#include "ResourceManager.h"
#include "DX11Renderer.h"
#include "RenderTexture.h"
#include "Shader.h"

void Vignette::Initialize()
{
    // 쉐이더 로드
    m_vs = ResourceManager::Instance().Load<Shader>("Assets/Shaders/Vignette_VS.hlsl");
    m_ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/Vignette_PS.hlsl");
}

void Vignette::Render(RenderTexture* src, RenderTexture* dest)
{
    if (!m_vs || !m_ps) return;

    auto context = DX11Renderer::Instance().GetContext();

    if (dest) dest->Bind();

    m_vs->Bind();
    m_ps->Bind();

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    auto sampler = DX11Renderer::Instance().GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    DX11Renderer::Instance().DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    if (dest) dest->Unbind();
}

PostProcessType Vignette::GetType() const
{
    return PostProcessType::Vignette;
}