#pragma once

#include "MonoBehaviour.h"

class Shader;

//  Material에 접근 -> CullMode + NoiseMap을 Binding
// //음... Render한 번 한 다음에 해당 컴포넌트를 갖고 있는 Object들을 모아서 처리. 그냥 Flag 임. 

class Outline : public MonoBehaviour
{
	DTGENERATED_BODY(Outline);

public:
	Outline() = default;
	virtual ~Outline() = default;

	void Start() override;
	void SetShader(Shader* Shader) { m_Shader = Shader; }

	Shader* GetShader() { return m_Shader; }

private:

	Shader* m_Shader = nullptr;

};