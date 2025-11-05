#pragma once

#include <SimpleMath.h>
#include <algorithm>


inline std::ostream& operator<<(std::ostream& os, const DirectX::SimpleMath::Matrix& m)
{
    os  << "[" << m._11 << ", " << m._12 << ", " << m._13 << ", " << m._14 << "]\n"
        << "[" << m._21 << ", " << m._22 << ", " << m._23 << ", " << m._24 << "]\n"
        << "[" << m._31 << ", " << m._32 << ", " << m._33 << ", " << m._34 << "]\n"
        << "[" << m._41 << ", " << m._42 << ", " << m._43 << ", " << m._44 << "]\n";
    return os;
}


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

    inline Quaternion EulerToQuaternion_ZXY(const Vector3& eulerDeg)
    {
        return Quaternion::CreateFromYawPitchRoll(-Deg2Rad(eulerDeg.y), Deg2Rad(eulerDeg.x), -Deg2Rad(eulerDeg.z));

        // 왠진 모르겠지만 왼손기준으로 안돔 이렇게 안하면
    }

    inline Vector3 QuaternionToEulerDeg_ZXY(const Quaternion& q)
    {

        const Matrix m = Matrix::CreateFromQuaternion(q);

        const float r11 = m._11, r12 = m._12, r13 = m._13;
        const float r21 = m._21, r22 = m._22, r23 = m._23;
        const float r31 = m._31, r32 = m._32, r33 = m._33;

        float sx = std::clamp(r32, -1.0f, 1.0f); 
        float x = std::asin(sx);

        float y, z;

        const float cx = std::cos(x);
        if (std::abs(cx) < 1e-6f)
        {
            if (sx > 0.0f) { 
                y = std::atan2(r13, r11);
                z = 0.0f;
            }
            else { 
                y = std::atan2(-r13, r11);
                z = 0.0f;
            }
        }
        else 
        {
            y = std::atan2(-r31, r33);
            z = std::atan2(-r12, r22);
        }

        return Vector3(Rad2Deg(x), Rad2Deg(y), Rad2Deg(z));
    }

    inline Matrix IdentityMatrix() { return Matrix::CreateScale(1.0f); }


}