#include "pch.h"
#include "outline.h"

#include "ResourceManager.h"
#include "Shader.h"

BEGINPROPERTY(Outline)
ENDPROPERTY()


void Outline::Start()
{
	Shader* Extracted_Shaader = ResourceManager::Instance().Load<Shader>("Assets/Shaders/OutlineShader_PS.hlsl");
		if (Extracted_Shaader) m_Shader = Extracted_Shaader;
}
