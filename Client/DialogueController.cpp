#include "DialogueController.h"
#include "GameObject.h"
#include "Text.h"          
#include "SceneManager.h"  
#include "InputManager.h"  
#include "KeyCode.h"
#include "GameManager.h"
#include "Scene.h"

BEGINPROPERTY(DialogueController)
DTPROPERTY(DialogueController, m_aliceObject)
DTPROPERTY(DialogueController, m_redQueenObject)
DTPROPERTY(DialogueController, m_dialogueText)
DTPROPERTY(DialogueController, m_redQueenText)
ENDPROPERTY()

void DialogueController::Start()
{
    m_dialogueLines.clear();

    if (GameManager::Instance()->GetStageLevel() <= 5) {

        // 처음이 엘리스입니다
        m_dialogueLines.push_back(L"당신은 누구시죠? 비켜주세요.");
        m_dialogueLines.push_back(L"안녕, 앨리스? 여기서 뭐 하는 거니?");
        m_dialogueLines.push_back(L"말도 안 돼... 길을 잃은 것 같아요.");
        m_dialogueLines.push_back(L"나는 이 구역의 지배자, 붉은 여왕이다!");
        m_dialogueLines.push_back(L"조용히 해라! 게임을 시작하지.");
    }

    else {
        m_dialogueLines.push_back(L"당신은 누구시죠2? 비켜주세요.");
        m_dialogueLines.push_back(L"안녕2, 앨리스? 여기서 뭐 하는 거니?");
        m_dialogueLines.push_back(L"말도 안 돼... 길을 잃은 것 같아요2.");
        m_dialogueLines.push_back(L"나는 이 구역의 지배자2, 붉은 여왕이다!");
        m_dialogueLines.push_back(L"조용히 해라! 게임을 시작하지.");
    }


    NextDialogue();
}

void DialogueController::Update(float deltaTime)
{
    if (InputManager::Instance().GetKeyDown(KeyCode::Space) ||
        InputManager::Instance().GetKeyDown(KeyCode::MouseLeft))
    {
        NextDialogue();
    }
}

void DialogueController::NextDialogue()
{
    m_currentIndex++;

    if (m_currentIndex >= m_dialogueLines.size())
    {
        SceneManager::Instance().LoadScene("MainGameScene"); 
        return;
    }

    bool isAliceTurn = (m_currentIndex % 2 == 0);

    if (m_aliceObject)
    {
        m_aliceObject->SetActive(isAliceTurn);
        m_dialogueText->SetText(m_dialogueLines[m_currentIndex]);
    }

    if (m_redQueenObject)
    {
        m_redQueenObject->SetActive(!isAliceTurn);
        m_redQueenText->SetText(m_dialogueLines[m_currentIndex]);
    }
}