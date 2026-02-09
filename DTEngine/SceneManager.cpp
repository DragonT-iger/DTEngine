#include "pch.h"
#include <cassert>
#include <iostream>
#include <filesystem>
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"

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

        m_active->Clear();
        m_active->SetBGMPlayed(false);
    }

    auto it = m_scenes.find(m_nextName);
    if (it == m_scenes.end())
    {
        std::cerr << "[SceneManager] Error: Scene not found: " << m_nextName << std::endl;
        m_nextName.clear();
        return false;
    }
    Scene* nextScene = it->second.get();
    m_active = it->second.get(); 
    nextScene->Clear();

    std::string scenePath = "Scenes/" + m_nextName + ".scene";
    std::string fullPath = ResourceManager::Instance().ResolveFullPath(scenePath);

    if (!m_active->LoadFile(fullPath))
    {
        std::cerr << "[SceneManager] Failed to reload scene file: " << fullPath << std::endl;
    }

    std::cout << "[SceneManager] Scene Loaded: " << m_nextName << std::endl;

    if (m_active)
    {
        m_active->Awake();
        m_active->Start();
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


//MAP 순회하면서 다 RELOAD
bool SceneManager::RestoreActiveScene()
{
    std::string originalPath = "Assets/Scenes/";

    for (auto& pair : m_scenes)
    {
        const std::string& sceneName = pair.first;
        Scene* scene = pair.second.get();

        std::string Path = originalPath + sceneName+ ".scene";
        scene->Clear();
        scene->SetBGMPlayed(false);
        if (!scene->LoadFile(Path))
        {
            std::cerr << "[SceneManager] Failed to restore scene! Path was: " << Path << std::endl;
            return false;
        }

        std::cout << "[SceneManager] Scene Restored from Backup." << std::endl;
    }


    if (auto it = m_scenes.find(m_originalSceneName); it != m_scenes.end())
    {
        m_active = it->second.get();
        return true;
    }

    return false;
   
}

//02_03
//bool SceneManager::RestoreActiveScene()
//{
//    //if (!m_active) return false;
//
//       //m_active->Clear();
//
//       //
//       //std::string fullPath = ResourceManager::Instance().ResolveFullPath(m_backupPath);
//
//       //if (m_active->LoadFile(fullPath))
//       //{
//       //    m_active->SetName(m_originalSceneName);
//
//       //    std::cout << "[SceneManager] Scene Restored from Backup." << std::endl;
//       //    return true;
//       //}
//
//       //std::cerr << "[SceneManager] Failed to restore scene! Path was: " << fullPath << std::endl;
//       //return false;
//}