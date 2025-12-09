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
MonitorController::~MonitorController() = default;

void MonitorController::Awake()
{
    m_renderTexture = std::make_unique<RenderTexture>(); // 귀찮아서 awake만 초기화해줬음 에디터에선 그니까 play stop 한번 해야 초기화 됨 
    m_renderTexture->Initialize(256, 256);

    MeshRenderer* renderer = GetComponent<MeshRenderer>();
    if (renderer)
    {
        renderer->GetMaterial()->SetTexture(materialSlot, m_renderTexture.get());
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
