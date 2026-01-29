#pragma once
#include "IPostProcess.h"
#include <wrl/client.h>


class Shader;
struct ID3D11Buffer;


// Update를 받아야 하는데, binding된 Effect cb를 통해서 받고 있긴함. 
// 01_29
// Binding 안되면 이거 실행 안돼서, Camera에 effect component를 넣는 기행이 되는지 물어보자.(?)

class CircleMaskEffect : public IPostProcess
{
public:
    CircleMaskEffect() = default;
    ~CircleMaskEffect() override = default;

    void Initialize() override;
    void Render(RenderTexture* src, RenderTexture* dest, const Camera* camera) override;

    PostProcessType GetType() const override { return PostProcessType::CircleMask; }

private:
    Shader* m_ps = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;


};