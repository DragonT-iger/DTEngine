#pragma once

#include <SimpleMath.h>
#include <algorithm>



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
        return Quaternion::CreateFromYawPitchRoll(Deg2Rad(eulerDeg.y), Deg2Rad(eulerDeg.x), Deg2Rad(eulerDeg.z));
    }

    inline Vector3 QuaternionToEulerDeg_ZXY(const Quaternion& q)
    {

        const Matrix m = Matrix::CreateFromQuaternion(q);

        // ZXY(= Rz(z)*Rx(x)*Ry(y)) 행렬의 r32 요소가 sin(x)가 됨
        const float r11 = m._11, r12 = m._12, r13 = m._13;
        const float r21 = m._21, r22 = m._22, r23 = m._23;
        const float r31 = m._31, r32 = m._32, r33 = m._33;

        // x = asin(r32)
        float sx = std::clamp(r32, -1.0f, 1.0f); // NaN 방지
        float x = std::asin(sx);

        float y, z;

        // 짐벌락 보정: |cos(x)| ~= 0 (x가 ±90도일 때)
        const float cx = std::cos(x);
        if (std::abs(cx) < 1e-6f)
        {
            // (y, z 중 하나를 0으로 고정)
            if (sx > 0.0f) { // x = +90도
                y = std::atan2(r13, r11);
                z = 0.0f;
            }
            else { // x = -90도
                y = std::atan2(-r13, r11);
                z = 0.0f;
            }
        }
        else // 짐벌락 아님
        {
            // y = atan2(-r31, r33)
            y = std::atan2(-r31, r33);
            // z = atan2(-r12, r22)
            z = std::atan2(-r12, r22);
        }

        return Vector3(Rad2Deg(x), Rad2Deg(y), Rad2Deg(z));
    }

    inline Matrix IdentityMatrix() { return Matrix::CreateScale(1.0f); }


}