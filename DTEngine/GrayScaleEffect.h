#pragma once
#include "IPostProcess.h"
#include "Shader.h"
#include <memory>

class GrayScaleEffect : public IPostProcess
{
public:
    GrayScaleEffect() = default;
    ~GrayScaleEffect() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest) override;
    PostProcessType GetType() const override { return PostProcessType::GrayScale; }

private:
    Shader* m_vs = nullptr;
    Shader* m_ps = nullptr;
};