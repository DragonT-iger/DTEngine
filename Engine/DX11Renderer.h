#pragma once
class DX11Renderer
{
public:
	DX11Renderer(HWND* hwnd) : m_hWnd(hwnd) {}




private:

	HRESULT CreateDeviceSwapChain(HWND hWnd);
	HRESULT CreateRenderTarget();




	HWND* m_hWnd;

};

