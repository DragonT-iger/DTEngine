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
    // 인스턴싱된 상태가 아니면 저장 안 함
    if (!m_isMaterialInstanced || m_material == nullptr)
        return;

    // "MaterialInstance": { ... } 시작
    writer.BeginObject("MaterialInstance");
    {
        // 1. 색상 저장 (Write 함수 오버로딩 활용)
        Vector4 color = m_material->GetColor();
        writer.Write("Color", color.x, color.y, color.z, color.w);

        // 2. 텍스처 저장
        // "Textures": { "0": guid, "1": guid ... }
        writer.BeginObject("Textures");
        for (int i = 0; i < 5; ++i)
        {
            Texture* tex = m_material->GetTexture(i);
            if (tex)
            {
                uint64_t guid = tex->GetMeta().guid;
                // 슬롯 인덱스를 키(key)로 변환하여 저장
                writer.Write(std::to_string(i).c_str(), guid);
            }
        }
        writer.EndObject(); // End Textures
    }
    writer.EndObject(); // End MaterialInstance
}

void MeshRenderer::LoadInstanceData(JsonReader& reader)
{
    // "MaterialInstance" 객체가 있는지 확인하고 진입
    if (reader.BeginObject("MaterialInstance"))
    {
        // 강제 인스턴싱 수행
        Material* mat = GetMaterial();
        if (mat)
        {
            // 1. 색상 로드 (ReadVec4 활용)
            // 기본값은 현재 색상으로 설정
            std::array<float, 4> defaultColor = { mat->GetColor().x, mat->GetColor().y, mat->GetColor().z, mat->GetColor().w };
            std::array<float, 4> c = reader.ReadVec4("Color", defaultColor);
            mat->SetColor(Vector4(c[0], c[1], c[2], c[3]));

            // 2. 텍스처 로드
            if (reader.BeginObject("Textures"))
            {
                for (int i = 0; i < 5; ++i)
                {
                    std::string key = std::to_string(i);
                    // 해당 슬롯(키)에 데이터가 있는지 확인
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
                reader.EndObject(); // End Textures
            }
        }
        reader.EndObject(); // End MaterialInstance
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
