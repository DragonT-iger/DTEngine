#pragma once

#include <DirectXTK/SimpleMath.h>
#include <algorithm>
#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const DirectX::SimpleMath::Matrix& m)
{
    os  << "[" << m._11 << ", " << m._12 << ", " << m._13 << ", " << m._14 << "]\n"
        << "[" << m._21 << ", " << m._22 << ", " << m._23 << ", " << m._24 << "]\n"
        << "[" << m._31 << ", " << m._32 << ", " << m._33 << ", " << m._34 << "]\n"
        << "[" << m._41 << ", " << m._42 << ", " << m._43 << ", " << m._44 << "]\n";
    return os;
}

using Matrix = DirectX::SimpleMath::Matrix;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Quaternion = DirectX::SimpleMath::Quaternion;

namespace SimpleMathHelper {

    constexpr float Deg2Rad(float deg)
    {
        return deg * (DirectX::XM_PI / 180.0f);
    }
    constexpr float Rad2Deg(float rad)
    {
        return rad * (180.0f / DirectX::XM_PI);
    }

    inline Quaternion EulerToQuaternion_ZXY(const Vector3& eulerDeg)
    {
        return Quaternion::CreateFromYawPitchRoll(Deg2Rad(eulerDeg.y), Deg2Rad(eulerDeg.x), Deg2Rad(eulerDeg.z));

        // 유니티도 ZXY 순서임
    }

    inline Vector3 QuaternionToEulerDeg_ZXY(const Quaternion& q)
    {

        const Matrix m = Matrix::CreateFromQuaternion(q);

        const float r12 = m._12;
        const float r22 = m._22;
        const float r31 = m._31;
        const float r32 = m._32;
        const float r33 = m._33;

        float x = std::asin(std::clamp(-r32, -1.0f, 1.0f));
        float y = 0.0f;
        float z = 0.0f;

        if (std::abs(std::cos(x)) > 0.0001f)
        {
            y = std::atan2(r31, r33);
            z = std::atan2(r12, r22);
        }
        else
        {
			// 짐벌락 발생
            y = std::atan2(-m._13, m._11);
            z = 0.0f;
        }

        return Vector3(Rad2Deg(x), Rad2Deg(y), Rad2Deg(z));
    }

    inline Matrix IdentityMatrix() { return Matrix::CreateScale(1.0f); }


}