#include "pch.h"

#include <d3d11.h> 
#include <DirectXTK/WICTextureLoader.h> 
#include <DirectXTK/DDSTextureLoader.h> 
#include <fstream>

#include "Texture.h"
#include "DX11Renderer.h"
#include "DXHelper.h"
#include "../ThirdParty/nlohmann/json.hpp"


using json = nlohmann::json;

using Microsoft::WRL::ComPtr;

Texture::Texture() = default;

Texture::~Texture() { Unload(); }

bool Texture::LoadFile(const std::string& fullPath)
{
    auto device = DX11Renderer::Instance().GetDevice();
    auto context = DX11Renderer::Instance().GetContext();
    if (!device) return false;

    LoadMetaData(fullPath);

    std::wstring widestr = std::wstring(fullPath.begin(), fullPath.end());
    HRESULT hr = S_OK;

    std::string ext = fullPath.substr(fullPath.find_last_of(".") + 1);

    if (ext == "dds" || ext == "DDS")
    {
        DirectX::DDS_LOADER_FLAGS ddsFlags = DirectX::DDS_LOADER_DEFAULT; // 이것도 SRGB데이터를 IGNORE하는데도 잘 작동함 어디선가 보간되고 있는듯
        //if (m_bSRGB)
        //    ddsFlags = DirectX::DDS_LOADER_FORCE_SRGB;
        //else
        //    ddsFlags = DirectX::DDS_LOADER_IGNORE_SRGB;

        hr = DirectX::CreateDDSTextureFromFileEx(
            device,
            context,
            widestr.c_str(),
            0, 
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0, 
            0, 
            ddsFlags, 
            m_textureResource.GetAddressOf(),
            m_srv.GetAddressOf()
        );
    }
    else
    {
        DirectX::WIC_LOADER_FLAGS loadFlags = DirectX::WIC_LOADER_IGNORE_SRGB;
        //if (m_bSRGB)
        //    loadFlags = DirectX::WIC_LOADER_FORCE_SRGB;
        //else
        //    loadFlags = DirectX::WIC_LOADER_IGNORE_SRGB; 
        //    이게 왜 false 여야 되는거지 Yena는 그냥 Default 여도 되는데 뭔가 꼬여있다. 
        //    이 말은 지금 무시 했지만 어디선가 보간이 되고 있다는건데 UI만 이해할수가 없네?

        hr = DirectX::CreateWICTextureFromFileEx(
            device, context,
            widestr.c_str(),
            0, 
            D3D11_USAGE_DEFAULT,
            D3D11_BIND_SHADER_RESOURCE,
            0, 
            0, 
            loadFlags,
            m_textureResource.GetAddressOf(),
            m_srv.GetAddressOf()
        );
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

    UpdateSampler();

    return true;
}

bool Texture::SaveFile(const std::string& fullPath) { return false; }

void Texture::Unload()
{
    m_srv.Reset();
    m_textureResource.Reset();
    m_currentSampler = nullptr;
}

void Texture::SaveImportSettings(const std::string& fullPath)
{
    std::string metaPath = fullPath + ".meta";
    json data;

    std::ifstream inFile(metaPath);
    if (inFile.is_open()) { try { inFile >> data; } catch (...) {} inFile.close(); }

    data["FilterMode"] = (int)m_filterMode;
    data["WrapMode"] = (int)m_wrapMode;
    data["SRGB"] = m_bSRGB;

    std::ofstream outFile(metaPath);
    if (outFile.is_open()) outFile << data.dump(4);
}


void Texture::SetFilterMode(FilterMode mode)
{
    if (m_filterMode == mode) return;
    m_filterMode = mode;
    UpdateSampler(); 
}

void Texture::SetWrapMode(WrapMode mode)
{
    if (m_wrapMode == mode) return;
    m_wrapMode = mode;
    UpdateSampler(); 
}

void Texture::UpdateSampler()
{
    m_currentSampler = DX11Renderer::Instance().GetSampler(m_filterMode, m_wrapMode);
}

void Texture::LoadMetaData(const std::string& fullPath)
{
    std::string metaPath = fullPath + ".meta";
    std::ifstream file(metaPath);
    if (file.is_open())
    {
        try {
            json data;
            file >> data;
            if (data.contains("FilterMode")) m_filterMode = (FilterMode)data["FilterMode"];
            if (data.contains("WrapMode")) m_wrapMode = (WrapMode)data["WrapMode"];
            if (data.contains("SRGB")) m_bSRGB = data["SRGB"];
        }
        catch (...) {
            std::cout << "Cannot Load Metadata" << std::endl;
        }
    }
}

