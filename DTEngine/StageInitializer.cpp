#include "pch.h"
#include "StageInitializer.h"
#include "GameObject.h"
#include "../Client/GameManager.h"
#include "TilemapGenerator.h"
#include "../Client/BattleGrid.h"
#include "TilemapData.h"
#include "Text.h"

BEGINPROPERTY(StageInitializer)
DTPROPERTY(StageInitializer, m_mapData1)
DTPROPERTY(StageInitializer, m_mapData2)
DTPROPERTY(StageInitializer, m_mapData3)
DTPROPERTY(StageInitializer, m_mapData4)
DTPROPERTY(StageInitializer, m_mapData5)
DTPROPERTY(StageInitializer, m_mapData6)
DTPROPERTY(StageInitializer, m_generator)
//DTPROPERTY(StageInitializer, m_battleGrid)
//DTPROPERTY(StageInitializer, m_victoryWindow)
//DTPROPERTY(StageInitializer, m_loseWindow)
//DTPROPERTY(StageInitializer, m_settingWindow)
DTPROPERTY(StageInitializer, m_stageLevelText)
//DTPROPERTY(StageInitializer, m_stageDialogueText)
ENDPROPERTY()

void StageInitializer::Awake()
{
    int currentLevel = GameManager::Instance()->GetStageLevel();

    TilemapData* currentMap = nullptr;
    std::wstring stageFirstText;

    if (currentLevel == 1 || currentLevel == 2 || currentLevel == 3 || currentLevel == 4)
    {
        std::cout << currentLevel << " " << "그냥 브금\n";
        SoundManager::Instance().PlayBGM("BGM/BGM_Setting", true);
    }
    else if (currentLevel == 5 || currentLevel == 6)
    {
        std::cout << currentLevel << " " << "보스 브금\n";
        SoundManager::Instance().PlayBGM("BGM/Boss_Setting", true);
    }

    switch (currentLevel)
    {
    case 1:
        currentMap = m_mapData1;
        stageFirstText = L"Stage 1"; 
        break;
    case 2:
        currentMap = m_mapData2;
        stageFirstText = L"Stage 2";
        break;
    case 3:
        currentMap = m_mapData3;
        stageFirstText = L"Stage 3";
        break;
    case 4:
        currentMap = m_mapData4;
        stageFirstText = L"Stage 4";
        break;
    case 5:
        currentMap = m_mapData5;
        stageFirstText = L"Stage 5";
        break;
    case 6:
        currentMap = m_mapData6;
        stageFirstText = L"Stage 6";
        break;
    default:
        currentMap = m_mapData1;
        stageFirstText = L"Unknown Stage";
        break;
    }

    if (m_generator && currentMap)
    {
        m_generator->SetMapData(currentMap);
    }

    if (m_stageLevelText)
    {
        m_stageLevelText->SetText(stageFirstText);
    }

}