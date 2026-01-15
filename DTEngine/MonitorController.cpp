#include "pch.h"
#include "MonitorController.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

BEGINPROPERTY(MonitorController)
DTPROPERTY_SETTER(MonitorController, m_sourceCamera, SetSourceCamera)
DTPROPERTY(MonitorController, materialSlot)
ENDPROPERTY()



MonitorController::MonitorController() = default;
MonitorController::~MonitorController() {
    if (m_sourceCamera && m_renderTexture)
    {
        if (m_sourceCamera->GetTargetTexture() == m_renderTexture.get())
        {
            m_sourceCamera->SetTargetTexture(nullptr);
        }
    }
}

void MonitorController::Awake()
{
    if (m_sourceCamera)
    {
        SetSourceCamera(m_sourceCamera);
    }
}


void MonitorController::SetSourceCamera(Camera* cam)
{
    m_sourceCamera = cam;

    if (cam == nullptr) {
        return;
    }

    MeshRenderer* renderer = GetComponent<MeshRenderer>();
    RenderTexture* textureToUse = nullptr;

    if (cam->GetTargetTexture() != nullptr)
    {
        textureToUse = cam->GetTargetTexture();

    }
    else
    {
        if (m_renderTexture == nullptr)
        {
            m_renderTexture = std::make_unique<RenderTexture>();
            m_renderTexture->Initialize(256, 256);
        }

        textureToUse = m_renderTexture.get();

        cam->SetTargetTexture(textureToUse);
    }

    if (renderer && textureToUse)
    {
        renderer->GetSharedMaterial()->SetTexture(materialSlot, textureToUse);
    }
}
