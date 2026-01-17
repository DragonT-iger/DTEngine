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

    // Loop 처리 (Duration(Tick)을 넘어가면 0으로)
    // Duration이 0이 아닌지 체크 필요
    if (m_CurrentClip->Duration > 0.0f)
    {
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentClip->Duration);
    }

    for (size_t i = 0; i < m_CurrentClip->Channels.size(); ++i)
    {
        int boneIdx = m_ChannelToBoneIndex[i];
        if (boneIdx == -1) continue; // 해당 모델에 없는 뼈 애니메이션은 무시

        const auto& channel = m_CurrentClip->Channels[i];

        Vector3 S = InterpolateScaling(channel.ScaleKeys, m_CurrentTime);
        Quaternion R = InterpolateRotation(channel.RotationKeys, m_CurrentTime);
        Vector3 T = InterpolatePosition(channel.PositionKeys, m_CurrentTime);

        Matrix localMat = Matrix::CreateScale(S)
            * Matrix::CreateFromQuaternion(R)
            * Matrix::CreateTranslation(T);

        m_TargetSkeletal->SetBonePose(boneIdx, localMat);
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
