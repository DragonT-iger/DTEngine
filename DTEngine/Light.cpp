#include "pch.h"
#include "Light.h"
#include <algorithm> 
#include "DX11Renderer.h"
std::vector<Light*> Light::s_allLights;

BEGINPROPERTY(Light)
DTPROPERTY_ACCESSOR(Light, m_type, GetTypeInt, SetTypeInt)
DTPROPERTY_ACCESSOR(Light, m_intensity, GetIntensity, SetIntensity)
DTPROPERTY_ACCESSOR(Light, m_range, GetRange, SetRange)
DTPROPERTY_ACCESSOR(Light, m_color, GetColor, SetColor)
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

void Light::SetIntensity(float val)
{
    if (m_intensity == val) return;

    m_intensity = val;
    DX11Renderer::Instance().UpdateLights(Light::GetAllLights());
}

void Light::SetRange(float val)
{
    if (m_range == val) return;

    m_range = val;
    DX11Renderer::Instance().UpdateLights(Light::GetAllLights());
}

void Light::SetColor(Vector3 val)
{
    if (m_color == val) return;

    m_color = val;
    DX11Renderer::Instance().UpdateLights(Light::GetAllLights());
}

void Light::SetTypeInt(int val)
{
    if ((int)m_type == val) return;

    m_type = (LightType)val;
    DX11Renderer::Instance().UpdateLights(Light::GetAllLights());
}