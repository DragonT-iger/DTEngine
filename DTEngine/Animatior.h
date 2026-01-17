#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"


class Skeletal;
class AnimationClip;


class Animator : public MonoBehaviour
{

    DTGENERATED_BODY(Animator);


    Skeletal* m_TargetSkeletal = nullptr;

    AnimationClip* m_CurrentClip = nullptr;
    float m_CurrentTime = 0.0f;

    // 최적화: Channel 인덱스 -> Skeletal의 Bone 인덱스 캐싱
    std::vector<int> m_ChannelToBoneIndex;


    bool Play = false;
    bool Loop = false;


    std::string m_Animation_Name;

public:
    void Update(float deltaTime) override;

    //void Play(AnimationClip* clip); //Clip이랑 Skeletal Pointer를 연결-> Index에 넣는 느낌. 


    //void SetClip(AnimationClip* clip);



    void SetClip(std::string& Name);// { m_Animation_Name = Name; }
   const std::string& GetClipName() const { return m_Animation_Name; }

    //Toggle
    void SetPlay (bool On_Off) { Play = On_Off; }
    void SetLoop(bool Looping) { Loop = Looping; }
};