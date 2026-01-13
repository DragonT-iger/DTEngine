#include "pch.h"
#include "RenderTexture.h"
#include <d3d11.h>
#include "DX11Renderer.h" 

RenderTexture::RenderTexture() = default;
RenderTexture::~RenderTexture() = default;

bool RenderTexture::Initialize(int width, int height, RenderTextureType type, bool isSRGB)
{
    m_width = width;
    m_height = height;
    m_type = type;

    ID3D11Device* device = DX11Renderer::Instance().GetDevice();

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0; 
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;

    if (m_type == RenderTextureType::CubeMap)
    {
        textureDesc.ArraySize = 6;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    }
    else
    {
        textureDesc.ArraySize = 1;
        textureDesc.MiscFlags = 0;
    }

    if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, m_renderTargetTexture.GetAddressOf())))
        return false;

    m_renderTargetTexture.As(&m_textureResource);

    m_faceRTVs.clear();

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    if (isSRGB) {
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
    else {
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    if (m_type == RenderTextureType::CubeMap)
    {
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray.MipSlice = 0;
        rtvDesc.Texture2DArray.ArraySize = 1;

        for (int i = 0; i < 6; ++i)
        {
            rtvDesc.Texture2DArray.FirstArraySlice = i; // i번째 면
            ComPtr<ID3D11RenderTargetView> rtv;
            if (FAILED(device->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, rtv.GetAddressOf())))
                return false;
            m_faceRTVs.push_back(rtv);
        }
    }
    else
    {
        // 기존 2D 텍스처
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        ComPtr<ID3D11RenderTargetView> rtv;
        if (FAILED(device->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, rtv.GetAddressOf())))
            return false;
        m_faceRTVs.push_back(rtv);
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (m_type == RenderTextureType::CubeMap)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = 1;
        srvDesc.TextureCube.MostDetailedMip = 0;
    }
    else
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }

    if (FAILED(device->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_srv.GetAddressOf())))
        return false;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(device->CreateTexture2D(&depthDesc, nullptr, m_depthStencilTexture.GetAddressOf())))
        return false;

    if (FAILED(device->CreateDepthStencilView(m_depthStencilTexture.Get(), nullptr, m_dsv.GetAddressOf())))
        return false;

    // Viewport 설정
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    return true;
}

void RenderTexture::Resize(int width, int height)
{
    if (width <= 0 || height <= 0) return;
    if (m_width == width && m_height == height) return;

    m_renderTargetTexture.Reset();
    m_srv.Reset();
    m_textureResource.Reset();
    m_depthStencilTexture.Reset();
    m_dsv.Reset();

    Initialize(width, height);
}

void RenderTexture::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
    m_viewport.TopLeftX = x;
    m_viewport.TopLeftY = y;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinDepth = minDepth;
    m_viewport.MaxDepth = maxDepth;
}

void RenderTexture::Bind(int faceIndex)
{
    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();

    if (faceIndex >= 0 && faceIndex < m_faceRTVs.size())
    {
        ID3D11RenderTargetView* rtv = m_faceRTVs[faceIndex].Get();
        context->OMSetRenderTargets(1, &rtv, m_dsv.Get());
        context->RSSetViewports(1, reinterpret_cast<const D3D11_VIEWPORT*>(&m_viewport));
    }
}

void RenderTexture::Unbind()
{
    DX11Renderer::Instance().SetRenderTarget(nullptr);
}

void RenderTexture::Clear(float r, float g, float b, float a, int faceIndex)
{
    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();
    const float color[4] = { r, g, b, a };

    if (faceIndex >= 0 && faceIndex < m_faceRTVs.size())
    {
        context->ClearRenderTargetView(m_faceRTVs[faceIndex].Get(), color);
        context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}