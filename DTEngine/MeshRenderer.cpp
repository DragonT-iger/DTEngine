#include "pch.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Model.h"

BEGINPROPERTY(MeshRenderer)
DTPROPERTY_SETTER(MeshRenderer, m_modelPath, SetModelPath)
DTPROPERTY_SETTER(MeshRenderer, m_meshIndex, SetMeshIndex)
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