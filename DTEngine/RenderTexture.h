#pragma once
#include <wrl.h>
#include "Texture.h"
#include "RenderViewport.h"


using Microsoft::WRL::ComPtr;



struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

class RenderTexture : public Texture
{
public:
    RenderTexture();
    ~RenderTexture();

    bool Initialize(int width, int height);

    void Resize(int width, int height);

    void Bind();

    void Unbind();

    void Clear(float r, float g, float b, float a);

    void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

    ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }


private:
    int m_width = 0;
    int m_height = 0;

    ComPtr<ID3D11Texture2D>          m_renderTargetTexture;
    ComPtr<ID3D11RenderTargetView>   m_rtv;
    //ComPtr<ID3D11ShaderResourceView> m_srv;

    ComPtr<ID3D11Texture2D>          m_depthStencilTexture;
    ComPtr<ID3D11DepthStencilView>   m_dsv;

    RenderViewport m_viewport = {};
};