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
    void Render(RenderTexture* src, RenderTexture* dest, const Camera* camera) override;
    PostProcessType GetType() const override { return PostProcessType::GrayScale; }

private:
    Shader* m_ps = nullptr;
};