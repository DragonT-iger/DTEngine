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
    for (int i = 0; i < 2; ++i)
    {
        m_tempRT[i] = std::make_unique<RenderTexture>();
        m_tempRT[i]->Initialize(width, height, RenderTextureType::Tex2D, false);
    }

    auto device = DX11Renderer::Instance().GetDevice();
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // point보다 리니어가 낫네 이게 좀 자연스럽게 보간되네 지글지글 거림이나 계단현상 완화됨
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

        effect->Render(currentSrc, currentDest);

        currentSrc = currentDest;
        pingPongIndex = 1 - pingPongIndex;
        currentDest = m_tempRT[pingPongIndex].get();
    }

    Blit(currentSrc, backBufferRTV, width, height);
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