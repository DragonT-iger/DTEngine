#include "pch.h"
#include "Animatior.h"
#include "SkeletalRsource.h"
#include "Skeletal.h"
#include "AnimationClip.h"
#include "ResourceManager.h"

BEGINPROPERTY(Animator)
DTPROPERTY_SETTER(Animator, Play, SetPlay)
DTPROPERTY_SETTER(Animator, Loop, SetLoop)
DTPROPERTY_SETTER(Animator, m_AniID, SetClip)
DTPROPERTY_SETTER(Animator, Animated_Time, SetTime)

ENDPROPERTY()



void Animator::Update(float deltaTime)
{
    if (!m_CurrentClip || !m_TargetSkeletal) return;
    if (!Play) return;


    float timeIncrement = deltaTime * m_CurrentClip->TicksPerSecond * Animated_Time;
    m_CurrentTime += timeIncrement;

    if (m_CurrentClip->Duration > 0.0f)
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentClip->Duration);

    BoneResource* boneRes = m_TargetSkeletal->GetBoneResource();




    for (size_t i = 0; i < m_CurrentClip->Channels.size(); ++i)
    {
        const auto& channel = m_CurrentClip->Channels[i];

        int boneIndex = m_TargetSkeletal->GetBoneIndex(channel.BoneName);
        if (boneIndex < 0) continue; 

        //channel.bonename이 기존 bonemap이랑 매칭 안되는 경우, animation 없는 bone은 그냥 넘어감. 얘는 Skeletal에서 default 값을 줌.
        //animation 채널이 있지만 position이 0인 경우는 bindpos로 넣어둠 

        auto& boneNode = boneRes->m_Bones[boneIndex];

        //어차피 SetClip 하면서 여기에 값이 들어갈텐데?? 
        Vector3 bindS, bindT;
        Quaternion bindR;
        boneNode.DefaultLocalMatrix.Decompose(bindS, bindR, bindT);

        Vector3 S = channel.ScaleKeys.empty() ? bindS : InterpolateScaling(channel.ScaleKeys, m_CurrentTime);
        Quaternion R = channel.RotationKeys.empty() ? bindR : InterpolateRotation(channel.RotationKeys, m_CurrentTime);
        Vector3 T = channel.PositionKeys.empty() ? bindT : InterpolatePosition(channel.PositionKeys, m_CurrentTime);

       
        R.Normalize();
        Matrix localMat = Matrix::CreateScale(S)
            * Matrix::CreateFromQuaternion(R)
            * Matrix::CreateTranslation(T);

            m_TargetSkeletal->SetBonePose(boneIndex, localMat);
        
    }
}

void Animator::SetClip(uint64_t id)
{
    if (id == m_AniID)
        return;

    m_AniID = id;

    m_TargetSkeletal = _GetOwner()->GetComponent<Skeletal>();
    if (!m_TargetSkeletal || m_TargetSkeletal->GetModelID() == 0) //컴포넌트는 있는데 모델이 없으면 안되니깐.
        return;


    if (id != 0)
    {
        m_CurrentTime = 0.0f;

        std::string path = AssetDatabase::Instance().GetPathFromID(m_AniID);
        if (!path.empty())
        {
            m_Animation_Name = path;
            m_CurrentClip = ResourceManager::Instance().Load<AnimationClip>(path);
        }
    }
    else 
        return;


    if (m_CurrentClip->Channels.size())
    {
        m_ChannelToBoneIndex.clear();
        BoneResource* boneRes = m_TargetSkeletal->GetBoneResource();

        for (const auto& channel : m_CurrentClip->Channels)
        {
            auto it = boneRes->m_BoneMapping.find(channel.BoneName);
            if (it != boneRes->m_BoneMapping.end())
            {
                m_ChannelToBoneIndex.push_back(it->second);
            }
            else
            {
                m_ChannelToBoneIndex.push_back(-1); // 매칭되는 뼈 없음
            }
        }

    }


}

void Animator::SetTime(float Speed)
{
    Animated_Time = Speed;
}
