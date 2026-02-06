#include "pch.h"
#include "Skeletal.h"

#include "ResourceManager.h"
#include "SkeletalRsource.h"
#include "Model.h"

#include "AnimationClip.h"


BEGINPROPERTY(Skeletal)
DTPROPERTY_SETTER(Skeletal, m_skellID, SetSkeletal)


ENDPROPERTY()

//넣을 거는 FBX, 즉 Model의 경로  SetSkeletal(std::string filename)

Skeletal::Skeletal()
{
}

Skeletal::~Skeletal()
{
	m_BoneResource = nullptr;
}

void Skeletal::Awake()
{

}

void Skeletal::Update(float deltaTime)
{
   
}





void Skeletal::LateUpdate(float dTime)
{
    if (!m_BoneResource) return;
    size_t nodeCount = m_BoneResource->m_Bones.size();
    for (size_t i = 0; i < nodeCount; ++i)
    {
        if (!m_BoneResource) return;

        size_t nodeCount = m_BoneResource->m_Bones.size();

        for (size_t i = 0; i < nodeCount; ++i)
        {
            BoneNode& resNode = m_BoneResource->m_Bones[i];

            Matrix localMat = m_AnimatedLocalMatrices[i];

            if (resNode.ParentIndex != -1)
            {
                m_globalTransforms[i] = localMat * m_globalTransforms[resNode.ParentIndex];
            }
            else
            {
                m_globalTransforms[i] = localMat;
            }


            m_finalTransforms[i] = resNode.OffsetMatrix * m_globalTransforms[i];
        }
    }
}

void Skeletal::SetSkeletal(uint64_t id)
{
    m_skellID = id;
   

    if (m_skellID != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(m_skellID);

        Model* pModel = ResourceManager::Instance().Load<Model>(path);

        if (pModel == nullptr)
        {
          
            return;
        }

        BoneResource* BoneResource = pModel->GetBone();

        if (BoneResource)
        {

            m_BoneResource = nullptr;

            m_BoneResource = BoneResource;

            ClearVector();

            size_t nodeCount = m_BoneResource->m_Bones.size();
            m_AnimatedLocalMatrices.assign(nodeCount, SimpleMathHelper::IdentityMatrix());

            for (size_t i = 0; i < nodeCount; ++i)
            {
                m_AnimatedLocalMatrices[i] = m_BoneResource->m_Bones[i].DefaultLocalMatrix;
            }

            m_globalTransforms.assign(nodeCount, SimpleMathHelper::IdentityMatrix());
            m_finalTransforms.assign(nodeCount, SimpleMathHelper::IdentityMatrix());
        }
    }
    else return;
}

void Skeletal::ClearVector()
{
    m_finalTransforms.clear();
    m_globalTransforms.clear();
    m_AnimatedLocalMatrices.clear();

}


int Skeletal::GetBoneIndex(const std::string& Name)
{
    return (m_BoneResource->GetIndex(Name));
}

//주입 받기 
    void Skeletal::SetBonePose(int boneIndex, const Matrix & mat)
    {
        if (boneIndex < 0 || boneIndex >= (int)m_AnimatedLocalMatrices.size())
            return;

        m_AnimatedLocalMatrices[boneIndex] = mat;
    }

   


    
