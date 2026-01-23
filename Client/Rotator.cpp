#include "Rotator.h"
#include "GameObject.h" 
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "InputManager.h"


BEGINPROPERTY(Rotator)
DTPROPERTY(Rotator, m_rotationSpeed)
ENDPROPERTY()

void Rotator::Update(float deltaTime)
{
    Transform* tr = GetTransform();
    if (!tr) return;;

    Vector3 currentEuler = tr->GetEditorEuler();

    currentEuler += m_rotationSpeed * deltaTime;

    tr->SetRotationEuler(currentEuler);

    auto scene = SceneManager::Instance().GetActiveScene();
    Camera* mainCamera = scene->GetMainCamera();

    if (mainCamera)
    {
        if (InputManager::Instance().GetKeyDown(KeyCode::G))
        {
            mainCamera->SetPostProcessEffect(PostProcessType::GrayScale, true);
			std::cout << "GrayScale effect enabled." << std::endl;
        }

        if (InputManager::Instance().GetKeyDown(KeyCode::H))
        {
            mainCamera->SetPostProcessEffect(PostProcessType::GrayScale, false);
        }

        if (InputManager::Instance().GetKeyDown(KeyCode::T))
        {
            bool isEnabled = mainCamera->IsEffectEnabled(PostProcessType::GrayScale);
            mainCamera->SetPostProcessEffect(PostProcessType::GrayScale, !isEnabled);
        }
    }
}



//void Rotator::Start()
//{
//	std::cout << "Started Rotator on " << GetName() << std::endl;
//
//    GameObject* go = Instantiate("dfsasdf");
//
//    go->AddComponent<UIButton>();
//}
