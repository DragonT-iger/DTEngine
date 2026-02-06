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
DTPROPERTY_SETTER(Dissolved, m_materialid, InjectDissolveMaterila)

ENDPROPERTY()


void Dissolved::SetAlbedoTexture(uint64_t TextureID)
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

void Dissolved::InjectDissolveMaterila(uint64_t material_id )
{


	auto cmp =  this->_GetOwner()->GetComponent<MeshRenderer>();
	auto effectcmp = this->_GetOwner()->GetComponent<Effect>(); //이게 있어야 dissolve가 됨.
	if (cmp && effectcmp)
	{
		if (material_id != 0)
		{
			std::string path = AssetDatabase::Instance().GetPathFromID(material_id);
			if (!path.empty())
			{
				Material* mat = ResourceManager::Instance().Load<Material>(path);
				if (mat)
				{
					cmp->SetMaterial(mat);
				}
			}
		}
	}
	else 
		return;

	

}
