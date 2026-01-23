#pragma once
#include "IPostProcess.h"

class Shader;

class Vignette : public IPostProcess
{
public:
    Vignette() = default;
    ~Vignette() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest) override;
    PostProcessType GetType() const override;

private:
    Shader* m_vs = nullptr;
    Shader* m_ps = nullptr;
};