#pragma once

#include <cstdint>
#include <wrl/client.h> 
#include "Singleton.h"
#include <SimpleMath.h>

using Matrix = DirectX::SimpleMath::Matrix;


struct HWND__;
using HWND = HWND__*;


struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain1;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;

class DX11Renderer : public Singleton<DX11Renderer>
{
public:
    DX11Renderer();
    ~DX11Renderer();

    bool Initialize(HWND hwnd, int width, int height, bool vsync = false);
    void Resize(int width, int height);

    void UpdateFrameCBuffer(const Matrix& viewTM, const Matrix& projectionTM);

    void BeginFrame(const float clearColor[4]);
    void EndFrame();
    void Present();

    void SetFullscreen(bool enable);
    void Destroy();

    void SetRenderTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv = nullptr);

    HWND GetHwnd();
    ID3D11Device* GetDevice()  const;
    ID3D11DeviceContext* GetContext() const;
    ID3D11RenderTargetView* GetBackbufferRTV() const;

    int  Width()  const { return m_width; }
    int  Height() const { return m_height; }

    bool GetVsync() { return m_vsync; }
    void SetVsync(bool vsync) { m_vsync = vsync; }


    float GetAspectRatio() const
    {
        if (m_height == 0) return 1.0f;
        return static_cast<float>(m_width) / static_cast<float>(m_height);
    }


    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }
    void SetWidth(int width) { m_width = width; }
    void SetHeight(int height) { m_height = height; }

    void UpdateLights(const std::vector<class Light*>& lights);

    void ResetRenderState();

private:
    bool CreateDeviceAndSwapchain();
    void CreateBackbuffers(int width, int height);
    void ReleaseBackbuffers();


    __declspec(align(16))
        struct CBuffer_Frame_Data
    {
        Matrix ViewTM;
        Matrix ProjectionTM;
    };

    struct LightData
    {
        DirectX::SimpleMath::Vector4 PositionRange;  // xyz: 위치, w: 범위(Range)
        DirectX::SimpleMath::Vector4 DirectionType;  // xyz: 방향, w: 타입(0=Dir, 1=Point)
        DirectX::SimpleMath::Vector4 ColorIntensity; // xyz: 색상, w: 강도(Intensity)
    };

	constexpr static int MAX_LIGHTS = 4;

    __declspec(align(16))
        struct CBuffer_GlobalLight
    {
        LightData Lights[MAX_LIGHTS];           // 배열로 선언
        int ActiveCount;                        // 현재 활성화된 조명 개수
        DirectX::SimpleMath::Vector3 Padding;   // 16바이트 정렬 맞춤
    };


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

    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_frame; 
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_cbuffer_lights;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_defaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_defaultRasterizerState;

    // State
    int   m_width = 0;
    int   m_height = 0;
    bool  m_vsync = false;

};
