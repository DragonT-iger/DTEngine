#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Animator;
class TilemapGenerator;

class BreakableWall : public MonoBehaviour
{
    DTGENERATED_BODY(BreakableWall);

public:
    void Update(float dTime) override;

    void Init(TilemapGenerator* tg, const Vector2& gridPos);
    void StartWallBreakAnim();

private:
    TilemapGenerator* m_tilemapGenerator = nullptr;
    Vector2 m_gridPos{ -1, -1 };

    Animator* m_anim = nullptr;
    bool m_animStart = false;
    bool m_animEnd = false;
};
