#include "pch.h"
#include "DialogueController.h"
#include "GameObject.h"
#include "Text.h"          
#include "SceneManager.h"  
#include "InputManager.h"  
#include "KeyCode.h"
#include "GameManager.h"
#include "Scene.h"
#include "TilemapGenerator.h"

BEGINPROPERTY(DialogueController)
DTPROPERTY(DialogueController, m_aliceObject)
DTPROPERTY(DialogueController, m_redQueenObject)
DTPROPERTY(DialogueController, m_dialogueText)
DTPROPERTY(DialogueController, m_redQueenText)
DTPROPERTY(DialogueController, m_cheshireObject)
DTPROPERTY(DialogueController, m_cheshireText)
DTPROPERTY(DialogueController, m_tilemapGenerator5)
DTPROPERTY(DialogueController, m_tilemapGenerator6)
ENDPROPERTY()




void DialogueController::Start()
{
    m_dialogueLines.clear();

    if (GameManager::Instance()->GetStageLevel() <= 5)
    {
        m_dialogueLines.push_back({ Speaker::RedQueen, L"감히 내 병사들을 해치고 내 왕성에 들어오다니.\n규칙 없는 자 주제에, 무례하기까지 하구나!" });

        m_dialogueLines.push_back({ Speaker::Alice,     L"안녕하세요, 여왕 폐하.\n무례하게 굴 생각은 없었어요. 정말이에요." });

        m_dialogueLines.push_back({ Speaker::Alice,     L"전 그냥 여왕 폐하께 부탁을 드리러 왔어요.\n저를 집으로 돌려보내 주실 수 있나요?" });

        m_dialogueLines.push_back({ Speaker::Alice,     L"저에게 집으로 돌아가는 규칙을 만들어 주세요.\n체셔가 그러는데, 여왕 폐하께서는 그런 규칙을 만드실 수 있대요." });

        m_dialogueLines.push_back({ Speaker::RedQueen, L"내가 왜 그래야 하지? 나, 하트 여왕은, \n오로지 규칙 있는 자들을 위해서만 규칙을 만드니라!" });

        m_dialogueLines.push_back({ Speaker::RedQueen, L"경비병, 이번에야말로 저 고약한 녀석의 목을 쳐라!!!" });
      
    }
    else
    {

        m_dialogueLines.push_back({ Speaker::Alice,
            L"제가 이겼어요, 여왕 폐하!\n이제 날 집에 보내 줘요!!" });

        m_dialogueLines.push_back({ Speaker::RedQueen,
            L"무례한 녀석!\n괘씸한, 고약한, 무도한 놈 같으니라고!" });

        m_dialogueLines.push_back({ Speaker::RedQueen,
            L"규칙도 없는 주제에, 감히 이 나를 이겼다고?\n체셔 놈의 하잘것없는 속임수겠지!" });

        m_dialogueLines.push_back({ Speaker::RedQueen,
            L"용서할 수 없다.\n저 녀석의 목을 쳐라!!!!!" });

        m_dialogueLines.push_back({ Speaker::Alice,
            L"어쩌면 좋지? 여긴 어디야?",
            [this]() {
                if (m_tilemapGenerator5) m_tilemapGenerator5->_GetOwner()->SetActive(false);
                if (m_tilemapGenerator6) m_tilemapGenerator6->_GetOwner()->SetActive(true);
            }
            });

        m_dialogueLines.push_back({ Speaker::Cheshire,
            L"처형장에 온 걸 환영해, 앨리스.\n우리 여왕 폐하께서 화가 아주 많이 난 모양인데…." });

        m_dialogueLines.push_back({ Speaker::Alice,
            L"도와줘, 체셔!" });

        m_dialogueLines.push_back({ Speaker::Cheshire,
            L"아마도 이번이 마지막이겠어.\n살짝 발을 얹어 볼까?" });

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
    
    const DialogueData& currentData = m_dialogueLines[m_currentIndex];

    if (currentData.onStartAction != nullptr)
    {
        currentData.onStartAction();
    }

    if (currentData.speaker == Speaker::Alice)
    {
        if (m_aliceObject) m_aliceObject->SetActive(true);
        if (m_cheshireObject) m_cheshireObject->SetActive(false);
        if (m_redQueenObject) m_redQueenObject->SetActive(false);

        if (m_dialogueText)
        {
            m_dialogueText->SetText(currentData.text);
        }
    }
    else if (currentData.speaker == Speaker::RedQueen)
    {
        if (m_aliceObject) m_aliceObject->SetActive(false);
        if (m_cheshireObject) m_cheshireObject->SetActive(false);
        if (m_redQueenObject) m_redQueenObject->SetActive(true);

        if (m_redQueenText)
        {
            m_redQueenText->SetText(currentData.text);
        }
    }
    else if (currentData.speaker == Speaker::Cheshire)
    {
        if (m_aliceObject) m_aliceObject->SetActive(false);
        if (m_cheshireObject) m_cheshireObject->SetActive(true);
        if (m_redQueenObject) m_redQueenObject->SetActive(false);

        if (m_cheshireText)
        {
            m_cheshireText->SetText(currentData.text);
        }
    }
}