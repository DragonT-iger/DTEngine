#pragma once

#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "ReflectionMacros.h"


#include <vector>
#include <string>
#include <map>


class Model;
class BoneResource;



class Skeletal : public MonoBehaviour
{

	DTGENERATED_BODY(Skeletal);


public:
	Skeletal();
	virtual ~Skeletal();

	void Awake() override;
	void Update(float dTime) override;
	void LateUpdate(float dTime) override;
    uint64_t GetModelID() const { return m_skellID; }

    const std::vector<Matrix>& GetFinalBoneTransforms() const { return m_finalTransforms; }

	void SetSkeletal(std::string filename);
	void SetSkeletal(uint64_t id );
	void ClearVector();
	std::string GetSkeletal() { return m_FbxName; }

	std::vector<Matrix>& GetFinalMatrix() { return m_finalTransforms; }
	BoneResource* GetBoneResource() { return m_BoneResource; }
	int GetBoneIndex(const std::string& Name);
	void SetBonePose(int boneIndex, const Matrix& mat); 

private:


    std::vector<Matrix> m_finalTransforms; //CB에 Binding할 Vector [ ] 
	std::vector<Matrix> m_globalTransforms; // 중첩되는 Model Matrix 
	std::vector<Matrix> m_AnimatedLocalMatrices; //animator로 주입되는 matrix 



	BoneResource* m_BoneResource = nullptr; //Model한테 받는 Raw Resource Pointer 
	uint64_t m_skellID = 0; // Awkae 순회하면서 중복 업데이트 확인; 중복 체크는 SubMesh들이 동일 Skeletal을 참조하기 때문임. ; 일단 대기. 
	std::string m_FbxName = " ";

};

