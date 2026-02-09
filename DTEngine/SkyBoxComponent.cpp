#include "pch.h"
#include "SkyBoxComponent.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "AssetDatabase.h"
#include "ResourceManager.h"
#include "DX11Renderer.h"


BEGINPROPERTY(SkyBoxComponent)
DTPROPERTY_SETTER(SkyBoxComponent, m_SkyBoxColor,SetSkyBoxColor);
DTPROPERTY_SETTER(SkyBoxComponent, m_SkyBoxID, SetSkyBox);



ENDPROPERTY()


void SkyBoxComponent::Awake()
{
	std::string path = AssetDatabase::Instance().GetPathFromID(m_SkyBoxID);
	DX11Renderer::Instance().SetSkyBox(path);

	
	SkyBoxColorBinding();

}

void SkyBoxComponent::Start()
{
}

void SkyBoxComponent::Update(float deltaTime)
{


	//if(this->GetTransform()->IsDirty())

	//if (m_Dirty)
		SkyBoxColorBinding();
}


//Material에서 색상 변화할 때, Dirty가 켜지는 식으로 처리하려고 하는데, 
void SkyBoxComponent::SkyBoxColorBinding()
{
	IBL data; data.IBL_Value = m_SkyBoxColor;

	Quaternion q = this->GetTransform()->GetRotationQuat();

	Matrix rot = Matrix::CreateFromQuaternion(q);

	data.Rotation_Matrix = rot.Invert(); //transpose


	DX11Renderer::Instance().UpdateSkyBox_CBUFFER(data);


	m_Dirty = false;

}

void SkyBoxComponent::SetSkyBoxColor(Vector4 Color)
{

	if (Color == m_SkyBoxColor) return; // 안 막으면 Binding이 매 프레임마다. 

	m_SkyBoxColor = Color;
	m_Dirty = true;
}

void SkyBoxComponent::SetSkyBox(uint64_t id)
{
	m_SkyBoxID = id;
	std::string path = AssetDatabase::Instance().GetPathFromID(m_SkyBoxID);
    DX11Renderer::Instance().SetSkyBox(path);

}
