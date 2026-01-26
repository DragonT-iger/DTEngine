#pragma once
#include "MonoBehaviour.h"

class UIButton;
class GameObject;

class OwnerSetActive : public MonoBehaviour
{
    DTGENERATED_BODY(OwnerSetActive);

public:
    OwnerSetActive() = default;
    ~OwnerSetActive() override = default;

    void Awake() override;

    GameObject*& GetTarget() { return m_target; }
    void SetTarget(GameObject*& target) { m_target = target; }

    void ToggleTarget();
private:

    GameObject* m_target = nullptr;
};

