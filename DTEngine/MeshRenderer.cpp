#include "pch.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Model.h"
#include "Material.h"

BEGINPROPERTY(MeshRenderer)
DTPROPERTY_SETTER(MeshRenderer, m_modelPath, SetModelPath)
DTPROPERTY_SETTER(MeshRenderer, m_meshIndex, SetMeshIndex)
DTPROPERTY_SETTER(MeshRenderer, m_materialPath, SetMaterialPath)
ENDPROPERTY()

void MeshRenderer::Awake()
{
    if (m_mesh == nullptr && !m_modelPath.empty())
    {
        SetModelPath(m_modelPath);
    }
}

void MeshRenderer::SetModelPath(const std::string& path)
{
    m_modelPath = path;

    if (!m_modelPath.empty())
    {
        Model* model = ResourceManager::Instance().Load<Model>(m_modelPath);
        if (model)
        {
            m_mesh = model->GetMesh(m_meshIndex);
        }
    }
}

void MeshRenderer::SetMeshIndex(int index)
{
    m_meshIndex = index;

    if (!m_modelPath.empty())
    {
        SetModelPath(m_modelPath);
    }
}

void MeshRenderer::SetMaterialPath(const std::string& path)
{
    m_materialPath = path;

    if (!m_materialPath.empty())
    {
        Material* mat = ResourceManager::Instance().Load<Material>(m_materialPath);
        if (mat)
        {
            SetMaterial(mat);
        }
    }
    else
    {
        SetMaterial(nullptr);
    }
}
