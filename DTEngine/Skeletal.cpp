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

    // 2. 버퍼 크기가 안 맞으면 재할당 (최초 1회 혹은 변경 시)
    if (m_globalTransforms.size() != boneCount) m_globalTransforms.resize(boneCount);
    if (m_finalTransforms.size() != boneCount) m_finalTransforms.resize(boneCount);

    // 3. 계층 구조 순회 및 행렬 계산
    // 중요: 부모의 행렬이 먼저 계산되어야 자식이 그걸 참조할 수 있습니다.
    // m_Bones 벡터가 "부모가 항상 자식보다 인덱스가 작은 순서"로 정렬되어 있다고 가정합니다.
    // (Assimp나 대부분의 로더는 이 순서를 보장하지만, 보장되지 않는다면 재귀 함수를 써야 합니다.)

    for (size_t i = 0; i < boneCount; ++i)
    {
        const BoneNode& node = bones[i];

        // ---------------------------------------------------
        // Step 1. Local Transform 계산
        // ---------------------------------------------------
        // TODO: 나중에 여기에 애니메이션 보간(Interpolation) 로직이 들어갑니다.
        // 지금은 애니메이션이 없으므로 '기본 포즈(Bind Pose)'를 사용합니다.
        Matrix localMatrix = node.DefaultLocalMatrix;


        // ---------------------------------------------------
        // Step 2. Global Transform 계산 (누적 행렬)
        // ---------------------------------------------------
        // 부모가 있으면: 내 로컬 * 부모의 글로벌
        // 부모가 없으면(루트): 내 로컬이 곧 글로벌
        if (node.ParentIndex != -1)
        {
            // 부모의 계산된 Global 행렬을 가져옴
            const Matrix& parentGlobal = m_globalTransforms[node.ParentIndex];

            // SimpleMath 기준: Local * Parent (행렬 곱 순서 주의)
            m_globalTransforms[i] = localMatrix * parentGlobal;
        }
        else
        {
            m_globalTransforms[i] = localMatrix;
        }

        // ---------------------------------------------------
        // Step 3. Final Transform 계산 (스키닝용)
        // ---------------------------------------------------
        // Vertex(Mesh Space) -> (Offset) -> Bone Space -> (Global) -> World Space
        // 결과적으로: Final = Offset * Global
        m_finalTransforms[i] = (node.OffsetMatrix * m_globalTransforms[i]).Transpose();;

        //std::cout << m_finalTransforms[i]._11 << std::endl;

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
