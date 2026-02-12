#include "pch.h"
#include "EffectManager.h"
#include "ResourceManager.h"
#include "Prefab.h"
#include "GameObject.h"
#include "Transform.h"
#include "MultiSpriteController .h"

EffectManager::~EffectManager()
{
    m_EffectMap.clear();
}

bool EffectManager::Initialize()
{
    std::vector<std::string> effectKeys = {
        "Smoke",
        "Poison",
        "Attack01",
       "2Attack02",
       "2Attack03",
       "5Magic_Circle",
       "6Magic_Circle",
       "Magic_Circle_2",
        "Heal"
    };

    std::string basePath = "Assets/Prefab/Effect/v4/Effect_";
    std::string extension = ".prefab";

    for (const auto& key : effectKeys)
    {
        std::string fullPath = basePath + key + extension;

        Prefab* prefab = ResourceManager::Instance().Load<Prefab>(fullPath);

        if (prefab)
        {
            m_EffectMap[key] = prefab;
        }
        else
        {
            //std::cout << "Effect InitalizeFailed" << "Path: " << fullPath << std::endl;
            return false;
        }
    }

    return true;

}

void EffectManager::PlayEffect(const std::string& key, GameObject* target)
{
    if (target == nullptr) return;
 
   // PlayEffect(key, target->GetTransform()->GetPosition() );
    const Vector3& position = target->GetTransform()->GetPosition();

    auto iter = m_EffectMap.find(key);
    if (iter == m_EffectMap.end())
    {
        return;
    }

    Prefab* prefab = iter->second;
    if (!prefab) return;

    GameObject* effectObj = prefab->Instantiate();
    if (!effectObj) return;

    Vector3 pos = effectObj->GetTransform()->GetPosition();

    effectObj->GetTransform()->SetPosition(position + pos);
    //effectObj->GetTransform()->SetPosition(position);
    auto ctrl = effectObj->GetComponent<MultiSpriteController>();
    if (ctrl)
    {
        ctrl->Initalize();
        ctrl->SetAutoDestroy(true);
        ctrl->SetFollowTarget(nullptr);
        ctrl->StartChain();
    }


}

void EffectManager::PlayEffect_Targeting(const std::string& key, GameObject* from, GameObject* target)
{
    if (from == nullptr || target == nullptr) return;

    const Vector3& startPos = from->GetTransform()->GetPosition();
    const Vector3& targetPos = target->GetTransform()->GetPosition();

    auto iter = m_EffectMap.find(key);
    if (iter == m_EffectMap.end()) return;

    Prefab* prefab = iter->second;
    if (!prefab) return;

    GameObject* effectObj = prefab->Instantiate();
    if (!effectObj) return;

    Vector3 offset = effectObj->GetTransform()->GetPosition();

    Vector3 forward = targetPos - startPos;
    forward.Normalize();

   
    Vector3 worldUp = Vector3(0, 1, 0);
    Vector3 right;
    worldUp.Cross(forward, right); 

    if (right.LengthSquared() < 0.0001f)
    {
        right = Vector3(1, 0, 0);
    }
    right.Normalize();

    
    Vector3 up;
    forward.Cross(right, up); 
    up.Normalize();

    offset.z += 2.3f; 

    Vector3 worldOffset = (right * offset.x) + (up * offset.y) + (forward * offset.z);
    Vector3 finalPos = startPos + worldOffset;

    effectObj->GetTransform()->SetPosition(finalPos);

    auto ctrl = effectObj->GetComponent<MultiSpriteController>();
    if (ctrl)
    {
        ctrl->Initalize();
        ctrl->SetAutoDestroy(true);
        ctrl->SetFollowTarget(from);
        ctrl->StartChain();
    }
}
