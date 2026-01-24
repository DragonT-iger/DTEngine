#pragma once
#include "IPostProcess.h"
#include "RenderTexture.h"
#include "Shader.h"
#include <memory>

struct BloomCBuffer
{
    float threshold;  
    float intensity;  
    float padding[2]; 
};


class BloomEffect : public IPostProcess
{
public:
    BloomEffect() = default;
    ~BloomEffect() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest , const Camera* camera) override;
    PostProcessType GetType() const override { return PostProcessType::Bloom; }

private:
    Shader* m_extractPS = nullptr; // 밝기 추출 셰이더
    Shader* m_blurPS = nullptr;    // 블러 셰이더
    Shader* m_combinePS = nullptr; // 합성 셰이더   

	std::unique_ptr<RenderTexture> m_tempRT[2]; //핑퐁용 임시 렌더 타겟

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
};