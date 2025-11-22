#pragma once
#include <d3d11.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class RenderTexture
{
public:
    RenderTexture();
    ~RenderTexture();

    bool Initialize(int width, int height);

    void Resize(int width, int height);

    void Bind();

    void Unbind();

    void Clear(float r, float g, float b, float a);

    ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    int m_width = 0;
    int m_height = 0;

    ComPtr<ID3D11Texture2D>          m_renderTargetTexture;
    ComPtr<ID3D11RenderTargetView>   m_rtv;
    ComPtr<ID3D11ShaderResourceView> m_srv;

    ComPtr<ID3D11Texture2D>          m_depthStencilTexture;
    ComPtr<ID3D11DepthStencilView>   m_dsv;

    D3D11_VIEWPORT m_viewport = {};
};