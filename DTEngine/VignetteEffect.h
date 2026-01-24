#pragma once
#include "IPostProcess.h"
#include <wrl/client.h>

class Shader;

class VignetteEffect : public IPostProcess
{
public:
    VignetteEffect() = default;
    ~VignetteEffect() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest) override;

    PostProcessType GetType() const override { return PostProcessType::Vignette; }

private:
    Shader* m_ps = nullptr;
};