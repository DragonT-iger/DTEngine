#include "pch.h"
#include "MonitorController.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

BEGINPROPERTY(MonitorController)
DTPROPERTY_SETTER(MonitorController, m_sourceCamera , SetSourceCamera)
ENDPROPERTY()



MonitorController::MonitorController() = default;
MonitorController::~MonitorController() = default;

void MonitorController::Awake()
{
    m_renderTexture = std::make_unique<RenderTexture>();
    m_renderTexture->Initialize(256, 256);

    MeshRenderer* renderer = GetComponent<MeshRenderer>();
    if (renderer)
    {
        renderer->GetMaterial()->SetTexture(0, m_renderTexture.get());
    }
    if (m_sourceCamera) {
        m_sourceCamera->SetTargetTexture(m_renderTexture.get());
    }
}

void MonitorController::SetSourceCamera(Camera* cam)
{
    if (cam == nullptr) {
        m_sourceCamera = nullptr;
        return;
    }
    m_sourceCamera = cam; cam->SetTargetTexture(m_renderTexture.get()); 
}
