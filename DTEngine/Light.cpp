#include "pch.h"
#include "Light.h"
#include <algorithm> 
#include "DX11Renderer.h"
std::vector<Light*> Light::s_allLights;

BEGINPROPERTY(Light)
DTPROPERTY_ENUM(Light, m_type, "Directional", "Point");
DTPROPERTY(Light, m_color);
DTPROPERTY(Light, m_intensity);
DTPROPERTY(Light, m_range);
ENDPROPERTY()

Light::Light() = default;

Light::~Light()
{
    OnDisable();
}

void Light::OnEnable()
{
    if (std::find(s_allLights.begin(), s_allLights.end(), this) == s_allLights.end())
    {
        s_allLights.push_back(this);
		std::cout << "Light enabled. Total lights: " << s_allLights.size() << std::endl;
    }

    DX11Renderer::Instance().UpdateLights(GetAllLights());
}

void Light::OnDisable()
{
    auto it = std::find(s_allLights.begin(), s_allLights.end(), this);
    if (it != s_allLights.end())
    {
        s_allLights.erase(it);
		std::cout << "Light disabled. Total lights: " << s_allLights.size() << std::endl;
    }

    DX11Renderer::Instance().UpdateLights(GetAllLights());
}
