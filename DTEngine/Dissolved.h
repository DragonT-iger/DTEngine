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
	void Awake() override;

public:

	//Editor에서 albedo만 넣어주면 됨. 
	void SetAlbedoTexture(uint64_t TextureID);
	void SetAlbedoTexture(const std::string& TextureID);

	void SetNoiseTexture();
	//죽었을 때, 해당 material 넣어주면 path 통해서 넣어주면됨. 
	void InjectDissolveMaterila(const std::string& path);
private:
	Texture* m_DiffuseTexture = nullptr;
	Texture* m_NoiseMap = nullptr;

	uint64_t m_Textureid =0;
	std::string m_path="";

	bool m_Dissolve_Start = false;
	Material* m_material = nullptr;
};