#pragma once
#include "IPostProcess.h"

class Shader;

class GrayScale : public IPostProcess
{
public:
    GrayScale() = default;
    ~GrayScale() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest) override;
    PostProcessType GetType() const override;

private:
    Shader* m_vs = nullptr;
    Shader* m_ps = nullptr;
};