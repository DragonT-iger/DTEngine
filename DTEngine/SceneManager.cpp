#include "pch.h"
#include <cassert>
#include <iostream>
#include <filesystem>
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "UIManager.h"

Scene* SceneManager::GetActiveScene() const
{
    return m_active;
}

void SceneManager::RegisterScene(const std::string& filePath)
{
    std::string name = std::filesystem::path(filePath).stem().string();

	std::string newFilePath = ResourceManager::Instance().ResolveFullPath(filePath);

    auto newScene = std::make_unique<Scene>();

    newScene->SetName(name);

    if (newScene->LoadFile(newFilePath))
    {
        m_scenes[name] = std::move(newScene);
    }
}

void SceneManager::LoadScene(const std::string& name)
{
    m_nextName = name;
	std::cout << "[SceneManager] Scene change requested: " << name << std::endl;
}

bool SceneManager::ProcessSceneChange()
{
    if (m_nextName.empty()) return false;


    GetActiveScene();

    if (m_active)
    {
        m_active->Exit();
    }
    // 현재 씬 정리 필요.

    auto it = m_scenes.find(m_nextName);
    assert(it != m_scenes.end());
    m_active = it->second.get();

    if (m_active)
    {
        m_active->Awake();
        m_active->Start();

        // uimanager 한테 tables 넘겨주기.
        UIManager::Instance().SetUIInteractables(m_active->CollectUIInteractables());

        //m_active->Enter(); addcomponent 할때 OnEnable 호출 됨

    }

    m_nextName.clear();

    return true;
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

