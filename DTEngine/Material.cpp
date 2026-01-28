 #include "pch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h" 
#include "DX11Renderer.h"    
#include "DXHelper.h"
#include "AssetDatabase.h" 
#include "IDManager.h"

#include <d3d11.h>
#include <fstream>
#include <iostream>


#include "SimpleMathHelper.h"
#include "../ThirdParty/nlohmann/json.hpp"
#include "Image.h"

#include "Texture_ID_Manager.h"


using json = nlohmann::json;

using Microsoft::WRL::ComPtr;



Material::Material() {
    m_textures.resize(MAX_TEXTURE_SLOTS, nullptr);
}
Material::~Material() { Unload(); }


void Material::SetDefaultShader()
{
    std::string defaultShaderPath = "Assets/Shaders/Error_VS.hlsl";
    m_shader = ResourceManager::Instance().Load<Shader>(defaultShaderPath);
}

bool Material::LoadFile(const std::string& fullPath)
{
    std::filesystem::path path(fullPath);
    path.replace_extension(".mat");
    std::string finalPath = path.string();

    std::ifstream file(finalPath);

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

    //★ 음... Texture로 Shader를 판별해주는 느낌... 
    if (data.contains("ShaderID"))
    {
        uint64_t shaderID = data["ShaderID"];
        std::string shaderPath = AssetDatabase::Instance().GetPathFromID(shaderID);

        if (!shaderPath.empty())
        {
            std::string shaderPath = AssetDatabase::Instance().GetPathFromID(shaderID);
            if (!shaderPath.empty())
            {
                m_shader = ResourceManager::Instance().Load<Shader>(shaderPath);
            }
            else
            {
                std::cerr << "Warning: Shader ID " << shaderID << " path not found. Using default." << std::endl;
                SetDefaultShader();
            }
        }
        else
        {
            std::cerr << "Warning: Shader ID " << shaderID << " not found in AssetDatabase." << std::endl;
            SetDefaultShader();
        }
    }
    else {
		std::cerr << "Shader File Shit" << std::endl;
    }

    if (data.contains("Color"))
    {
        std::vector<float> colorVec = data["Color"];
        if (colorVec.size() == 4)
        {
            m_data.Color.x = colorVec[0];
            m_data.Color.y = colorVec[1];
            m_data.Color.z = colorVec[2];
            m_data.Color.w = colorVec[3];
        }
    }
    if (data.contains("UVTransform"))
    {
        std::vector<float> uvVec = data["UVTransform"];
        if (uvVec.size() == 4)
        {
            m_data.UVTransform.x = uvVec[0]; 
            m_data.UVTransform.y = uvVec[1]; 
            m_data.UVTransform.z = uvVec[2]; 
            m_data.UVTransform.w = uvVec[3]; 
        }
    }
    else
    {
        m_data.UVTransform = Vector4(1.0f, 1.0f, 0.0f, 0.0f);
    }
    if (data.contains("RenderMode"))
    {
        m_renderMode = (RenderMode)data["RenderMode"];
    }
    else
    {
        m_renderMode = RenderMode::Opaque;
    }

    if (data.contains("CullMode"))
    {
        m_cullMode = (CullMode)data["CullMode"];
    }
    else
    {
        m_cullMode = CullMode::Back;
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

    if (data.contains("ShadowBias"))
    {
        m_data.Shadow_Bias = data["ShadowBias"];
    }
    else
    {
        m_data.Shadow_Bias = 0.005f; 
    }


    if (data.contains("ShadowScale"))
    {
        m_data.Shadow_Scale = data["ShadowScale"];
    }
    else
    {
        m_data.Shadow_Scale = 1.0f;
    }

    ID3D11Device* device = DX11Renderer::Instance().GetDevice();
    if (!device) return false;

    if (!m_textures.empty() && m_textures[0] != nullptr) //5장이 모두 있는 경우 pbr else default shader 
    {
        m_data.UseTexture = 1;
    }
    else
        m_data.UseTexture = 0;

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
        std::cerr << "Error: Failed to save material file: " << fullPath << std::endl;

        data["ShaderID"] = 0;
    }

    data["Color"] = { m_data.Color.x, m_data.Color.y, m_data.Color.z, m_data.Color.w };

    data["UVTransform"] = {
        m_data.UVTransform.x, m_data.UVTransform.y,
        m_data.UVTransform.z, m_data.UVTransform.w
    };

    data["RenderMode"] = (int)m_renderMode;
    data["CullMode"] = (int)m_cullMode;

    json texData;
    for (int slot = 0; slot < m_textures.size(); ++slot)
    {
        if (m_textures[slot])
        {
            uint64_t id = m_textures[slot]->GetMeta().guid;
            texData[std::to_string(slot)] = id;
        }
    }
    data["Textures"] = texData;
    data["ShadowBias"] = m_data.Shadow_Bias;
    data["ShadowScale"] = m_data.Shadow_Scale;

    std::ofstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "Error: Failed to save material file: " << fullPath << std::endl;
        return false;
    }

    file << data.dump(4);
    return true;
}

void Material::Unload()
{
    m_shader = nullptr;
    m_textures.clear();
    m_textures.resize(MAX_TEXTURE_SLOTS, nullptr);
}

bool Material::SetTexture(int slot, Texture* texture)
{
    if (m_textures[slot] == texture) {
        return false;
    }

    if (slot >= m_textures.size())
    {
        m_textures.resize(slot + 1, nullptr);
    }

    if (texture) m_textures[slot] = texture;
    
    m_textures[slot] = texture;

    if (!m_textures.empty() && m_textures[0] != nullptr)
    {
        m_data.UseTexture = 1;
    }
    else
    {
        m_data.UseTexture = 0;
    }
    UpdateTextureBatchID();

	return true;
}

Material* Material::Clone()
{
    Material* newMat = new Material();

    newMat->m_shader = m_shader;
    newMat->m_textures = m_textures; 
    newMat->m_data = m_data;        
    newMat->m_cullMode = m_cullMode;
    newMat->m_renderMode = m_renderMode;
	newMat->m_data.Shadow_Bias = m_data.Shadow_Bias;

    newMat->UpdateTextureBatchID();

    return newMat;
}

Texture* Material::GetTexture(int slot) const
{
    if (slot < 0 || slot >= m_textures.size())
        return nullptr;

    return m_textures[slot];
}

bool Material::SetColor(const Vector4& color)
{
    if (m_data.Color == color)
    {
        return false;
    }

    m_data.Color = color;

    return true;
}

Vector4 Material::GetColor() const
{
    return Vector4(m_data.Color);
}

float Material::GetShadowScale() const
{
    return m_data.Shadow_Scale;
}

void Material::SetShadowScale(float Scale)
{
    m_data.Shadow_Scale = Scale;
}

void Material::SetShadowBias(float bias)
{
	m_data.Shadow_Bias = bias;
}

float Material::GetShadowBias() const
{
    return m_data.Shadow_Bias;
}

uint16_t Material::GetShaderID()
{
    return m_shader->GetID();
}

//Texture는 여러 장이니깐. 조합한 ID를 반환.
uint64_t Material::GetTextureID()
{
    return m_textureBatchID;
}

void Material::UpdateTextureBatchID()
{
    std::vector<uint64_t> textureAddresses;
    textureAddresses.reserve(m_textures.size());

    for (const auto& tex : m_textures)
    {
        //주소값으로 판단. 
        if (tex) textureAddresses.push_back(reinterpret_cast<uint64_t>(tex));
        
        else  textureAddresses.push_back(0); // 텍스쳐 없어도 0으로
       
    }

    m_textureBatchID = TextureCombinationManager::GetID(textureAddresses);

}


//일단 기존 LEGACY랑 호환 
void Material::Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM)
{
    BindPipeLine();
    BindPerObject(worldTM, worldInverseTransposeTM);
}

// ★ 기존 bind를 분리; Bind 폐기 예정 
// RenderKey에서 Depth를 제외한 Shader Texture의 값이 변동된 순간에만 gpu bind를 명령하도록 설계 
void Material::BindPipeLine()
{
    if (!m_shader) return;

    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();
    if (!context) return;

    DX11Renderer::Instance().BindShader(m_shader);

    uint32_t currentFlags = 0; //Texture bit flag 처리 

    for (size_t i = 0; i < MAX_TEXTURE_SLOTS; ++i) //0 1 2 3 4 5 장의 기본 Texture에 대한 연산 
    {
        ID3D11ShaderResourceView* srv = nullptr;
        ID3D11SamplerState* sampler = nullptr;

        if (m_textures[i])
        {
            currentFlags |= (1 << i);

           srv = m_textures[i]->GetSRV();
           sampler = m_textures[i]->GetSampler(); 
        }

        DX11Renderer::Instance().BindTexture((int)i, srv);


        if (sampler)
            context->PSSetSamplers(static_cast<UINT>(i), 1, &sampler);
    }

    currentFlags |= (uint32_t)MaterialTextureFlag::IBL; //일단 기본으로 넣어둘게 

    DX11Renderer::Instance().UpdateTextureFlag_CBUFFER(currentFlags);

    if (m_renderMode == RenderMode::Transparent)
    {
        DX11Renderer::Instance().SetBlendMode(BlendMode::AlphaBlend);
    }
    else
    {
        DX11Renderer::Instance().SetBlendMode(BlendMode::Opaque);
    }

    DX11Renderer::Instance().SetCullMode(m_cullMode);



   

}

void Material::BindPerObject(const Matrix& worldTM, const Matrix& worldInverseTransposeTM)
{

    DX11Renderer::Instance().UpdateObject_CBUFFER(worldTM, worldInverseTransposeTM);
    DX11Renderer::Instance().UpdateMaterial_CBUFFER(m_data);

    //Material 정보는 오브젝트 단위로 업데이트 되는 게 맞으니깐. ㅇㅇ 
}
