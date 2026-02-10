// MultiSpriteController.h
#pragma once
#include "SpriteEffect.h"

class MultiSpriteController : public SpriteEffect
{

    DTGENERATED_BODY(MultiSpriteController);


public:
    enum class TriggerMode
    {
        AfterPrevStart,
        AfterPrevFinish
    };

    struct Node
    {
        SpriteEffect* fx = nullptr;
        float delay = 0.f;        // prev trigger 이후 대기 시간
        Node* next = nullptr;

        // runtime state
        bool armed = false;       // 트리거를 기다리는 상태(대기 상태)
        bool triggered = false;   // prev 이벤트 감지됨
        float t = 0.f;            // delay 누적
        bool started = false;     // 실제로 Play 시작했는지 (중복 방지)
    };

public:
    // 에디터에서 넣을 4개 슬롯
    SpriteEffect* m_fx1 = nullptr;
    SpriteEffect* m_fx2 = nullptr;
    SpriteEffect* m_fx3 = nullptr;
    SpriteEffect* m_fx4 = nullptr;

    float m_delay1 = 0.f;
    float m_delay2 = 0.f;
    float m_delay3 = 0.f;
    float m_delay4 = 0.f;

    TriggerMode m_triggerMode = TriggerMode::AfterPrevStart;
    bool m_autoStart = false;     // 시작 시 fx1 바로 Play
    bool m_loopChain = false;    // 체인 전체 루프 여부

public:
    virtual void Start() override;
    virtual void Update(float dTime) override;

    void Initalize();


    void RebuildChain();
    void StartChain();           // 외부에서 강제로 시작
    void StopChain();            // 외부에서 강제로 정지

    void SetAutoDestroy(bool enable) { m_bAutoDestroy = enable; }
    void SetFollowTarget(GameObject* target) { m_followTarget = target; }

private:
    Node m_n1, m_n2, m_n3, m_n4;
    Node* m_head = nullptr;

    // prev 이벤트 감지용 상태
    bool m_prevStartedCache[4] = { false,false,false,false };
    bool m_prevFinishedCache[4] = { false,false,false,false };

    bool m_bAutoDestroy = false;     
    GameObject* m_followTarget = nullptr; 

private:
    bool HasJustStarted(SpriteEffect* fx, int idx);
    bool HasJustFinished(SpriteEffect* fx, int idx);
    void ArmFollowingNodes();
    bool IsChainComplete() const;
};
