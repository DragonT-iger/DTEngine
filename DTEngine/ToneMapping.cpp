#include "pch.h"
#include "BloomEffect.h"
#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "ToneMapping.h"
#include "DXHelper.h"

void ToneMapping::Initialize()
{
    m_ToneMappingShader = ResourceManager::Instance().Load<Shader>("Assets/Shaders/ToneMapping_PS.hlsl");
  
    if (!m_ToneMappingShader) std::cout << "Failed to load BloomExtract_PS.hlsl\n";
   
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(ToneMappingCBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    auto device = DX11Renderer::Instance().GetDevice();
    HRESULT hr = device->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);
}

void ToneMapping::Render(RenderTexture* src, RenderTexture* dest, const Camera* camera)
{
    if (!m_ToneMappingShader || !src) return;

    auto& renderer = DX11Renderer::Instance();
    auto context = renderer.GetContext();

    if (m_constantBuffer)
    {
        ToneMappingCBuffer data;
        data.Exposure = 1; 

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
    }

    if (dest) //그리고 UI를 그려야 하니깐 ㅇㅇ 
    {
        dest->Bind();
        dest->Clear(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
      
        ID3D11RenderTargetView* backBufferRTV = renderer.GetBackbufferRTV(); 
        context->OMSetRenderTargets(1, &backBufferRTV, nullptr); // Depth는 필요 없음

        D3D11_VIEWPORT viewport = {};
        viewport.Width = (float)src->GetWidth();
        viewport.Height = (float)src->GetHeight();
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        context->RSSetViewports(1, &viewport);
    }

   
    m_ToneMappingShader->Bind();

  
    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    ID3D11ShaderResourceView* srv = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srv);

    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    renderer.DrawFullScreenQuad();

   
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    if (dest)
    {
        dest->Unbind();
    }
    else
    {
        // 백버퍼의 경우 명시적인 Unbind는 필요 없지만, 
        // 확실히 하기 위해 RTV를 해제해 줄 수도 있음.
        ID3D11RenderTargetView* nullRTV = nullptr;
        context->OMSetRenderTargets(1, &nullRTV, nullptr);
    }
    
}
