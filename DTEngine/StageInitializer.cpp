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
        stageFirstText = L"아이콘에 왕관 표시가 된 녀석이 보스야.\n'추격' 규칙으로 벽을 부수고 잡아 봐!"; 
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 2:
        currentMap = m_mapData2;    
        stageFirstText = L"전투 중에 불리한 상황이 된다면,\n 네가 얻은 포션이나 버섯을 사용해 봐.";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 3:
        currentMap = m_mapData3;    
        stageFirstText = L"나이트는 비숍에게, 비숍은 룩에게,\n 룩은 나이트에게 강해. 그걸 기억하라고.";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(false);
        m_magicBottle->SetActive(true);
        break;
    case 4:
        currentMap = m_mapData4;
        stageFirstText = L"마우스 오른쪽 버튼을 클릭한 채로 \n마우스를 움직여 화면을 돌릴 수 있어";
        if (!m_mushroom || !m_magicBottle)
            return;
        m_mushroom->SetActive(true);
        m_magicBottle->SetActive(false);
        break;
    case 5:
        currentMap = m_mapData5;
        stageFirstText = L"가시덤불에 닿으면 피해를 입으니, 잘 생각해.\n닿지 않으려면 어떻게 해야 할까?";
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