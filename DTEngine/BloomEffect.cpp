#include "pch.h"
#include "BloomEffect.h"
#include "DX11Renderer.h"
#include "ResourceManager.h"
#include "Camera.h"

void BloomEffect::Initialize()
{
    m_extractionPS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomExtract_PS.hlsl");
    m_blurPS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomBlur_PS.hlsl");
    m_combinePS = ResourceManager::Instance().Load<Shader>("Assets/Shaders/BloomCombine_PS.hlsl");

    for (int i = 0; i < 2; ++i) {
        m_tempRT[i] = std::make_unique<RenderTexture>();
        m_tempRT[i]->Initialize(1, 1, RenderTextureType::Tex2D); 
    }

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;        // CPU가 업데이트하고 GPU가 읽음
    desc.ByteWidth = sizeof(BloomCBuffer);   // 구조체 크기
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;                 // UpdateSubresource 사용 시 0
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

    // -----------------------------------------------------------
    // 0. 리사이즈 (블룸은 보통 절반 해상도로 처리하여 성능 확보)
    // -----------------------------------------------------------
    int width = src->GetWidth() / 2;
    int height = src->GetHeight() / 2;
    if (width < 1) width = 1;
    if (height < 1) height = 1;

    if (m_tempRT[0]->GetWidth() != width || m_tempRT[0]->GetHeight() != height)
    {
        m_tempRT[0]->Resize(width, height);
        m_tempRT[1]->Resize(width, height);
    }

    // -----------------------------------------------------------
    // 1. 상수 버퍼 업데이트 (Camera -> Constant Buffer)
    // -----------------------------------------------------------
    if (camera && m_constantBuffer)
    {
        BloomCBuffer data;
        // 카메라에 GetBloomThreshold(), GetBloomIntensity() 함수가 있다고 가정
        data.threshold = camera->GetBloomThreshold();
        data.intensity = camera->GetBloomIntensity();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
    }

    // 공용 샘플러 설정
    ID3D11SamplerState* sampler = renderer.GetSampler(FilterMode::Bilinear, WrapMode::Clamp);
    context->PSSetSamplers(0, 1, &sampler);

    // -----------------------------------------------------------
    // Pass 1: 밝기 추출 (Thresholding)
    // -----------------------------------------------------------
    m_tempRT[0]->Bind();
    m_tempRT[0]->Clear(0, 0, 0, 0);

    if (m_extractPS)
    {
        m_extractPS->Bind();
        // 픽셀 셰이더 슬롯 0번(b0)에 상수 버퍼 바인딩
        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* srcSRV = src->GetSRV();
    context->PSSetShaderResources(0, 1, &srcSRV);

    renderer.DrawFullScreenQuad();
    m_tempRT[0]->Unbind();

    // -----------------------------------------------------------
    // Pass 2: 블러 (Gaussian Blur) - 가로/세로 핑퐁
    // -----------------------------------------------------------
    // 여기서는 간단하게 BloomCBuffer만 다루므로 블러 방향 설정은 생략하거나
    // 블러 셰이더가 별도의 상수를 쓴다면 여기서 추가 바인딩 필요함

    // [가로 블러] RT[0] -> RT[1]
    m_tempRT[1]->Bind();
    m_tempRT[1]->Clear(0, 0, 0, 0);
    if (m_blurPS) m_blurPS->Bind(); // (가로 방향 세팅 필요)

    ID3D11ShaderResourceView* brightSRV = m_tempRT[0]->GetSRV();
    context->PSSetShaderResources(0, 1, &brightSRV);
    renderer.DrawFullScreenQuad();
    m_tempRT[1]->Unbind();

    // [세로 블러] RT[1] -> RT[0] (다시 RT[0]에 저장)
    m_tempRT[0]->Bind();
    if (m_blurPS) m_blurPS->Bind(); // (세로 방향 세팅 필요)

    ID3D11ShaderResourceView* blurSRV = m_tempRT[1]->GetSRV();
    context->PSSetShaderResources(0, 1, &blurSRV);
    renderer.DrawFullScreenQuad();
    m_tempRT[0]->Unbind();

    // -----------------------------------------------------------
    // Pass 3: 최종 합성 (Combine)
    // -----------------------------------------------------------
    dest->Bind();
    dest->Clear(0, 0, 0, 1);

    if (m_combinePS)
    {
        m_combinePS->Bind();
        // 합성 단계에서도 intensity 값을 쓸 수 있으므로 상수 버퍼 바인딩
        context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    }

    ID3D11ShaderResourceView* original = src->GetSRV();        // t0
    ID3D11ShaderResourceView* bloom = m_tempRT[0]->GetSRV();   // t1

    context->PSSetShaderResources(0, 1, &original);
    context->PSSetShaderResources(1, 1, &bloom);

    renderer.DrawFullScreenQuad();

    // SRV 해제 (안하면 다음 프레임에 쓰기 경고 뜸)
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, nullSRVs);

    dest->Unbind();
}