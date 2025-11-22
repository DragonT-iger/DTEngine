#include "pch.h" 
#include "DX11Renderer.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <wrl/client.h>
#include <cassert>
#include <stdexcept>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")




#include "DXHelper.h"


using Microsoft::WRL::ComPtr;

DX11Renderer::DX11Renderer() = default;
DX11Renderer::~DX11Renderer() { Destroy(); }

bool DX11Renderer::Initialize(HWND hwnd, int width, int height, bool vsync)
{
    m_hwnd = hwnd; m_width = width; m_height = height; m_vsync = vsync;
    if (!m_hwnd) return false;
    if (!CreateDeviceAndSwapchain()) return false;


    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(CBuffer_Frame_Data);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_device->CreateBuffer(&cbDesc, nullptr, m_cbuffer_frame.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    CreateBackbuffers(width, height);
    return true;
}

void DX11Renderer::Resize(int width, int height)
{
    if (!m_swapchain || width <= 0 || height <= 0) return;
    if (width == m_width && height == m_height)    return;

    ReleaseBackbuffers();
    const HRESULT hr = m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    DXHelper::ThrowIfFailed(hr);

    CreateBackbuffers(width, height);
    m_width = width; m_height = height;
}
 
void DX11Renderer::UpdateFrameCBuffer(const Matrix& viewTM, const Matrix& projectionTM)
{
    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = m_context->Map(m_cbuffer_frame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    CBuffer_Frame_Data* dataPtr = static_cast<CBuffer_Frame_Data*>(mappedData.pData);

    dataPtr->ViewTM = viewTM.Transpose();
    dataPtr->ProjectionTM = projectionTM.Transpose();

    m_context->Unmap(m_cbuffer_frame.Get(), 0);

    m_context->VSSetConstantBuffers(0, 1, m_cbuffer_frame.GetAddressOf());
    m_context->PSSetConstantBuffers(0, 1, m_cbuffer_frame.GetAddressOf());
}

void DX11Renderer::BeginFrame(const float clearColor[4])
{
    assert(m_context && m_rtv);

    bool clearDepth = true;

    ID3D11RenderTargetView* rtvs[1] = { m_rtv.Get() };
    m_context->OMSetRenderTargets(1, rtvs, clearDepth ? m_dsv.Get() : nullptr);
    m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);
    if (clearDepth && m_dsv)
        m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(m_width);
    vp.Height = static_cast<float>(m_height);
    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);
}

void DX11Renderer::EndFrame()
{

    //std::cout << m_width << " " << m_height << std::endl;
    // 필요 시 파이프라인 언바인드/커맨드 종료 등
}

void DX11Renderer::Present()
{
    if (!m_swapchain) return;
    m_swapchain->Present(GetVsync(), 0);
}

void DX11Renderer::SetFullscreen(bool enable)
{
    if (!m_swapchain) return;
    m_swapchain->SetFullscreenState(enable ? TRUE : FALSE, nullptr);
}

void DX11Renderer::Destroy()
{
    ReleaseBackbuffers();
    m_cbuffer_frame.Reset();
    m_swapchain.Reset();
    m_context.Reset();
    m_device.Reset();
    m_hwnd = nullptr;
}

void DX11Renderer::SetRenderTarget(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
    if (rtv == nullptr)
    {
        ID3D11RenderTargetView* rtvs[1] = { m_rtv.Get() };
        m_context->OMSetRenderTargets(1, rtvs, m_dsv.Get());

        D3D11_VIEWPORT vp{};
        vp.Width = static_cast<float>(m_width);
        vp.Height = static_cast<float>(m_height);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_context->RSSetViewports(1, &vp);
    }
    else
    {
        ID3D11RenderTargetView* rtvs[1] = { rtv };
        m_context->OMSetRenderTargets(1, rtvs, dsv);
    }
}

HWND DX11Renderer::GetHwnd()
{
    return m_hwnd;
}

ID3D11Device* DX11Renderer::GetDevice() const { return m_device.Get(); }
ID3D11DeviceContext* DX11Renderer::GetContext() const { return m_context.Get(); }
ID3D11RenderTargetView* DX11Renderer::GetBackbufferRTV() const { return m_rtv.Get(); }

bool DX11Renderer::CreateDeviceAndSwapchain()
{
    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL outLevel{};

    ComPtr<ID3D11Device> dev;
    ComPtr<ID3D11DeviceContext> ctx;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        levels, _countof(levels), D3D11_SDK_VERSION,
        dev.GetAddressOf(), &outLevel, ctx.GetAddressOf());

    DXHelper::ThrowIfFailed(hr);

    

    ComPtr<IDXGIDevice>   dxgiDev;  dev.As(&dxgiDev);
    ComPtr<IDXGIAdapter>  adapter;  dxgiDev->GetAdapter(&adapter);
    ComPtr<IDXGIFactory2> factory;  adapter->GetParent(__uuidof(IDXGIFactory2), &factory);

    DXGI_SWAP_CHAIN_DESC1 scd{};
    scd.Width = 0;
    scd.Height = 0;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc = { 1, 0 };
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd{};
    fsd.Windowed = TRUE;

    ComPtr<IDXGISwapChain1> sc1;
    hr = factory->CreateSwapChainForHwnd(dev.Get(), m_hwnd, &scd, &fsd, nullptr, sc1.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    // Alt+Enter 기본 전체화면 토글 막기(직접 처리)
    //factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    m_device = std::move(dev);
    m_context = std::move(ctx);
    m_swapchain = std::move(sc1);
    return true;
}

void DX11Renderer::CreateBackbuffers(int width, int height)
{
    assert(m_swapchain && m_device);
    ReleaseBackbuffers();

    // Backbuffer RTV
    ComPtr<ID3D11Texture2D> backTex;
    HRESULT hr = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(backTex.GetAddressOf()));
    DXHelper::ThrowIfFailed(hr);

    m_backbufferTex = std::move(backTex);
    hr = m_device->CreateRenderTargetView(m_backbufferTex.Get(), nullptr, m_rtv.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    D3D11_TEXTURE2D_DESC rtvDesc;
    m_backbufferTex->GetDesc(&rtvDesc);


    // Depth/Stencil
    D3D11_TEXTURE2D_DESC ds{};
    ds.Width = rtvDesc.Width; ds.Height = rtvDesc.Height; ds.MipLevels = 1; ds.ArraySize = 1;
    ds.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ds.SampleDesc = { 1, 0 };
    ds.Usage = D3D11_USAGE_DEFAULT;
    ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_device->CreateTexture2D(&ds, nullptr, m_depthTex.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    hr = m_device->CreateDepthStencilView(m_depthTex.Get(), nullptr, m_dsv.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    m_width = rtvDesc.Width;
    m_height = rtvDesc.Height;



}

void DX11Renderer::ReleaseBackbuffers()
{
    m_dsv.Reset();
    m_depthTex.Reset();
    m_rtv.Reset();
    m_backbufferTex.Reset();
}
