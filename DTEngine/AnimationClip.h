#pragma once
#include "IResource.h"

struct KeyframeVector { double Time; Vector3 Value; };
struct KeyframeQuat { double Time; Quaternion Value; };

// 뼈 하나에 대한 시간대별 변화 (채널)
struct BoneChannel
{
    std::string BoneName; // 대상 뼈 이름
    std::vector<KeyframeVector> PositionKeys;
    std::vector<KeyframeQuat>   RotationKeys;
    std::vector<KeyframeVector> ScaleKeys;
};


// 하나의 전체 동작 (예: "Walk", "Attack")
class AnimationClip : public IResource
{
public:

    virtual bool LoadFile(const std::string& fullPath) override;
    virtual bool SaveFile(const std::string& fullPath) override;
    virtual void Unload() override;



    std::string Name;
    double Duration;       // 전체 길이 (초 or 틱)
    double TicksPerSecond; // Assimp에서 받아옴 
    std::vector<BoneChannel> Channels; // 움직이는 모든 뼈들의 데이터
};


//Animation Helpers

template<typename T>
size_t FindKeyIndex(float animationTime, const std::vector<T>& keys)
{
    if (keys.empty()) return 0;

    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        if (animationTime < keys[i + 1].Time)
            return i;
    }
    return keys.size() - 1;
}

Vector3 InterpolatePosition(const std::vector<KeyframeVector>& keys, float animationTime);
Quaternion InterpolateRotation(const std::vector<KeyframeQuat>& keys, float animationTime);
Vector3 InterpolateScaling(const std::vector<KeyframeVector>& keys, float animationTime);

