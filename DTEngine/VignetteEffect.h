#pragma once
#include "IPostProcess.h"
#include <wrl/client.h>

struct BloomData
{
    float threshold;
    float intensity;
    float padding1;
    float padding2;
};

class Shader;
class ID3D11Buffer;


class VignetteEffect : public IPostProcess
{
public:
    VignetteEffect() = default;
    ~VignetteEffect() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest, const Camera* camera) override;

    PostProcessType GetType() const override { return PostProcessType::Vignette; }

private:
    Shader* m_ps = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_bloomBuffer = nullptr;


};