#pragma once

#include "MonoBehaviour.h"

class SkyBoxComponent : public MonoBehaviour
{
	DTGENERATED_BODY(SkyBoxComponent);


public:

	SkyBoxComponent() = default;
	~SkyBoxComponent() = default;

public:
	void Awake() override;
	void Start() override;
	void Update(float deltaTime) override;

public:
	void SkyBoxColorBinding();
	void SetSkyBoxColor(Vector4 Color);
	const Vector4& GetSkyBoxColor() { return m_SkyBoxColor; }

	void SetSkyBox(uint64_t id); 

private:
	bool m_Dirty = false;
	Vector4 m_SkyBoxColor = Vector4(1, 1, 1, 1);

	uint64_t m_SkyBoxID = 0;

};