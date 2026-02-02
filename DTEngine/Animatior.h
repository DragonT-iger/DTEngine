#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"


class Skeletal;
class AnimationClip;
class Rigid;

class Animator : public MonoBehaviour
{

    DTGENERATED_BODY(Animator);


    Skeletal* m_TargetSkeletal = nullptr;
    AnimationClip* m_CurrentClip = nullptr;

    float m_CurrentTime = 0.0f;

    // 최적화: Channel 인덱스 -> Skeletal의 Bone 인덱스 캐싱
    std::vector<int> m_ChannelToBoneIndex;


    std::string m_Animation_Name;

public:
    void Update(float deltaTime) override;
    void SetClip(uint64_t id);
    void SetTime(float Speed);
   const std::string& GetClipName() const { return m_Animation_Name; }

    //Toggle
   void SetPlay(bool On_Off);/* { Play = On_Off; m_Done = !On_Off; }*/ //애니 끄면 done인거고, 애니 키면 done이 아니니깐? 초기화 개념.
    void SetLoop(bool Looping) { Loop = Looping; }

    bool GetAnimationDone() { return m_Done; }

private:

    bool Play = false;
    bool Loop = false;
    float Animated_Time = 1.0f;


    bool m_Done = false;

    Rigid* m_TargetRigid = nullptr; 
    bool      m_IsRigidMode = false;

    bool m_ClipChanged = false;

    uint64_t m_AniID = 0;

};