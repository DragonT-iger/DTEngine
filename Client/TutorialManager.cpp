#include "pch.h"
#include "TutorialManager.h"
#include "GameObject.h"
#include "Text.h"  
#include "UIButton.h" 

BEGINPROPERTY(TutorialManager)
DTPROPERTY(TutorialManager, leftChat);
DTPROPERTY(TutorialManager, rightChat);
DTPROPERTY(TutorialManager, leftChatText);
DTPROPERTY(TutorialManager, rightChatText);
DTPROPERTY(TutorialManager, m_QueenUI);
DTPROPERTY(TutorialManager, m_CatUI);
ENDPROPERTY()

void TutorialManager::Awake()
{

    //if (leftChat) leftChat->SetActive(false);
    //if (rightChat) rightChat->SetActive(false);
    //if (m_QueenUI) m_QueenUI->SetActive(false);
    //if (m_CatUI) m_CatUI->SetActive(false);
}

void TutorialManager::Start()
{

    NextStep();
}

void TutorialManager::NextStep()
{
    m_CurrentStepIndex++;

    switch (m_CurrentStepIndex)
    {
    case 0: 
    {
        if (leftChat) leftChat->SetActive(true);

        if (leftChatText)
            leftChatText->SetText(L"여긴 어디지?");
    }
    break;

    case 1: 
    {
        if (leftChat) leftChat->SetActive(false);
        if (rightChat) rightChat->SetActive(true);

        if (rightChatText)
            rightChatText->SetText(L"이곳은 규칙이 지배하는 보드랜드.\n 자신만의 규칙을 가지지 못한 자는 이곳에 \n 필요 없느니라!");

        if (m_QueenUI) m_QueenUI->SetActive(true);
    }
    break;

    case 2:
    {
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
        if (rightChatText)
            rightChatText->SetText(L"이곳에서 나가려면 너만의 규칙을 \n 정해야만 하지");
    }

    default:
        std::cout << "튜토리얼 끝" << std::endl;
        break;
    }
}