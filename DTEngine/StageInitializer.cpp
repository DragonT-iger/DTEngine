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
        stageFirstText = L"추격 전략을 사용하면 금이 간 벽을 \n부수고 지나갈 수 있으니 참고해"; 
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 2:
        currentMap = m_mapData2;
        stageFirstText = L"화면 하단의 아이템창을 누른 뒤, \n대상을 클릭해서 아이템을 사용할 수 있어";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 3:
        currentMap = m_mapData3;
        stageFirstText = L"병종을 고려해서 잘 배치해 봐";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 4:
        currentMap = m_mapData4;
        stageFirstText = L"마우스 왼쪽 버튼을 클릭한 채로 \n마우스를 움직여 화면을 돌릴 수 있어";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 5:
        currentMap = m_mapData5;
        stageFirstText = L"여왕의 장미에 닿으면 피해를 입으니 \n돌아가는 게 좋을 거야";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 6:
        currentMap = m_mapData6;
        stageFirstText = L"맵이 커졌다고 당황하지 말고 \n알맞은 규칙을 잘 세워 보라고!";
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