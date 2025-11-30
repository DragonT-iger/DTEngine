#include "pch.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Model.h"

BEGINPROPERTY(MeshRenderer)
DTPROPERTY(MeshRenderer, m_modelPath)
DTPROPERTY(MeshRenderer ,m_meshIndex)
ENDPROPERTY()

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

void MeshRenderer::Awake()
{
    SetModelPath(m_modelPath);
}

void MeshRenderer::SetMeshIndex(int index)
{
    m_meshIndex = index;
    if (!m_modelPath.empty())
    {
        SetModelPath(m_modelPath); // 인덱스 변경 시 메쉬 재할당
    }
}