#include "pch.h"
#include "ClientSceneManager.h"
#include "SceneManager.h" 
#include "Scene.h"
#include <iostream>
#include "GameManager.h"


//Sound 처리는 Scene에서 처리. 

void ClientSceneManager::LoadScene(const std::string& sceneName)
{

    // 예시임
    if (sceneName == "TutorialScene")
    {
        InitTutorialScene();
        //GameManager::Instance()->m_leftHealth = 10;
    }
    else if (sceneName == "Stage1" || sceneName == "Stage2" || sceneName == "Stage3" || sceneName == "Stage4" || sceneName == "Stage5")
    {
        InitInGameScene();
    }
    else if (sceneName == "EndingScene")
    {
        InitEndingScene();
    }

    else if (sceneName == "TitleScene")
    {
        InitTitleScene();
    }
    
    std::string path = "Scenes\\" + sceneName;

    //SceneManager::Instance().RegisterScene(path);
    SceneManager::Instance().LoadScene(sceneName);
}

void ClientSceneManager::InitTitleScene()
{
    std::cout << "[ClientSceneManager] Title Scene Initialization..." << std::endl;
    GameManager::Instance()->ResetLifeAndMoney();
    GameManager::Instance()->SetStageLevel(1);
}

void ClientSceneManager::InitInGameScene()
{
    std::cout << "[ClientSceneManager] In-Game Initialization..." << std::endl;
}

void ClientSceneManager::InitEndingScene()
{
    std::cout << "[ClientSceneManager] Ending Scene Initialization..." << std::endl;
}

void ClientSceneManager::InitTutorialScene()
{

}