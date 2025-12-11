#include "ColorToggler.h"
#include "InputManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "KeyCode.h"

BEGINPROPERTY(ColorToggler)
ENDPROPERTY()

void ColorToggler::Update(float deltaTime)
{
    if (InputManager::Instance().GetKeyDown(KeyCode::N))
    {
        MeshRenderer* meshRenderer = GetComponent<MeshRenderer>();
        if (meshRenderer)
        {
            Material* material = meshRenderer->GetSharedMaterial();
            if (material)
            {
                Vector4 targetColor = m_isFirstColor ? m_firstColor : m_secondColor;

                material->SetColor(targetColor / 255.0f);

                // 플래그 반전
                m_isFirstColor = !m_isFirstColor;
            }
        }
    }
}