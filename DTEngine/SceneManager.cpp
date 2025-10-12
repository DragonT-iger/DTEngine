#include "pch.h"
#include <cassert>
#include "SceneManager.h"
#include "Scene.h"

Scene* SceneManager::GetActiveScene() const
{
    return m_active;
}

void SceneManager::RegisterScene(const std::string& name, std::unique_ptr<Scene> scene)
{ 
    m_scenes.emplace(name, std::move(scene));
}

void SceneManager::LoadScene(const std::string& name)
{
    m_nextName = name;
}

void SceneManager::ProcessSceneChange()
{
    if (m_nextName.empty()) return;

    // 현재 씬 정리 필요.

    auto it = m_scenes.find(m_nextName);
    assert(it != m_scenes.end());
    m_active = it->second.get();

    if (m_active)
    {
        m_active->Awake();
        m_active->Start();
    }

    m_nextName.clear();
}
