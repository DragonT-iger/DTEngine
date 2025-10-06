#pragma once

#include <cstdint>
#include <wrl/client.h> 



struct HWND__;
using HWND = HWND__*;


struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class DX11Renderer
{
public:
    DX11Renderer();
    ~DX11Renderer();

    bool Initialize(HWND hwnd, int width, int height, bool vsync = true);
    void Resize(int width, int height);

    void BeginFrame(const float clearColor[4]/*, bool clearDepth = true*/);
    void EndFrame();
    void Present();

    void SetFullscreen(bool enable);
    void Destroy();


    HWND GetHwnd();
    ID3D11Device* GetDevice()  const;
    ID3D11DeviceContext* GetContext() const;
    ID3D11RenderTargetView* GetBackbufferRTV() const;

    int  Width()  const { return m_width; }
    int  Height() const { return m_height; }
    bool IsVSync() const { return m_vsync; }
    void ToggleVSync() { m_vsync = !m_vsync; }

private:
    bool CreateDeviceAndSwapchain();
    void CreateBackbuffers(int width, int height);
    void ReleaseBackbuffers();

private:
    // Platform
    HWND m_hwnd = nullptr;

    // D3D11 core
    Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swapchain;

    // Backbuffer & Depth
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_backbufferTex; 
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthTex;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

    // State
    int   m_width = 0;
    int   m_height = 0;
    bool  m_vsync = true;
};
