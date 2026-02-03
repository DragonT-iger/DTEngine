#include "ArrowObjectPool.h"

#include "GameObject.h"
#include "Image.h"
#include "Scene.h"
#include "SceneManager.h"

BEGINPROPERTY(ArrowObjectPool)
ENDPROPERTY()


void ArrowObjectPool::Awake()
{
    Scene* scene = SceneManager::Instance().GetActiveScene();
    if (!scene) return;

    for (int i = 0; i < 64; i++)
    {
        GameObject* arrow = scene->CreateGameObject("Arrow");
        arrow->GetTransform()->SetParent(_GetOwner()->GetTransform());
        arrow->AddComponent<Image>();
        // 필요 시 여기에 addcomponent 하는데 할려면 include 먼저.
        arrow->SetActive(false);
    }
   
}
