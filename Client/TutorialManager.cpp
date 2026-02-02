#include "pch.h"
#include "TutorialManager.h"
#include "GameObject.h"
#include "Text.h"  
#include "UIButton.h" 
#include "InputManager.h"

BEGINPROPERTY(TutorialManager)
DTPROPERTY(TutorialManager, leftChat);
DTPROPERTY(TutorialManager, rightChat);
DTPROPERTY(TutorialManager, leftChatText);
DTPROPERTY(TutorialManager, rightChatText);
DTPROPERTY(TutorialManager, m_queenUI);
DTPROPERTY(TutorialManager, m_catUI);
DTPROPERTY(TutorialManager, m_catUI2);
DTPROPERTY(TutorialManager, m_firstNextButton);
DTPROPERTY(TutorialManager, m_secondNextButton);
DTPROPERTY(TutorialManager, m_catText2);
DTPROPERTY(TutorialManager, m_BattleStart);
DTPROPERTY(TutorialManager, m_victoryUI);
ENDPROPERTY()

void TutorialManager::Awake()
{

    //if (leftChat) leftChat->SetActive(false);
    //if (rightChat) rightChat->SetActive(false);
    //if (m_queenUI) m_queenUI->SetActive(false);
    //if (m_catUI) m_catUI->SetActive(false);
}

void TutorialManager::Start()
{

    NextStep();
}

void TutorialManager::Update(float deltaTime)
{
	if (InputManager::Instance().GetKeyDown(KeyCode::Space) || InputManager::Instance().GetKeyDown(KeyCode::Enter))
    {
        NextStep();
	}
}

void TutorialManager::NextStep()
{
    if(m_canProceedToNextStep == false)
		return;

    m_CurrentStepIndex++;

    switch (m_CurrentStepIndex)
    {
    case 0: 
    {
        if (leftChat) leftChat->SetActive(true);
		if (m_firstNextButton) m_firstNextButton->SetActive(true);

        if (leftChatText)
            leftChatText->SetText(L"여긴 어디지?");
    }
    break;

    case 1: 
    {
        if (leftChat) leftChat->SetActive(false);
        if (rightChat) rightChat->SetActive(true);

        if (rightChatText)
            rightChatText->SetText(L"이곳은 규칙이 지배하는 보드랜드.\n자신만의 규칙을 가지지 못한 자는 이곳에 \n 필요 없느니라!");

        if (m_queenUI) m_queenUI->SetActive(true);
    }
    break;

    case 2:
    {
		if (m_queenUI) m_queenUI->SetActive(false);
		if (rightChat) rightChat->SetActive(false);


        // 체스 병사 엘리스에게 접근 후 공격 
        // 엘리스 체력바 점차 감소 ~~~
    }
    break;

    case 3:
    {
        // 체서 고양이 UI 애니메이션
    }
    break;

    case 4:
    {
		if (m_catUI) m_catUI->SetActive(true);
        if (rightChatText)
            rightChatText->SetText(L"이곳에서 나가려면 너만의 규칙을 \n정해야만 하지");
    }
    break;
    case 5:
    {
        if (rightChat) rightChat->SetActive(false);
        if (leftChat) leftChat->SetActive(true);
        if (leftChatText) leftChatText->SetText(L"규칙을 어떻게 정해야 하지?");
    }
    break;
    case 6:
    {
        if (rightChat) rightChat->SetActive(true);
        if (leftChat) leftChat->SetActive(false);
        if (rightChatText) rightChatText->SetText(L"심심하던 참이니 \n내가 한번 도와줘 볼까?");
    }
    break;
    case 7:
    {
        if (rightChat) rightChat->SetActive(false);
    }
    break;
    case 8: 
    {    
		if (m_catUI) m_catUI->SetActive(false);
		if (m_firstNextButton) m_firstNextButton->SetActive(false);

		if (m_secondNextButton) m_secondNextButton->SetActive(true);
		if (m_catUI2) m_catUI2->SetActive(true);
        if (m_catText2)
			m_catText2->SetText(L"네 자신을 지키려면 \n체스 병사의 도움이 필요해!");
    }
    break;
    case 9:
    {
        if(m_catText2)
			m_catText2->SetText(L"빛나는 타일을 눌러봐");
    }
    break;
    case 10:
    {
        if (m_catText2)
			m_catText2->SetText(L"해당 위치에 코스트를 지불하고 \n체스 병사를 소환할 수 있어");
    }
    break;
    case 11:
    {
        if (m_catText2) {
			m_catText2->SetText(L"적은 지정된 경로를\n따라서 움직여");
        }
    }
    break;
    case 12:
    {
        if (m_catText2) {
			m_catText2->SetText(L"병사를 선택해서 이동과 전투\n규칙을 정해줄 수 있어");
        }
    }
    break;
    case 13:
    {
        if (m_catText2) {
            m_catText2->SetText(L"대기 규칙을 적용하면\n병사가 방어타일로 이동해");
        }
    }
    break;
    case 14:
    {
        if(m_catText2) {
            m_catText2->SetText(L"자, 모든 규칙을 정했다면 \n전투를 실행해 봐!");
		}
		if (m_BattleStart) m_BattleStart->SetActive(true);
    }
    break;
    case 15:
    {
        if (m_BattleStart) m_BattleStart->SetActive(false);
        if (m_catText2) m_catText2->SetText(L"적이 일정 범위 내에 들어왔을 때 \n전투가 벌어지게 돼");
    }
    break;
    case 16:
    {
        if (m_catUI2) m_catUI2->SetActive(false);
        if (m_victoryUI) m_victoryUI->SetActive(true);

    }
    break;

    default:
        std::cout << "튜토리얼 끝" << std::endl;
        break;
    }
}