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

DTPROPERTY(StageInitializer, m_mushroom)
DTPROPERTY(StageInitializer, m_magicBottle)
ENDPROPERTY()

void StageInitializer::Awake()
{
    int currentLevel = GameManager::Instance()->GetStageLevel();

    TilemapData* currentMap = nullptr;
    std::wstring stageFirstText;

    switch (currentLevel)
    {
    case 1:
        currentMap = m_mapData1;
        stageFirstText = L"Stage 1"; 
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 2:
        currentMap = m_mapData2;
        stageFirstText = L"Stage 2";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 3:
        currentMap = m_mapData3;
        stageFirstText = L"Stage 3";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 4:
        currentMap = m_mapData4;
        stageFirstText = L"Stage 4";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 5:
        currentMap = m_mapData5;
        stageFirstText = L"Stage 5";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 6:
        currentMap = m_mapData6;
        stageFirstText = L"Stage 6";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
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