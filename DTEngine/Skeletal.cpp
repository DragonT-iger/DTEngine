#include "pch.h"
#include "Skeletal.h"

#include "ResourceManager.h"
#include "SkeletalRsource.h"
#include "Model.h"


BEGINPROPERTY(Skeletal)
DTPROPERTY_SETTER(Skeletal, m_FbxName , SetSkeletal)
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
	if (!m_BoneResource)
		return;




    const auto& bones = m_BoneResource->m_Bones; //Read Only

    size_t boneCount = bones.size();

    if (m_AnimatedLocalMatrices.size() != boneCount) 
    {
        m_AnimatedLocalMatrices.resize(boneCount);

        for (size_t i = 0; i < boneCount; ++i)
            m_AnimatedLocalMatrices[i] = bones[i].DefaultLocalMatrix;
    
    }

    if (m_globalTransforms.size() != boneCount) m_globalTransforms.resize(boneCount);
    if (m_finalTransforms.size() != boneCount) m_finalTransforms.resize(boneCount);


    //Vector라, Indexing이 계층구조 순서로 되어있음. 

    for (size_t i = 0; i < boneCount; ++i)
    {
        const BoneNode& node = bones[i];

        Matrix localMatrix = m_AnimatedLocalMatrices[i];

        if (node.ParentIndex != -1)
        {
            const Matrix& parentGlobal = m_globalTransforms[node.ParentIndex];

            m_globalTransforms[i] = localMatrix * parentGlobal;
        }
        else
        {
            m_globalTransforms[i] = localMatrix;
        }


        m_finalTransforms[i] = (m_globalTransforms[i] * node.OffsetMatrix).Transpose();


    }


}

void Skeletal::SetSkeletal(std::string filename)
{
    //붙여넣기 시, 공백 들어가서 
    filename.erase(std::remove_if(filename.begin(), filename.end(),
        [](unsigned char x) { return std::isspace(x); }),
        filename.end());

    m_FbxName = filename;

	BoneResource* BoneResource = ResourceManager::Instance().Load<Model>(filename)->GetBone(); //Bone data 생성 주기는 Model 생성과 동일. 

	if (BoneResource)
	{
		m_BoneResource = BoneResource;
	}
	else std::cout << "Bone 생성 오류 FBX 경로: " << filename << std::endl;

}

void Skeletal::SetBonePose(int boneIndex, const Matrix& mat)
{
    if (boneIndex < m_AnimatedLocalMatrices.size())
        m_AnimatedLocalMatrices[boneIndex] = mat;
}
