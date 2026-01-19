#include "pch.h"
#include "Animatior.h"
#include "SkeletalRsource.h"
#include "Skeletal.h"
#include "AnimationClip.h"
#include "ResourceManager.h"

BEGINPROPERTY(Animator)
DTPROPERTY_SETTER(Animator, Play, SetPlay)
DTPROPERTY_SETTER(Animator, Loop, SetLoop)
DTPROPERTY_SETTER(Animator, m_Animation_Name, SetClip)

ENDPROPERTY()



void Animator::Update(float deltaTime)
{
    if (!m_CurrentClip || !m_TargetSkeletal) return;
    if (!Play) return;


    float timeIncrement = deltaTime * m_CurrentClip->TicksPerSecond;
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

       
       // T *= 0.01f;

        R.Normalize();
        Matrix localMat = Matrix::CreateScale(S)
            * Matrix::CreateFromQuaternion(R)
            * Matrix::CreateTranslation(T);

            m_TargetSkeletal->SetBonePose(boneIndex, localMat);
        
    }
}


void Animator::SetClip(std::string& filename)
{
    //붙여넣기 시, 공백 들어가서 
    filename.erase(std::remove_if(filename.begin(), filename.end(),
        [](unsigned char x) { return std::isspace(x); }),
        filename.end());

    m_Animation_Name = filename;

    AnimationClip* clip = ResourceManager::Instance().Load<AnimationClip>(m_Animation_Name);


    if (!clip)
    {
        std::cout << "Clip data 찾을 수 없음: " << filename << std::endl;
        return;
    }

    m_TargetSkeletal = _GetOwner()->GetComponent<Skeletal>();

    if (m_TargetSkeletal)
    {
        m_CurrentClip = clip;
        m_CurrentTime = 0.0f;

     
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
    else
    {
        std::cout << "Animation Clip 연결 오류; Skeletal Component nullptr "<< std::endl;;
        return;
    }


}
