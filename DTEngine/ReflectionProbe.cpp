#include "pch.h"
#include "ReflectionProbe.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Scene.h"
#include "SceneManager.h"
#include "DX11Renderer.h"

ReflectionProbe::ReflectionProbe() = default;
ReflectionProbe::~ReflectionProbe() = default;

BEGINPROPERTY(ReflectionProbe)

DTPROPERTY(ReflectionProbe , m_targetSlot)

ENDPROPERTY()

void ReflectionProbe::Awake()
{
    m_cubeMap = std::make_unique<RenderTexture>();
    m_cubeMap->Initialize(512, 512, RenderTextureType::CubeMap);

    m_cameraObject = std::make_unique<GameObject>("Probe_Internal_Cam");

    m_captureCamera = m_cameraObject->AddComponent<Camera>();
    m_captureCamera->SetEditorFovY(90.0f);
    m_captureCamera->SetAspectRatio(1.0f);
    m_captureCamera->SetNearZ(0.01f);

    if (auto renderer = _GetOwner()->GetComponent<MeshRenderer>())
    {
        if (auto mat = renderer->GetMaterial())
        {
            mat->SetTexture(m_targetSlot, m_cubeMap.get());
        }
    }
}

void ReflectionProbe::Render()
{
    Scene* currentScene = SceneManager::Instance().GetActiveScene();
    if (!currentScene) return;
    GameObject* owner = _GetOwner();
    bool wasActive = owner->IsActive();
    owner->SetActive(false);

    Transform* myTrans = GetTransform();
    Transform* camTrans = m_cameraObject->GetTransform();

    camTrans->SetPosition(myTrans->GetPosition());

    Vector3 targets[6] = {
        { 1, 0, 0 }, { -1, 0, 0 },
        { 0, 1, 0 }, { 0, -1, 0 },
        { 0, 0, 1 }, { 0, 0, -1 }
    };

    Vector3 ups[6] = {
        { 0, 1, 0 }, { 0, 1, 0 },
        { 0, 0, -1 }, { 0, 0, 1 },
        { 0, 1, 0 }, { 0, 1, 0 }
    };

    for (int i = 0; i < 6; ++i)
    {
        camTrans->LookAt(myTrans->GetPosition() + targets[i], ups[i]);

        m_captureCamera->LateUpdate(0.0f);

        m_cubeMap->Bind(i);
        m_cubeMap->Clear(0.2f, 0.2f, 0.2f, 1.0f, i); 

        currentScene->Render(m_captureCamera, m_cubeMap.get(), false);
    }

    m_cubeMap->Unbind();

    owner->SetActive(wasActive);
}