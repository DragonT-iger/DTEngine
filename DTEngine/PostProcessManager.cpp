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

void PostProcessManager::Initialize(int width, int height)
{
    // 핑퐁 버퍼 생성
    for (int i = 0; i < 2; ++i)
    {
        m_tempRT[i] = std::make_unique<RenderTexture>();
        m_tempRT[i]->Initialize(width, height, RenderTextureType::Tex2D, false);
    }

    // 샘플러 생성
    auto device = DX11Renderer::Instance().GetDevice();
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_pointSampler.GetAddressOf());
}

void PostProcessManager::Resize(int width, int height)
{
    for (int i = 0; i < 2; ++i)
    {
        if (m_tempRT[i]) m_tempRT[i]->Resize(width, height);
    }
}

void PostProcessManager::Execute(RenderTexture* sceneTexture, ID3D11RenderTargetView* backBufferRTV, uint32_t activeEffectsMask)
{
    RenderTexture* currentSrc = sceneTexture;
    RenderTexture* currentDest = m_tempRT[0].get();

    int pingPongIndex = 0;

    for (auto& effect : m_effects)
    {
        // ★ [에러 수정] 명시적 형변환으로 비교 (uint32_t <-> PostProcessType)
        uint32_t effectType = static_cast<uint32_t>(effect->GetType());

        if ((activeEffectsMask & effectType) == 0)
            continue;

        effect->Render(currentSrc, currentDest);

        currentSrc = currentDest;
        pingPongIndex = 1 - pingPongIndex;
        currentDest = m_tempRT[pingPongIndex].get();
    }

    Blit(currentSrc, backBufferRTV);
}

void PostProcessManager::Blit(RenderTexture* src, ID3D11RenderTargetView* destRTV)
{
    auto context = DX11Renderer::Instance().GetContext();

    ID3D11RenderTargetView* rtvs[1] = { destRTV };
    context->OMSetRenderTargets(1, rtvs, nullptr);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0; vp.TopLeftY = 0;
    vp.Width = (float)DX11Renderer::Instance().GetWidth();
    vp.Height = (float)DX11Renderer::Instance().GetHeight();
    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);

    // ★ ResourceManager를 이용해 "Copy" 쉐이더(VS, PS 쌍)를 로드해서 바인딩하세요.
    // 예: Shader* shader = ResourceManager::Instance().Load<Shader>("Copy");
    //     if(shader) shader->Bind();

    // 임시: 쉐이더 바인딩 코드는 프로젝트 상황에 맞춰 넣어야 함 (아래는 리소스 바인딩만 함)
    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);
    context->PSSetSamplers(0, 1, m_pointSampler.GetAddressOf());

    DX11Renderer::Instance().DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);
}