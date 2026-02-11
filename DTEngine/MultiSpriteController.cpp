#pragma once
#include "pch.h"
#include "MultiSpriteController .h"



BEGINPROPERTY(MultiSpriteController)
DTPROPERTY(MultiSpriteController, m_fx1)
DTPROPERTY(MultiSpriteController, m_fx2)
DTPROPERTY(MultiSpriteController, m_fx3)
DTPROPERTY(MultiSpriteController, m_fx4)

DTPROPERTY(MultiSpriteController, m_delay1)
DTPROPERTY(MultiSpriteController, m_delay2)
DTPROPERTY(MultiSpriteController, m_delay3)
DTPROPERTY(MultiSpriteController, m_delay4)

DTPROPERTY(MultiSpriteController, m_autoStart)


ENDPROPERTY()



void MultiSpriteController::Start()
{
    RebuildChain();

    StopChain();

    if (m_autoStart)
        StartChain();
}


void MultiSpriteController::Initalize()
{
    RebuildChain();
    StopChain();
}

void MultiSpriteController::RebuildChain()
{
    m_n1 = Node{ m_fx1, m_delay1, nullptr };
    m_n2 = Node{ m_fx2, m_delay2, nullptr };
    m_n3 = Node{ m_fx3, m_delay3, nullptr };
    m_n4 = Node{ m_fx4, m_delay4, nullptr };

    // 유효한 노드만 연결 (linked list)
    Node* nodes[4] = { &m_n1, &m_n2, &m_n3, &m_n4 };

    Node* prev = nullptr;
    m_head = nullptr;

    for (int i = 0; i < 4; ++i)
    {
        if (!nodes[i]->fx) continue;

        if (!m_head) m_head = nodes[i];
        if (prev) prev->next = nodes[i];
        prev = nodes[i];
    }
    if (prev) prev->next = nullptr;

    // 캐시 초기화
    for (int i = 0; i < 4; ++i)
    {
        m_prevStartedCache[i] = false;
        m_prevFinishedCache[i] = false;
    }
}

void MultiSpriteController::StartChain()
{
    if (!m_head || !m_head->fx) return;

    for (Node* n = m_head; n; n = n->next)
    {
        n->armed = false;
        n->triggered = false;
        n->t = 0.f;
        n->started = false;
    }

    m_head->fx->_GetOwner()->SetActive(true);
    m_head->fx->Restart();
    m_head->started = true;
    // 나머지는 대기 상태로 arm
    ArmFollowingNodes();
}

void MultiSpriteController::StopChain()
{
    for (Node* n = m_head; n; n = n->next)
    {
        if (n->fx) n->fx->Stop();
        n->armed = false;
        n->triggered = false;
        n->t = 0.f;
        n->started = false;
    }
}

void MultiSpriteController::ArmFollowingNodes()
{
    // head 다음부터 “앞 이벤트 기다리는” 상태로 만든다
    Node* prev = m_head;
    Node* cur = m_head ? m_head->next : nullptr;

    while (cur)
    {
        cur->armed = true;       // prev 이벤트 기다림
        cur->triggered = false;
        cur->t = 0.f;
        cur->started = false;

        prev = cur;
        cur = cur->next;
    }
}

bool MultiSpriteController::HasJustStarted(SpriteEffect* fx, int idx)
{
    if (!fx) return false;
    bool now = fx->IsPlaying();          // 시작 판단을 더 정확히 하려면 fx->HasEverStarted 같은 게 있으면 더 좋음
    bool just = (now && !m_prevStartedCache[idx]);
    m_prevStartedCache[idx] = now || m_prevStartedCache[idx]; // 한번 시작하면 true 유지
    return just;
}

bool MultiSpriteController::HasJustFinished(SpriteEffect* fx, int idx)
{
    if (!fx) return false;
    bool now = fx->IsFinished();
    bool just = (now && !m_prevFinishedCache[idx]);
    m_prevFinishedCache[idx] = now || m_prevFinishedCache[idx]; // 한번 끝나면 true 유지
    return just;
}

bool MultiSpriteController::IsChainComplete() const
{
    Node* last = nullptr;
    for (Node* n = m_head; n; n = n->next) last = n;
    if (!last || !last->fx) return true;
    return last->fx->IsFinished() || (!last->fx->IsPlaying() && last->started); // 정책에 따라
}

void MultiSpriteController::Update(float dTime)
{
  /*  if (m_followTarget != nullptr)
    {
        Vector3 targetPos = m_followTarget->GetTransform()->GetPosition();
        GetTransform()->SetPosition(targetPos);
    }*/


    if (!m_head) return;

    auto idxOf = [&](SpriteEffect* fx)->int {
        if (fx == m_fx1) return 0;
        if (fx == m_fx2) return 1;
        if (fx == m_fx3) return 2;
        if (fx == m_fx4) return 3;
        return 0;
        };


    Node* prev = m_head;
    Node* cur = m_head->next;

    while (cur)
    {
        if (cur->fx && cur->armed && !cur->started)
        {
            const int pidx = idxOf(prev->fx);

            bool prevEvent = false;
            if (m_triggerMode == TriggerMode::AfterPrevStart)
                prevEvent = HasJustStarted(prev->fx, pidx) || m_prevStartedCache[pidx]; // “시작된 상태면” 트리거로 인정(정책)
            else
                prevEvent = HasJustFinished(prev->fx, pidx);

            if (prevEvent)
            {
                cur->triggered = true;
                
            }

            if (cur->triggered)
            {
                cur->fx->_GetOwner()->SetActive(true);
                cur->t += dTime;
                if (cur->t >= cur->delay)
                {
                    cur->fx->Restart();
                    cur->started = true;
                    cur->armed = false;
                }
            }
        }

        prev = cur;
        cur = cur->next;
    }

    bool isChainComplete = true;

    Node* checkNode = m_head;

    if (checkNode == nullptr)
    {
        isChainComplete = true;
    }

    while (checkNode)
    {
        if (checkNode->fx)
        {
            
            bool isNodeFinished = checkNode->started && checkNode->fx->IsFinished();

            if (!isNodeFinished)
            {
                isChainComplete = false;
                break; 
            }
        }
        checkNode = checkNode->next;
    }

    if (IsChainComplete())
    {

        if (m_bAutoDestroy)
        {
            this->_GetOwner()->SetActive(false);
        }

        if (m_loopChain)
        {
            StartChain();
        }
        else
        {
            this->_GetOwner()->SetActive(false);
        }
    }
}
