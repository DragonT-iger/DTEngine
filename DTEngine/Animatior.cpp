#include "pch.h"
#include "Animatior.h"
#include "SkeletalRsource.h"
#include "Skeletal.h"
#include "AnimationClip.h"
#include "ResourceManager.h"
#include "Rigid.h"
#include "Model.h"
#include <iomanip>
BEGINPROPERTY(Animator)

DTPROPERTY_SETTER(Animator, Play, SetPlay)
DTPROPERTY_SETTER(Animator, Loop, SetLoop)
DTPROPERTY_SETTER(Animator, m_AniID, SetClip)
DTPROPERTY_SETTER(Animator, Animated_Time, SetTime)

ENDPROPERTY()

void Animator::Update(float deltaTime)
{
    if (!m_CurrentClip || !Play) return;

    float timeIncrement = deltaTime * m_CurrentClip->TicksPerSecond * Animated_Time;
    m_CurrentTime = fmod(m_CurrentTime + timeIncrement, m_CurrentClip->Duration);

    m_CurrentTime += timeIncrement;

    if (m_CurrentTime >= m_CurrentClip->Duration) {
        if (Loop) 
        {
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentClip->Duration);
        }
        else {
            m_CurrentTime = m_CurrentClip->Duration; 

            Play = false; 
            m_Done = true;

            return;
        }
    }


    for (size_t i = 0; i < m_CurrentClip->Channels.size(); ++i)
    {
        int targetIdx = m_ChannelToBoneIndex[i];
        if (targetIdx < 0) continue;

        const auto& channel = m_CurrentClip->Channels[i];

        Vector3 bindS, bindT;
        Quaternion bindR;

        if (m_IsRigidMode)
            m_TargetRigid->GetNodeResource()->Nodes[targetIdx].DefaultLocalMatrix.Decompose(bindS, bindR, bindT);
        else
            m_TargetSkeletal->GetBoneResource()->m_Bones[targetIdx].DefaultLocalMatrix.Decompose(bindS, bindR, bindT);

        Vector3 S = channel.ScaleKeys.empty() ? bindS : InterpolateScaling(channel.ScaleKeys, m_CurrentTime);
        Quaternion R = channel.RotationKeys.empty() ? bindR : InterpolateRotation(channel.RotationKeys, m_CurrentTime);
        Vector3 T = channel.PositionKeys.empty() ? bindT : InterpolatePosition(channel.PositionKeys, m_CurrentTime);

        R.Normalize();
        Matrix localMat = Matrix::CreateScale(S) * Matrix::CreateFromQuaternion(R) * Matrix::CreateTranslation(T);

        if (m_IsRigidMode) m_TargetRigid->SetNodePose(targetIdx, localMat);

        else m_TargetSkeletal->SetBonePose(targetIdx, localMat);


    }

    //std::cout << "OBJ NAME" << this->_GetOwner()->GetName() << deltaTime << std::endl;

}

void Animator::SetClip(uint64_t id)
{
    if (id == m_AniID)
        return;

    m_AniID = id;

    m_TargetSkeletal = _GetOwner()->GetComponent<Skeletal>();
    m_TargetRigid = _GetOwner()->GetComponent<Rigid>();

    uint64_t modelID = 0;
    if (m_TargetRigid) modelID = m_TargetRigid->GetModelID();
    else if (m_TargetSkeletal) modelID = m_TargetSkeletal->GetModelID();

    if (modelID == 0) return;

    std::string modelPath = AssetDatabase::Instance().GetPathFromID(modelID);
    Model* pModel = ResourceManager::Instance().Load<Model>(modelPath);
    if (!pModel) return;

    m_IsRigidMode = (pModel->GetNodeResource() != nullptr);

    if (m_AniID != 0)
    {
        m_CurrentTime = 0.0f;
        std::string aniPath = AssetDatabase::Instance().GetPathFromID(m_AniID);
        if (!aniPath.empty())
        {
            m_Animation_Name = aniPath;
            m_CurrentClip = ResourceManager::Instance().Load<AnimationClip>(aniPath);
        }
    }

    if (!m_CurrentClip) return;

    if (!m_CurrentClip->Channels.empty())
    {
        m_ChannelToBoneIndex.clear();
        m_ChannelToBoneIndex.reserve(m_CurrentClip->Channels.size());

        if (m_IsRigidMode)
        {
            NodeResource* nodeRes = pModel->GetNodeResource();
            for (const auto& channel : m_CurrentClip->Channels)
            {
                int idx = nodeRes->GetIndex(channel.BoneName);
                m_ChannelToBoneIndex.push_back(idx);
            }
        }
        else
        {
            BoneResource* boneRes = pModel->GetBone();
            if (!boneRes) return;
           
            for (const auto& channel : m_CurrentClip->Channels)
            {
                std::string channelName = channel.BoneName;
                int foundIdx = -1;

              
                for (size_t b = 0; b < boneRes->m_Bones.size(); ++b) 
                {
                    if (boneRes->m_Bones[b].name == channelName) 
                    {
                        foundIdx = (int)b;
                        break;
                    }
                }
               
                m_ChannelToBoneIndex.push_back(foundIdx);
            }
        }
    }


}
void Animator::SetTime(float Speed)
{
    Animated_Time = Speed;
}
