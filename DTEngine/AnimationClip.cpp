#include "pch.h"
#include "AnimationClip.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SimpleMathHelper.h"


bool AnimationClip::LoadFile(const std::string& fullPath)
{
    Assimp::Importer importer;

    // [최적화] Mesh, Material, Texture, Light, Camera 등 불필요한 요소 제거 (RVC = Remove Component)
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
        aiComponent_MESHES |
        aiComponent_MATERIALS |
        aiComponent_TEXTURES |
        aiComponent_LIGHTS |
        aiComponent_CAMERAS |
        aiComponent_COLORS |
        aiComponent_TEXCOORDS
    );

    // 왼손 좌표계 변환은 Model과 동일하게 유지해야 회전 방향이 맞음
    const aiScene* scene = importer.ReadFile(fullPath,
        aiProcess_ConvertToLeftHanded |
        aiProcess_RemoveComponent // 위에서 설정한 RVC 플래그 적용
    );

    if (!scene || !scene->mAnimations)
    {
        // 애니메이션이 없는 파일이거나 로드 실패
        return false;
    }

    // 보통 하나의 파일에 하나의 애니메이션 클립을 담는다고 가정 (index 0)
    // 여러 개라면 여기서 루프를 돌거나 인자로 인덱스를 받아야 함
    const aiAnimation* anim = scene->mAnimations[0];

    this->Name = anim->mName.C_Str();
    this->Duration = (float)anim->mDuration;

    // TicksPerSecond가 0인 경우(일부 포맷), 기본값 25.0f로 설정
    this->TicksPerSecond = (anim->mTicksPerSecond != 0.0f) ? (float)anim->mTicksPerSecond : 25.0f;

    // 채널(Bone) 데이터 파싱
    Channels.resize(anim->mNumChannels);

    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        BoneChannel& myChannel = Channels[i];

        std::string nodeName = channel->mNodeName.C_Str();

        if (nodeName == "Armature") continue;


        myChannel.BoneName = channel->mNodeName.C_Str();

        // 1. Position Keys
        for (unsigned int k = 0; k < channel->mNumPositionKeys; k++)
        {
            aiVector3D val = channel->mPositionKeys[k].mValue;
            float time = (float)channel->mPositionKeys[k].mTime;
            myChannel.PositionKeys.push_back({ time, Vector3(val.x, val.y, val.z) });
        }

        // 2. Rotation Keys
        for (unsigned int k = 0; k < channel->mNumRotationKeys; k++)
        {
            aiQuaternion val = channel->mRotationKeys[k].mValue;
            float time = (float)channel->mRotationKeys[k].mTime;
            // Assimp(w, x, y, z) -> SimpleMath(x, y, z, w) 생성자 순서 주의
            myChannel.RotationKeys.push_back({ time, Quaternion(val.x, val.y, val.z, val.w) });
        }

        // 3. Scale Keys
        for (unsigned int k = 0; k < channel->mNumScalingKeys; k++)
        {
            aiVector3D val = channel->mScalingKeys[k].mValue;
            float time = (float)channel->mScalingKeys[k].mTime;
            myChannel.ScaleKeys.push_back({ time, Vector3(val.x, val.y, val.z) });
        }
    }

    return true;
}

bool AnimationClip::SaveFile(const std::string& fullPath)
{
    return false;
}

void AnimationClip::Unload()
{
    Channels.clear();
}

Vector3 InterpolatePosition(const std::vector<KeyframeVector>& keys, float animationTime)
{
    if (keys.empty()) return Vector3(0,0,0);
    if (keys.size() == 1) return keys[0].Value;

    size_t p0Index = FindKeyIndex(animationTime, keys);
    size_t p1Index = p0Index + 1;
    if (p1Index >= keys.size()) return keys[p0Index].Value; // 끝 도달

    float t0 = keys[p0Index].Time;
    float t1 = keys[p1Index].Time;
    float factor = (animationTime - t0) / (t1 - t0);

    return Vector3::Lerp(keys[p0Index].Value, keys[p1Index].Value, factor);
}

Quaternion InterpolateRotation(const std::vector<KeyframeQuat>& keys, float animationTime)
{
    if (keys.empty()) return Quaternion();
    if (keys.size() == 1) return keys[0].Value;

    size_t p0Index = FindKeyIndex(animationTime, keys);
    size_t p1Index = p0Index + 1;
    if (p1Index >= keys.size()) return keys[p0Index].Value;

    float t0 = keys[p0Index].Time;
    float t1 = keys[p1Index].Time;
    float factor = (animationTime - t0) / (t1 - t0);


    return Quaternion::Slerp(keys[p0Index].Value, keys[p1Index].Value, factor);
}

Vector3 InterpolateScaling(const std::vector<KeyframeVector>& keys, float animationTime)
{
    if (keys.empty()) return Vector3(1,1,1);
    if (keys.size() == 1) return keys[0].Value;

    size_t p0Index = FindKeyIndex(animationTime, keys);
    size_t p1Index = p0Index + 1;
    if (p1Index >= keys.size()) return keys[p0Index].Value;

    float t0 = keys[p0Index].Time;
    float t1 = keys[p1Index].Time;
    float factor = (animationTime - t0) / (t1 - t0);

    return Vector3::Lerp(keys[p0Index].Value, keys[p1Index].Value, factor);
}
