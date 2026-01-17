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

    uint64_t GetModelID() const { return m_modelID; }

    const std::vector<Matrix>& GetFinalBoneTransforms() const { return m_finalTransforms; }

	void SetSkeletal(std::string filename);
	std::string GetSkeletal() { return m_FbxName; }

	std::vector<Matrix>& GetFinalMatrix() { return m_finalTransforms; }
	BoneResource* GetBoneResource() {  if(m_BoneResource) return m_BoneResource; }

	void SetBonePose(int boneIndex, const Matrix& mat); //Animation에 주입. 

private:

    uint64_t m_modelID = 0; // Awkae 순회하면서 중복 업데이트 확인; 중복 체크는 SubMesh들이 동일 Skeletal을 참조하기 때문임. 

    std::vector<Matrix> m_finalTransforms; //CB에 Binding할 Vector [ ] 

	std::vector<Matrix> m_globalTransforms;
	BoneResource* m_BoneResource = nullptr; //Model한테 받는 Raw Resource Pointer 



	std::vector<Matrix> m_AnimatedLocalMatrices;
	std::string m_FbxName = " ";

};

//Model 
//Object의 이름으로 fbx를 가져와야 하는건가
