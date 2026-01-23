#pragma once
#include <wrl.h>
#include "Texture.h"
#include "RenderViewport.h"


using Microsoft::WRL::ComPtr;

enum class RenderTextureType
{
    Tex2D,
    CubeMap
};

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

class RenderTexture : public Texture
{
public:
    RenderTexture();
    ~RenderTexture();

    bool Initialize(int width, int height, RenderTextureType type = RenderTextureType::Tex2D, bool isSRGB = false, bool enableAA = false);

    void Resize(int width, int height);

    void Bind(int faceIndex = 0);

    void Unbind();

    void Clear(float r, float g, float b, float a, int faceIndex = 0);

    void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

    ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

    ID3D11RenderTargetView* GetRTV(int faceIndex = 0) const {
        if (faceIndex < m_faceRTVs.size()) return m_faceRTVs[faceIndex].Get();
        return nullptr;
    }

	ID3D11Texture2D* GetTexture() const { return m_renderTargetTexture.Get(); }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }


private:
    int m_width = 0;
    int m_height = 0;

    bool m_isSRGB;
    bool m_enableAA = false;
    int m_msaa = 1;
	int m_msaaQuality = 0;

    ComPtr<ID3D11Texture2D>          m_renderTargetTexture;

    ComPtr<ID3D11Texture2D>          m_msaaTexture;
    ComPtr<ID3D11RenderTargetView>   m_msaaRTV;


    ComPtr<ID3D11Texture2D>          m_depthStencilTexture;
    ComPtr<ID3D11DepthStencilView>   m_dsv;

    RenderTextureType m_type = RenderTextureType::Tex2D;
    std::vector<ComPtr<ID3D11RenderTargetView>> m_faceRTVs;

    RenderViewport m_viewport = {};
};