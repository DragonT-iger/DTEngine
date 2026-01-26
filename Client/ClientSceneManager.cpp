#include "pch.h"
#include "ClientSceneManager.h"
#include "SceneManager.h" 
#include "Scene.h"
#include <iostream>

void ClientSceneManager::LoadScene(const std::string& sceneName)
{
    if (sceneName == "TutorialScene")
    {
        InitTitleScene();
    }
    else if (sceneName == "Stage1" || sceneName == "Stage2")
    {
        InitInGameScene();
    }
    else if (sceneName == "EndingScene")
    {
        InitEndingScene();
    }

    SceneManager::Instance().LoadScene(sceneName);
}

void ClientSceneManager::InitTitleScene()
{
    std::cout << "[ClientSceneManager] Title Scene Initialization..." << std::endl;
    // 예: 배경음악 재생, 글로벌 변수 초기화 등
}

void ClientSceneManager::InitInGameScene()
{
    std::cout << "[ClientSceneManager] In-Game Initialization..." << std::endl;
}

void ClientSceneManager::InitEndingScene()
{
    std::cout << "[ClientSceneManager] Ending Scene Initialization..." << std::endl;
}