#include "pch.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Material.h"
#include "AssetDatabase.h" 

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