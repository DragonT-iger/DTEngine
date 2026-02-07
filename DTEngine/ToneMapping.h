#pragma once
#include "IPostProcess.h"
#include "RenderTexture.h"
#include "Shader.h"
#include <memory>

struct ToneMappingCBuffer
{
    float Exposure = 1;
    float pad[3];
};


class ToneMapping : public IPostProcess
{
public:
    ToneMapping() = default;
    ~ToneMapping() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest, const Camera* camera) override;
    PostProcessType GetType() const override { return PostProcessType::ToneMapping; }

private:
    Shader* m_ToneMappingShader = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
};