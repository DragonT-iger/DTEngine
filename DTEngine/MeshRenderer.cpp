#include "pch.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Material.h"
#include "AssetDatabase.h" 
#include "Texture.h"

#include "../ThirdParty/nlohmann/json.hpp"


using json = nlohmann::json;


BEGINPROPERTY(MeshRenderer)
DTPROPERTY_SETTER(MeshRenderer, m_modelID, SetModelID)
DTPROPERTY_SETTER(MeshRenderer, m_meshIndex, SetMeshIndex)
DTPROPERTY_SETTER(MeshRenderer, m_materialID, SetMaterialID)
ENDPROPERTY()

void MeshRenderer::Awake()
{
    if (m_mesh == nullptr && m_modelID != 0)
    {
        SetModelID(m_modelID);
    }

    if (m_material == nullptr && m_materialID != 0)
    {
        SetMaterialID(m_materialID);
    }
}

MeshRenderer::MeshRenderer() = default;

MeshRenderer::~MeshRenderer()
{
    if (m_isMaterialInstanced && m_material != nullptr)
    {
        delete m_material;
        m_material = nullptr;
    }
}

void MeshRenderer::SetMaterial(Material* material)
{
    if (m_isMaterialInstanced && m_material != nullptr)
    {
        delete m_material;
    }

    m_material = material;
    m_isMaterialInstanced = false; 
}

void MeshRenderer::SaveInstanceData(JsonWriter& writer)
{
    if (!m_isMaterialInstanced || m_material == nullptr)
        return;

    writer.BeginObject("MaterialInstance");
    {
        Vector4 color = m_material->GetColor();
        writer.Write("Color", color.x, color.y, color.z, color.w);

        Vector2 tiling = m_material->GetTiling();
        writer.Write("Tiling", tiling.x, tiling.y);

        Vector2 offset = m_material->GetOffset();
        writer.Write("Offset", offset.x, offset.y);

        writer.BeginObject("Textures");
        for (int i = 0; i < Material::MAX_TEXTURE_SLOTS; ++i)
        {
            Texture* tex = m_material->GetTexture(i);
            if (tex)
            {
                uint64_t guid = tex->GetMeta().guid;
                writer.Write(std::to_string(i).c_str(), guid);
            }
        }
        writer.EndObject();
    }
    writer.EndObject();
}

void MeshRenderer::LoadInstanceData(JsonReader& reader)
{
    if (reader.BeginObject("MaterialInstance"))
    {
        Material* mat = GetMaterial();
        if (mat)
        {
            std::array<float, 4> defaultColor = { mat->GetColor().x, mat->GetColor().y, mat->GetColor().z, mat->GetColor().w };
            std::array<float, 4> c = reader.ReadVec4("Color", defaultColor);
            mat->SetColor(Vector4(c[0], c[1], c[2], c[3]));

			reader.ReadVec2("Tiling", { mat->GetTiling().x,  mat->GetTiling().y });
			reader.ReadVec2("Offset", { mat->GetOffset().x,  mat->GetOffset().y });

            if (reader.BeginObject("Textures"))
            {
                for (int i = 0; i < Material::MAX_TEXTURE_SLOTS; ++i)
                {
                    std::string key = std::to_string(i);
                    if (reader.Has(key.c_str()))
                    {
                        uint64_t texID = reader.ReadUInt64(key.c_str());
                        std::string path = AssetDatabase::Instance().GetPathFromID(texID);

                        if (!path.empty())
                        {
                            Texture* tex = ResourceManager::Instance().Load<Texture>(path);
                            mat->SetTexture(i, tex);
                        }
                    }
                }
                reader.EndObject();
            }
        }
        reader.EndObject(); 
    }
}

void MeshRenderer::SetModelID(uint64_t id)
{
    m_modelID = id;
    m_mesh = nullptr; 

    if (m_modelID != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(m_modelID);
        if (!path.empty())
        {
            Model* model = ResourceManager::Instance().Load<Model>(path);
            if (model)
            {
                m_mesh = model->GetMesh(m_meshIndex);
            }
        }
    }
}

void MeshRenderer::SetMeshIndex(int index)
{
    m_meshIndex = index;

    if (m_modelID != 0)
    {
        SetModelID(m_modelID);
    }
}

void MeshRenderer::SetMaterialID(uint64_t id)
{
    if (m_isMaterialInstanced && m_material != nullptr)
    {
        delete m_material;
        m_material = nullptr;
        m_isMaterialInstanced = false;
    }

    m_materialID = id;
    m_material = nullptr;

    if (m_materialID != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(m_materialID);
        if (!path.empty())
        {
            Material* mat = ResourceManager::Instance().Load<Material>(path);
            if (mat)
            {
                SetMaterial(mat);
            }
        }
    }
}

Material* MeshRenderer::GetMaterial()
{
    if (m_material == nullptr) return nullptr;

    if (m_isMaterialInstanced) return m_material;

    Material* instanceMat = m_material->Clone();

    m_material = instanceMat;
    m_isMaterialInstanced = true;

    return m_material;
}
