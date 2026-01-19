#include "pch.h"
#include "Skeletal.h"

#include "ResourceManager.h"
#include "SkeletalRsource.h"
#include "Model.h"

#include "AnimationClip.h"

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
   
}


void Skeletal::LateUpdate(float dTime)
{
    if (!m_BoneResource) return;

    size_t nodeCount = m_BoneResource->m_Bones.size();
    static bool bDebugOnce = true; // 딱 한 번만 출력

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

      
        m_finalTransforms[i] = (resNode.OffsetMatrix * m_globalTransforms[i]);

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

        size_t nodeCount = m_BoneResource->m_Bones.size();
        m_AnimatedLocalMatrices.assign(nodeCount, SimpleMathHelper::IdentityMatrix());


        //aniamtino 정보가 없는 bone은 defaultlocalmatrix로 초기화 하고, 후에 Animator update를 통해서 값이 업데이트 됨. 
        for (size_t i = 0; i < nodeCount; ++i)
        {
            m_AnimatedLocalMatrices[i] = m_BoneResource->m_Bones[i].DefaultLocalMatrix;
            Matrix defaultMat = m_BoneResource->m_Bones[i].DefaultLocalMatrix;
            m_AnimatedLocalMatrices[i] = defaultMat;
        }

        m_globalTransforms.resize(nodeCount);
        m_finalTransforms.resize(nodeCount);


	}
	else std::cout << "Bone 생성 오류 FBX 경로: " << filename << std::endl;

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

   


    
