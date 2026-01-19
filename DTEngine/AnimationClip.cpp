#include "pch.h"
#include "AnimationClip.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SimpleMathHelper.h"


bool AnimationClip::LoadFile(const std::string& fullPath)
{
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
        aiComponent_MESHES |
        aiComponent_MATERIALS |
        aiComponent_TEXTURES |
        aiComponent_LIGHTS |
        aiComponent_CAMERAS |
        aiComponent_COLORS |
        aiComponent_TEXCOORDS
    );

    const aiScene* scene = importer.ReadFile(fullPath,
        aiProcess_ConvertToLeftHanded |
        aiProcess_RemoveComponent
    );

    if (!scene || !scene->mAnimations || scene->mNumAnimations == 0)
        return false;

    const aiAnimation* anim = scene->mAnimations[0];

    Name = anim->mName.C_Str();
    Duration = anim->mDuration; // ticks 단위
    TicksPerSecond = (anim->mTicksPerSecond != 0.0) ? anim->mTicksPerSecond : 25.0;

    Channels.clear();
    Channels.reserve(anim->mNumChannels);

    for (unsigned int i = 0; i < anim->mNumChannels; ++i)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        std::string nodeName = channel->mNodeName.C_Str();

  
        BoneChannel ch;
        ch.BoneName = nodeName;

        // Position Keys
        ch.PositionKeys.reserve(channel->mNumPositionKeys);
        for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k)
        {
            auto& key = channel->mPositionKeys[k];

            key.mValue *= 0.01f;

            ch.PositionKeys.push_back({ key.mTime, Vector3(key.mValue.x, key.mValue.y, key.mValue.z) });
        }

        // Rotation Keys
        ch.RotationKeys.reserve(channel->mNumRotationKeys);
        for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k)
        {
            const auto& key = channel->mRotationKeys[k];
            // Assimp: (w,x,y,z), SimpleMath Quaternion(x,y,z,w)
            ch.RotationKeys.push_back({ key.mTime, Quaternion(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w) });
        }

        // Scale Keys
        ch.ScaleKeys.reserve(channel->mNumScalingKeys);
        for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k)
        {
            const auto& key = channel->mScalingKeys[k];
            ch.ScaleKeys.push_back({ key.mTime, Vector3(key.mValue.x, key.mValue.y, key.mValue.z) });
        }

        if (ch.PositionKeys.empty() && ch.RotationKeys.empty() && ch.ScaleKeys.empty())
            continue;

        Channels.push_back(std::move(ch));
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
