#pragma once

#include <memory>
#include <queue>

#include "ICommand.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h" 

class DestroyGameObjectCommand : public ICommand
{
public:
    DestroyGameObjectCommand(Scene* scene, GameObject* target);

    void Execute() override;
    void Undo() override;

private:
    void CollectHierarchy(GameObject* target, std::vector<GameObject*>& outList);

private:
    Scene* m_scene;

    std::vector<std::unique_ptr<GameObject>> m_storedObjects;

    std::vector<GameObject*> m_targetPtrs;

    GameObject* m_rootTarget = nullptr;    
    Transform* m_rootTargetTf = nullptr;
    Transform* m_oldParent = nullptr;      
    int         m_siblingIndex = 0;        
};