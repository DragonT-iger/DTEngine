#include "pch.h"
#include "BloomEffect.h"
#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "Camera.h"

void BloomEffect::Initialize()
{
    m_extractPS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomExtract_PS.hlsl");
    m_blurPS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomBlur_PS.hlsl");
    m_combinePS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomCombine_PS.hlsl");


	if (!m_extractPS) std::cout << "Failed to load BloomExtract_PS.hlsl\n";
	if (!m_blurPS) std::cout << "Failed to load BloomBlur_PS.hlsl\n";
	if (!m_combinePS) std::cout << "Failed to load BloomCombine_PS.hlsl\n";

    for (int i = 0; i < 2; ++i) {
        m_tempRT[i] = std::make_unique<RenderTexture>();
        m_tempRT[i]->Initialize(1, 1, RenderTextureType::Tex2D); 
    }

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;       
    desc.ByteWidth = sizeof(BloomCBuffer);  
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;                
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    auto device = DX11Renderer::Instance().GetDevice();
    HRESULT hr = device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
}

void BloomEffect::Render(RenderTexture* src, RenderTexture* dest, const Camera* camera)
{
    if (!src || !dest) return;

    auto& renderer = DX11Renderer::Instance();
    auto context = renderer.GetContext();

    int width = src->GetWidth() / 2;
    int height = src->GetHeight() / 2;
    if (width < 1) width = 1;
    if (height < 1) height = 1;

    if (m_tempRT[0]->GetWidth() != width || m_tempRT[0]->GetHeight() != height)
    {
        m_tempRT[0]->Resize(width, height);
        m_tempRT[1]->Resize(width, height);
    }

    BloomCBuffer data;
    if (camera && m_constantBuffer)
    {
        data.threshold = camera->GetBloomThreshold();
        data.intensity = camera->GetBloomIntensity();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
    }

    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    m_tempRT[0]->Bind();
    m_tempRT[0]->Clear(0, 0, 0, 0);

    if (m_extractPS)
    {
        m_extractPS->Bind();

        // 추출 단계에서는 블러 방향이 필요 없으므로 0으로 초기화 후 전송
        data.blurDir[0] = 0.0f;
        data.blurDir[1] = 0.0f;
        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0); // [중요] GPU로 전송

        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* srcSRV = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srcSRV);

    renderer.DrawFullScreenQuad();
    m_tempRT[0]->Unbind();

    m_tempRT[1]->Bind();
    m_tempRT[1]->Clear(0, 0, 0, 0);

    if (m_blurPS)
    {
        m_blurPS->Bind();

        data.blurDir[0] = 1.0f;
        data.blurDir[1] = 0.0f;
        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);

        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* brightSRV = m_tempRT[0]->GetSRV();
    context->PSSetShaderResources(0, 1, &brightSRV);
    renderer.DrawFullScreenQuad();
    m_tempRT[1]->Unbind();

    m_tempRT[0]->Bind();

    if (m_blurPS)
    {
        data.blurDir[0] = 0.0f;
        data.blurDir[1] = 1.0f;
        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0); // 필수!

        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* blurSRV = m_tempRT[1]->GetSRV();
    context->PSSetShaderResources(0, 1, &blurSRV);
    renderer.DrawFullScreenQuad();
    m_tempRT[0]->Unbind();

    dest->Bind();
    dest->Clear(0, 0, 0, 1);

    if (m_combinePS)
    {
        m_combinePS->Bind();
        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* original = src->GetSRV();
    ID3D11ShaderResourceView* bloom = m_tempRT[0]->GetSRV();

    context->PSSetShaderResources(0, 1, &original);
    context->PSSetShaderResources(1, 1, &bloom);

    renderer.DrawFullScreenQuad();

    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, nullSRVs);

    dest->Unbind();
}