#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

#include "ColorToggler.h"


class Rotator : public MonoBehaviour
{
    DTGENERATED_BODY(Rotator);

public:
    Rotator() = default;
    virtual ~Rotator() = default;


    //void Start() override;
    void Update(float deltaTime) override;

    void SetRotationSpeed(const Vector3& speed) { m_rotationSpeed = speed; }

private:
    Vector3 m_rotationSpeed = Vector3(0.0f, 20.0f, 0.0f);
};