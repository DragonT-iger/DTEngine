#include "pch.h"
#include <cassert>
#include <iostream>
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"

Scene* SceneManager::GetActiveScene() const
{
    return m_active;
}

void SceneManager::RegisterScene(const std::string& name, Scene* scene)
{ 
    m_scenes.emplace(name, scene);
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
    m_active = it->second;

    if (m_active)
    {
        m_active->Awake();
        m_active->Start();
    }

    m_nextName.clear();
}

bool SceneManager::BackupActiveScene()
{
    if (!m_active) return false;

    m_originalSceneName = m_active->GetName();

    if (m_active->SaveFile(m_backupPath))
    {
        std::cout << "[SceneManager] Scene Backup Saved." << std::endl;
        return true;
    }
    return false;
}

bool SceneManager::RestoreActiveScene()
{
    if (!m_active) return false;

    m_active->Clear();

    std::string fullPath = ResourceManager::Instance().ResolveFullPath(m_backupPath);

    if (m_active->LoadFile(fullPath))
    {
        m_active->SetName(m_originalSceneName);
        std::cout << "[SceneManager] Scene Restored from Backup." << std::endl;
        return true;
    }

    std::cerr << "[SceneManager] Failed to restore scene! Path was: " << fullPath << std::endl;
    return false;
}
