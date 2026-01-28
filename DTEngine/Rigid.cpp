#include "pch.h"
#include "Rigid.h"
#include "ResourceManager.h"
#include "SkeletalRsource.h"
#include "Model.h"

#include <iomanip>

BEGINPROPERTY(Rigid)
DTPROPERTY_SETTER(Rigid, m_modelID, SetModel)

ENDPROPERTY()

void Rigid::LateUpdate(float dTime)
{
    if (!m_resource) return;


    size_t nodeCount = m_resource->Nodes.size();

   // std::cout << nodeCount << std::endl;
    //std::cout << this->_GetOwner()->GetName()<< std::endl;

    for (size_t i = 0; i < nodeCount; ++i)
    {
        auto& nodeData = m_resource->Nodes[i];
        Matrix localMat = m_animatedLocalMatrices[i];

        if (nodeData.ParentIndex != -1)
            m_globalTransforms[i] = localMat * m_globalTransforms[nodeData.ParentIndex];
        else
            m_globalTransforms[i] = localMat;

        m_finalTransforms[i] = nodeData.OffsetMatrix* m_globalTransforms[i];
    }
}

void Rigid::SetModel(uint64_t id)
{
    m_modelID = id;
    if (m_modelID == 0) return;

    std::string path = AssetDatabase::Instance().GetPathFromID(m_modelID);

    Model* pModel = ResourceManager::Instance().Load<Model>(path);

    if (pModel == nullptr) return;

    NodeResource* pNodeRes = pModel->GetNodeResource();


    if (pNodeRes != nullptr)
    {
        m_resource = nullptr;

        m_resource = pNodeRes;

        size_t count = m_resource->Nodes.size();

        m_animatedLocalMatrices.assign(count, Matrix());
        m_globalTransforms.resize(count);
        m_finalTransforms.resize(count);

        for (size_t i = 0; i < count; ++i)
            m_animatedLocalMatrices[i] = m_resource->Nodes[i].DefaultLocalMatrix;
    }
}

void Rigid::SetNodePose(int nodeIndex, const Matrix& mat)
{
    if (nodeIndex >= 0 && nodeIndex < (int)m_animatedLocalMatrices.size())
        m_animatedLocalMatrices[nodeIndex] = mat;
}