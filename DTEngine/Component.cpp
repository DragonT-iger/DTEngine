#include "pch.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"

void Component::Destroy(GameObject* gameobject) {
	Scene* curScene = SceneManager::Instance().GetActiveScene();
	curScene->Destroy(gameobject);
}