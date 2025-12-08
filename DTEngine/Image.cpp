#include "pch.h" 
#include "Image.h"
#include "DX11Renderer.h"
#include "ReflectionMacros.h" 
#include "Transform.h"

BEGINPROPERTY(Image)
DTPROPERTY(Image, m_texture)
DTPROPERTY(Image, m_color)
ENDPROPERTY()



Image::Image() = default;


void Image::Render()
{
    if (m_texture != nullptr && this->IsActive())
    {
		Transform* tf = GetTransform();

		Vector2 position = Vector2(tf->GetPosition().x, -tf->GetPosition().y);
        //DX11Renderer::Instance().DrawUI(m_texture, position, m_color);
    }
}