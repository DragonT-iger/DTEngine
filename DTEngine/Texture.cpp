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

    LoadMetaData(fullPath);
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
        }
        catch (...) {}
    }
}

