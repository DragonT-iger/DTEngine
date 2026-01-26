#include "pch.h"
#include "Animatior.h"
#include "SkeletalRsource.h"
#include "Skeletal.h"
#include "AnimationClip.h"
#include "ResourceManager.h"
#include "Rigid.h"
#include "Model.h"

BEGINPROPERTY(Animator)

DTPROPERTY_SETTER(Animator, Play, SetPlay)
DTPROPERTY_SETTER(Animator, Loop, SetLoop)
DTPROPERTY_SETTER(Animator, m_AniID, SetClip)
DTPROPERTY_SETTER(Animator, Animated_Time, SetTime)

ENDPROPERTY()



void Animator::Update(float deltaTime)
{
    // 리지드/스켈레탈 중 하나라도 있어야 업데이트 가능
    if (!m_CurrentClip || !Play) return;

    float timeIncrement = deltaTime * m_CurrentClip->TicksPerSecond * Animated_Time;
    m_CurrentTime = fmod(m_CurrentTime + timeIncrement, m_CurrentClip->Duration);

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

    //조건 수정 필요
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

    // 4. 채널 매핑 최적화 (모드 분기)
    if (!m_CurrentClip->Channels.empty())
    {
        m_ChannelToBoneIndex.clear();
        m_ChannelToBoneIndex.reserve(m_CurrentClip->Channels.size());

        if (m_IsRigidMode)
        {
            // Rigid 모드: NodeResource의 NodeMapping 사용
            NodeResource* nodeRes = pModel->GetNodeResource();
            for (const auto& channel : m_CurrentClip->Channels)
            {
                int idx = nodeRes->GetIndex(channel.BoneName);
                m_ChannelToBoneIndex.push_back(idx);
            }
        }
        else
        {
            // Skeletal 모드: BoneResource의 BoneMapping 사용
            BoneResource* boneRes = pModel->GetBone();
            if (!boneRes) return;

            for (const auto& channel : m_CurrentClip->Channels)
            {
                auto it = boneRes->m_BoneMapping.find(channel.BoneName);
                if (it != boneRes->m_BoneMapping.end())
                    m_ChannelToBoneIndex.push_back(it->second);
                else
                    m_ChannelToBoneIndex.push_back(-1);
            }
        }
    }
}
void Animator::SetTime(float Speed)
{
    Animated_Time = Speed;
}
