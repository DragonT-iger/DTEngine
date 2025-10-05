#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
using namespace DirectX;
using namespace Microsoft::WRL;


class DX11Renderer
{
public:
	bool Init(HWND hwnd);

	HRESULT CreateDeviceSwapChain(HWND hWnd);
	HRESULT CreateRenderTarget();
	void SetViewPort();
	int ClearBackBuffer(XMFLOAT4 col);
	int Flip();

private:
	ComPtr<IDXGISwapChain>         m_pSwapChain;
	ComPtr<ID3D11Device>           m_pDevice;
	ComPtr<ID3D11DeviceContext>    m_pDXDC;
	ComPtr<ID3D11RenderTargetView> m_pRTView;

	bool m_bWindowMode = true;
	bool m_bVSync = false; // 나는 수직동기화가 싫워요

	struct { UINT Width; UINT Height; DXGI_FORMAT Format; } m_Mode = { 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM };

	D3D_FEATURE_LEVEL m_FeatureLevels = D3D_FEATURE_LEVEL_11_0;
};

