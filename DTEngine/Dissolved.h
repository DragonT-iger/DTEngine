#pragma once
#include "MonoBehaviour.h"

class Texture;
class Material;


class Dissolved : public MonoBehaviour
{
	DTGENERATED_BODY(Dissolved);


public:
	Dissolved() = default;
	virtual ~Dissolved() = default;

public:


public:

	//Editor에서 albedo만 넣어주면 됨. 
	void SetAlbedoTexture(uint64_t TextureID);

	//죽었을 때, 해당 material 넣어주면 path 통해서 넣어주면됨. 
	void InjectDissolveMaterila(uint64_t material);
private:
	Texture* m_DiffuseTexture = nullptr;
	uint64_t m_Textureid =0;
	uint64_t m_materialid = 0;
	bool m_Dissolve_Start = false;
	Material* m_material = nullptr;
};