#pragma once
#include <vector>
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include "IPostProcess.h"

class RenderTexture;

class PostProcessManager
{
public:
    PostProcessManager();
    ~PostProcessManager();

    void Initialize(int width, int height);
    void Resize(int width, int height);

    void Execute(RenderTexture* sceneTexture, ID3D11RenderTargetView* backBufferRTV, uint32_t activeEffectsMask, Camera* camera, int width, int height);

    template <typename T>
    T* AddEffect()
    {
        static_assert(std::is_base_of<IPostProcess, T>::value, "T must derive from IPostProcess");

        auto effect = std::make_shared<T>();
        effect->Initialize();
        m_effects.push_back(effect);
        return effect.get();
    }

private:
    void Blit(RenderTexture* src, ID3D11RenderTargetView* destRTV, int width, int height);

private:
    std::vector<std::shared_ptr<IPostProcess>> m_effects;

    std::unique_ptr<RenderTexture> m_tempRT[2];

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_fullscreenVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_copyPS;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointSampler;
};