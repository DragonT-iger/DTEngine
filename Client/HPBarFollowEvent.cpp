//#include "pch.h"
//#include "HPBarFollowEvent.h"
//#include "GameObject.h"
//#include "Unit.h"
//#include "Camera.h"
//#include "Image.h"
//#include "Scene.h"
//#include "SceneManager.h"
//#include "DX11Renderer.h"
//
//BEGINPROPERTY(HPBarFollowEvent)
//DTPROPERTY(HPBarFollowEvent, m_targetObject)
//DTPROPERTY(HPBarFollowEvent, m_trackImage)
//DTPROPERTY(HPBarFollowEvent, m_fillImage)
//DTPROPERTY(HPBarFollowEvent, m_camera)
//DTPROPERTY(HPBarFollowEvent, m_lerpSpeed)
//DTPROPERTY(HPBarFollowEvent, m_worldOffset)
//DTPROPERTY(HPBarFollowEvent, m_screenOffset)
//ENDPROPERTY()
//
//void HPBarFollowEvent::Start()
//{
//    if (!m_fillImage || !m_targetObject)
//        return;
//
//    // 원래 스케일 저장.
//    m_fillBaseScale = m_fillImage->GetTransform()->GetScale();
//
//    // unit component 캐싱.
//    m_targetUnit = m_targetObject->GetComponent<Unit>();
//}
//
//void HPBarFollowEvent::Update(float deltaTime)
//{
//    if (!m_camera)
//        return;
//
//    UpdateFollowPosition(deltaTime);
//    UpdateFillScale(deltaTime);
//}
//
//void HPBarFollowEvent::UpdateFollowPosition(float deltaTime)
//{
//    const Vector3 worldPos = m_targetObject->GetTransform()->GetWorldPosition() + m_worldOffset;
//    const Vector3 screenPos = m_camera->WorldToScreenPoint(worldPos);
//
//#ifdef _DEBUG
//    const auto& gameRes = InputManager::Instance().GetGameResolution();
//    const float renderWidth = static_cast<float>(gameRes.x);
//    const float renderHeight = static_cast<float>(gameRes.y);
//#else
//    const float renderWidth = static_cast<float>(DX11Renderer::Instance().GetWidth());
//    const float renderHeight = static_cast<float>(DX11Renderer::Instance().GetHeight());
//#endif
//
//    const float refWidth = static_cast<float>(DX11Renderer::Instance().GetRefWidth());
//    const float refHeight = static_cast<float>(DX11Renderer::Instance().GetRefHeight());
//    const float scaleX = renderWidth / refWidth;
//    const float scaleY = renderHeight / refHeight;
//
//    Vector2 uiPos(screenPos.x / scaleX, screenPos.y / scaleY);
//    uiPos += m_screenOffset;
//
//    Transform* uiTransform = GetTransform();
//    const Vector3 uiSize = uiTransform->GetWorldScale();
//    uiPos.x -= uiSize.x * 0.5f;
//    uiPos.y -= uiSize.y * 0.5f;
//
//    const Vector3 currentPos = uiTransform->GetPosition();
//    uiTransform->SetPosition(Vector3(uiPos.x, uiPos.y, currentPos.z));
//}
//
//void HPBarFollowEvent::UpdateFillScale(float deltaTime)
//{
//    if (!m_fillImage)
//        return;
//
//    if (!m_targetUnit->IsAlive())
//    {
//        if (auto* owner = _GetOwner())
//        {
//            owner->SetActive(false);
//        }
//        return;
//    }
//
//    const float maxHp = m_targetUnit->GetStats().maxHp;
//
//    if (maxHp <= 0.0f)
//    {
//        _GetOwner()->SetActive(false);
//        return;
//    }
//
//    const float targetRatio = std::clamp(m_targetUnit->GetHp() / maxHp, 0.0f, 1.0f);
//
//    // 지수형 보간으로 부드럽게
//    const float t = 1.0f - std::exp(-m_lerpSpeed * deltaTime);
//    m_currentRatio = m_currentRatio + (targetRatio - m_currentRatio) * t;
//
//    // x 스케일만 줄이면 우측부터 줄어드는거. 
//    Vector3 scale = m_fillBaseScale;
//    scale.x *= m_currentRatio;
//
//    m_fillImage->GetTransform()->SetScale(scale);
//}
