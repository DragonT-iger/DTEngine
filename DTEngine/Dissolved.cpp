#include "pch.h"
#include "Dissolved.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Material.h"
#include "AssetDatabase.h"
#include "MeshRenderer.h"
#include "Effect.h"

BEGINPROPERTY(Dissolved)
DTPROPERTY_SETTER(Dissolved, m_Textureid, SetAlbedoTexture)
ENDPROPERTY()

//순회해서 처리하는 장비의 경우 명시적으로 albedo를 넣어줘야 함.

void Dissolved::SetAlbedoTexture(uint64_t TextureID)
{
	if (!m_DiffuseTexture)
	{
		const std::string& Path = AssetDatabase::Instance().GetPathFromID(TextureID);
		if (Path != "")
		{
			Texture* Texture_albedo = ResourceManager::Instance().Load<Texture>(Path);
			if (Texture_albedo)
			{
				m_DiffuseTexture = Texture_albedo;
				m_Textureid = TextureID;
			}


		}
		else return;

	}
}

void Dissolved::Awake()
{
	m_NoiseMap = ResourceManager::Instance().Load<Texture>("Assets/Models/Noise/Noise_Map.png");
}

static float CountDown = 3.0f;

void Dissolved::Update(float dTime)
{
	if (m_CountDown)
	{
		m_Time += dTime;

		if (m_Time >= CountDown)
			this->_GetOwner()->SetActive(false);
	}


}


void Dissolved::SetAlbedoTexture(const std::string& Path)
{
	if (!m_DiffuseTexture)
	{
		if (Path != "")
		{
			Texture* Texture_albedo = ResourceManager::Instance().Load<Texture>(Path);
			if (Texture_albedo)
			{
				m_DiffuseTexture = Texture_albedo;
			}


		}
		else return;

	}
}

void Dissolved::SetNoiseTexture()
{
	if(!m_NoiseMap) m_NoiseMap = ResourceManager::Instance().Load<Texture>("Assets/Models/Noise/Noise_Map.png");

}

void Dissolved::InjectDissolveMaterila(const std::string& path)
{
	auto cmp =  this->_GetOwner()->GetComponent<MeshRenderer>();
	auto effectcmp = this->_GetOwner()->GetComponent<Effect>(); //이게 있어야 dissolve가 됨.
	if (cmp && effectcmp)
	{
		effectcmp->SetTimeMultiplier(0.1f);
		effectcmp->SetEdgeColor(Vector4(0, 0, 0, 1));
		effectcmp->Reset();
			if (!path.empty())
			{
				Material* mat = ResourceManager::Instance().Load<Material>(path);
				if (mat)
				{
					cmp->SetMaterial(mat);
					m_path = path;

					if (!m_NoiseMap) SetNoiseTexture();

						if (m_DiffuseTexture && m_NoiseMap)
					{
						mat->SetTexture(0, m_DiffuseTexture);
						mat->SetTexture(6, m_NoiseMap);

						m_CountDown = true;
					}
			}
		}
	}
	else 
		return;
}
