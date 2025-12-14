#include "pch.h"
#include "CubeRenderTexture.h"
#include "DX11Renderer.h"
#include <d3d11.h> 

using Microsoft::WRL::ComPtr;

CubeRenderTexture::CubeRenderTexture() = default;
CubeRenderTexture::~CubeRenderTexture() = default;

bool CubeRenderTexture::Initialize(int size)
{
    m_width = size;
    m_height = size;
    auto device = DX11Renderer::Instance().GetDevice();

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = size;
    texDesc.Height = size;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 6;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;

    if (FAILED(device->CreateTexture2D(&texDesc, nullptr, tex2D.GetAddressOf())))
        return false;

    m_textureResource = tex2D;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = 1;
    srvDesc.TextureCube.MostDetailedMip = 0;

    if (FAILED(device->CreateShaderResourceView(m_textureResource.Get(), &srvDesc, m_srv.GetAddressOf())))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.ArraySize = 1;

    for (int i = 0; i < 6; ++i)
    {
        rtvDesc.Texture2DArray.FirstArraySlice = i;
        if (FAILED(device->CreateRenderTargetView(m_textureResource.Get(), &rtvDesc, m_rtvFaces[i].GetAddressOf())))
            return false;
    }

    D3D11_TEXTURE2D_DESC depthDesc = texDesc;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.MiscFlags = 0;

    if (FAILED(device->CreateTexture2D(&depthDesc, nullptr, m_depthTex.GetAddressOf())))
        return false;

    if (FAILED(device->CreateDepthStencilView(m_depthTex.Get(), nullptr, m_dsv.GetAddressOf())))
        return false;

    UpdateSampler();
    return true;
}

ID3D11RenderTargetView* CubeRenderTexture::GetRTV(int faceIndex)
{
    if (faceIndex < 0 || faceIndex >= 6) return nullptr;
    return m_rtvFaces[faceIndex].Get();
}

void CubeRenderTexture::Bind(unsigned int slot)
{
    auto context = DX11Renderer::Instance().GetContext();
    ID3D11ShaderResourceView* srv = m_srv.Get();
    context->PSSetShaderResources(slot, 1, &srv);
}