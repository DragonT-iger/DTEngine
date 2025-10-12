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
        const float x = Deg2Rad(eulerDeg.x);
        const float y = Deg2Rad(eulerDeg.y);
        const float z = Deg2Rad(eulerDeg.z);

        Matrix m = Matrix::CreateRotationZ(z) * Matrix::CreateRotationX(x) * Matrix::CreateRotationY(y);
        return Quaternion::CreateFromRotationMatrix(m);
    }

    inline Vector3 QuaternionToEulerDeg_ZXY(const Quaternion& q)
    {
        //const Matrix m = Matrix::CreateFromQuaternion(q);

        //// ZXY(= Rz(z)*Rx(x)*Ry(y)) 전개 결과에서 나온 요소 사용
        //// R =
        //// [ -sx*sy*sz + cy*cz ,  -sz*cx ,  sx*sz*cy + sy*cz ]
        //// [  sx*sy*cz + sz*cy ,   cx*cz , -sx*cy*cz + sy*sz ]
        //// [        -sy*cx     ,    sx    ,      cx*cy       ]
        //const float r11 = m._11, r12 = m._12, r13 = m._13;
        //const float r21 = m._21, r22 = m._22, r23 = m._23;
        //const float r31 = m._31, r32 = m._32, r33 = m._33;

        //// 기본 해(짐벌락 아님):
        ////x = asin(r32)
        ////y = atan2(-r31, r33)
        ////z = atan2(-r12, r22)
        //float sx = std::clamp(r32, -1.0f, 1.0f);
        //float x = std::asin(sx);

        //float y, z;

        //// 짐벌락 보정: |cos(x)| ~= 0 (x ≈ ±90°)
        //const float cx = std::cos(x);
        //if (std::abs(cx) < 1e-6f)
        //{
        //    // x = +90°(sx=+1) 이면:  y+z = atan2(r13, r11)
        //    // x = -90°(sx=-1) 이면:  y-z = atan2(-r13, r11)
        //    if (sx > 0.0f) {
        //        y = std::atan2(r13, r11);
        //        z = 0.0f;
        //    } else {
        //        y = std::atan2(-r13, r11);
        //        z = 0.0f;
        //    }
        //}
        //else
        //{
        //    y = std::atan2(-r31, r33);
        //    z = std::atan2(-r12, r22);
        //}

        //return Vector3(Rad2Deg(x), Rad2Deg(y), Rad2Deg(z));

        const Matrix m = Matrix::CreateFromQuaternion(q);

        const float r31 = m._31, r32 = m._32, r33 = m._33;
        const float r12 = m._12, r22 = m._22;

        // 기본 해(보정 없음): x = asin(r32), y = atan2(-r31, r33), z = atan2(-r12, r22)
        const float sx = std::clamp(r32, -1.0f, 1.0f);
        const float x = std::asin(sx);
        const float y = std::atan2(-r31, r33);
        const float z = std::atan2(-r12, r22);

        return Vector3(Rad2Deg(x), Rad2Deg(y), Rad2Deg(z));
    }

    inline Matrix IdentityMatrix() { return Matrix::CreateScale(1.0f); }


}