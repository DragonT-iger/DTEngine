#include "pch.h"
#include "GrayScale.h"
#include "ResourceManager.h"
#include "DX11Renderer.h"
#include "RenderTexture.h"
#include "Shader.h"

void GrayScale::Initialize()
{
    // 쉐이더 로드
    m_vs = ResourceManager::Instance().Load<Shader>("Assets/Shaders/GrayScale_VS.hlsl");
    m_ps = ResourceManager::Instance().Load<Shader>("Assets/Shaders/GrayScale_PS.hlsl");
}

void GrayScale::Render(RenderTexture* src, RenderTexture* dest)
{
    if (!m_vs || !m_ps) return;

    auto context = DX11Renderer::Instance().GetContext();

    // 1. 렌더 타겟 설정 (Destination)
    // dest가 nullptr이면 백버퍼 등을 의미할 수 있으나, PostProcessManager 구조상 RT가 들어옴
    if (dest) dest->Bind();

    // 2. 쉐이더 바인딩
    m_vs->Bind();
    m_ps->Bind();

    // 3. 리소스 바인딩 (Source Texture)
    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    // 샘플러 바인딩 (DX11Renderer에서 가져오거나 생성해둔 것 사용)
    auto sampler = DX11Renderer::Instance().GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    // 4. 그리기 (Full Screen Quad)
    DX11Renderer::Instance().DrawFullScreenQuad();

    // 5. 정리 (Unbind)
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    if (dest) dest->Unbind();
}

PostProcessType GrayScale::GetType() const
{
    return PostProcessType::GrayScale;
}