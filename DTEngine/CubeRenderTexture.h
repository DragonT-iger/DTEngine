#pragma once
#include "Texture.h"
#include <wrl/client.h>

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

class CubeRenderTexture : public Texture
{
public:
    CubeRenderTexture();
    virtual ~CubeRenderTexture();

    bool Initialize(int size);

    ID3D11RenderTargetView* GetRTV(int faceIndex);

    ID3D11DepthStencilView* GetDSV() { return m_dsv.Get(); }

    void Bind(unsigned int slot);

private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtvFaces[6];
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
};