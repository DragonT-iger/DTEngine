#pragma once

#include <SimpleMath.h>



namespace SimpleMathHelper {
    using Vector3 = DirectX::SimpleMath::Vector3;
    using Matrix = DirectX::SimpleMath::Matrix;
    using Quaternion = DirectX::SimpleMath::Quaternion;

    constexpr float Deg2Rad(float deg)
    {
        return deg * (DirectX::XM_PI / 180.0f);
    }
    constexpr float Rad2Deg(float rad)
    {
        return rad * (180.0f / DirectX::XM_PI);
    }


}