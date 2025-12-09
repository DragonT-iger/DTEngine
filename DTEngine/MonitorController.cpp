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
    m_renderTexture->Initialize(512, 512);

    MeshRenderer* renderer = GetComponent<MeshRenderer>();
    if (renderer)
    {
        renderer->GetMaterial()->SetTexture(0, m_renderTexture.get());
    }
}

void MonitorController::Update(float deltaTime)
{
    if (m_sourceCamera && m_renderTexture)
    {
        m_renderTexture->Bind();
        m_renderTexture->Clear(0, 0, 0, 1); 

        // (2) 카메라 위치 기준으로 렌더링
        // 주의: 현재 엔진 구조상 Scene::Render()가 전체를 그리므로,
        // 특정 카메라 시점으로 그리는 별도 함수가 필요할 수 있습니다.
        // 여기서는 예시로 RenderScene(camera) 같은 함수가 있다고 가정합니다.

        // 간단하게 구현하려면:
        // DX11Renderer::UpdateFrameCBuffer(cam->View, cam->Proj)
        // SceneManager::GetActiveScene()->RenderGameObjects(); 

        //SceneManager::Instance().GetActiveScene()->RenderFromCamera(m_sourceCamera);

        m_renderTexture->Unbind();
    }
}