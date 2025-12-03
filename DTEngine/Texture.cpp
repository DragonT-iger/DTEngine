#include "pch.h"
#include "Texture.h"
#include "DX11Renderer.h"
#include "DXHelper.h"

#include <d3d11.h> 
#include <WICTextureLoader.h> 
#include <DDSTextureLoader.h> 

using Microsoft::WRL::ComPtr;

Texture::Texture() = default;

Texture::~Texture() { Unload(); }

bool Texture::LoadFile(const std::string& fullPath)
{
    auto device = DX11Renderer::Instance().GetDevice();
    auto context = DX11Renderer::Instance().GetContext();
    if (!device) return false;

    std::wstring widestr = std::wstring(fullPath.begin(), fullPath.end());
    HRESULT hr = S_OK;

    std::string ext = fullPath.substr(fullPath.find_last_of(".") + 1);
    if (ext == "dds" || ext == "DDS")
    {
        hr = DirectX::CreateDDSTextureFromFile(device, context, widestr.c_str(),
            m_textureResource.GetAddressOf(), m_srv.GetAddressOf());
    }
    else
    {
        hr = DirectX::CreateWICTextureFromFile(device, context, widestr.c_str(),
            m_textureResource.GetAddressOf(), m_srv.GetAddressOf());
    }

    if (FAILED(hr)) return false;

    if (m_textureResource)
    {
        CD3D11_TEXTURE2D_DESC desc;
        ComPtr<ID3D11Texture2D> tex2D;
        if (SUCCEEDED(m_textureResource.As(&tex2D)))
        {
            tex2D->GetDesc(&desc);
            m_width = desc.Width;
            m_height = desc.Height;
        }
    }

    return true;
}

bool Texture::SaveFile(const std::string& fullPath) { return false; }

void Texture::Unload()
{
    m_srv.Reset();
    m_textureResource.Reset();
}