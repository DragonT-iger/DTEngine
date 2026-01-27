#include "pch.h"
#include "TutorialTrigger.h"
#include "UIButton.h"
#include "TutorialManager.h" 
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Camera.h"

BEGINPROPERTY(TutorialTrigger)
DTPROPERTY(TutorialTrigger, m_TutorialManager);
DTPROPERTY(TutorialTrigger, m_Button);
ENDPROPERTY()

void TutorialTrigger::Start()
{
    if (m_Button && m_TutorialManager)
    {
        auto onClickFunc = [this]()
            {
                if (this->m_TutorialManager)
                {
                    this->m_TutorialManager->NextStep();
                }
            };

        m_Button->SetOnClick(onClickFunc);
    }
}

void TutorialTrigger::Update(float deltaTime)
{
    //if (InputManager::Instance().GetKeyDown(KeyCode::L))
    //{
    //    Scene* activeScene = SceneManager::Instance().GetActiveScene();
    //    Camera* mainCamera = activeScene ? activeScene->GetMainCamera() : nullptr;

    //    if (mainCamera)
    //    {
    //        MousePos mPos = InputManager::Instance().GetGameMousePosition();
    //        Vector2 screenPos = Vector2(static_cast<float>(mPos.x), static_cast<float>(mPos.y));

    //        Ray ray = mainCamera->ScreenPointToRay(screenPos.x , screenPos.y);

    //        GameObject* hitObject = nullptr;
    //        float hitDistance = 0.0f;

    //        if (activeScene->Raycast2(ray, hitObject, hitDistance))
    //        {
    //            std::cout << "[Raycast Hit] Object: " << hitObject->GetName()
    //                << " | Dist: " << hitDistance << std::endl;
    //        }
    //        else
    //        {
    //            std::cout << "[Raycast Miss]" << std::endl;
    //        }
    //    }
    //}//raycast 예제
}
