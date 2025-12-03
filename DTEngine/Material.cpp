#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h" 
#include "DX11Renderer.h"    
#include "DXHelper.h"
#include "AssetDatabase.h" 

#include <d3d11.h>
#include <SimpleMath.h>
#include <fstream>
#include <iostream>

#include "../ThirdParty/nlohmann/json.hpp"
using json = nlohmann::json;

using Microsoft::WRL::ComPtr;

__declspec(align(16))
struct CBuffer_Object_Data
{
    Matrix WorldTM;
    Matrix WorldInverseTransposeTM;
};

Material::Material() = default;
Material::~Material() { Unload(); }

bool Material::LoadFile(const std::string& fullPath)
{
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open material file: " << fullPath << std::endl;
        return false;
    }

    json data;
    try
    {
        file >> data;
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "JSON Parse Error in " << fullPath << ": " << e.what() << std::endl;
        return false;
    }

    if (data.contains("ShaderID"))
    {
        uint64_t shaderID = data["ShaderID"];
        std::string shaderPath = AssetDatabase::Instance().GetPathFromID(shaderID);

        if (!shaderPath.empty())
        {
            m_shader = ResourceManager::Instance().Load<Shader>(shaderPath);
        }
        else
        {
            std::cerr << "Warning: Shader ID " << shaderID << " not found in AssetDatabase." << std::endl;
        }
    }

    if (data.contains("Textures"))
    {
        for (auto& [key, value] : data["Textures"].items())
        {
            int slot = std::stoi(key);
            uint64_t texID = value;

            std::string texPath = AssetDatabase::Instance().GetPathFromID(texID);

            if (!texPath.empty())
            {
                Texture* tex = ResourceManager::Instance().Load<Texture>(texPath);
                if (tex)
                {
                    SetTexture(slot, tex);
                }
            }
            else
            {
                std::cerr << "Warning: Texture ID " << texID << " at slot " << slot << " not found." << std::endl;
            }
        }
    }

    ID3D11Device* device = DX11Renderer::Instance().GetDevice();
    if (!device) return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(CBuffer_Object_Data);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, m_cbuffer_object.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    return true;
}

bool Material::SaveFile(const std::string& fullPath)
{
    json data;

    if (m_shader)
    {
        uint64_t id = m_shader->GetMeta().guid;
        data["ShaderID"] = id;
    }
    else
    {
        data["ShaderID"] = 0;
    }

    json texData;
    for (auto const& [slot, tex] : m_textures)
    {
        if (tex)
        {
            uint64_t id = tex->GetMeta().guid;
            texData[std::to_string(slot)] = id;
        }
    }
    data["Textures"] = texData;

    std::ofstream file(fullPath);
    if (!file.is_open()) return false;

    file << data.dump(4); 
    return true;
}

void Material::Unload()
{
    m_cbuffer_object.Reset();
    m_shader = nullptr;
    m_textures.clear();
}

void Material::SetTexture(int slot, Texture* texture)
{
    if (texture) m_textures[slot] = texture;
    else m_textures.erase(slot);
}

void Material::Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM)
{
    if (!m_shader || !m_cbuffer_object) return;

    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();
    if (!context) return;

    m_shader->Bind();

    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = context->Map(m_cbuffer_object.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    CBuffer_Object_Data* dataPtr = static_cast<CBuffer_Object_Data*>(mappedData.pData);
    dataPtr->WorldTM = worldTM.Transpose();
    dataPtr->WorldInverseTransposeTM = worldInverseTransposeTM.Transpose();

    context->Unmap(m_cbuffer_object.Get(), 0);

    context->VSSetConstantBuffers(1, 1, m_cbuffer_object.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_cbuffer_object.GetAddressOf());

    for (auto const& [slot, tex] : m_textures)
    {
        if (tex)
        {
            ID3D11ShaderResourceView* srv = tex->GetSRV();
            context->PSSetShaderResources(slot, 1, &srv);
        }
    }
}