#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <iostream>
#include <iomanip>

#include "BattleGrid.h"
#include "CombatController.h"
#include "AllyUnit.h"
#include "EnemyUnit.h"
#include "AliceUnit.h"

class CombatTestRunner : public MonoBehaviour
{
    DTGENERATED_BODY(CombatTestRunner);

public:
    void Start() override
    {
        SetupScenario();
    }

    void Update(float dTime) override
    {
        m_frame++;
        PrintFrame();

        // 전투 진행
        if (m_controller)
            m_controller->Update(dTime);

        // 결과 나면 멈추고 싶으면 여기서 return 처리
        // (stageResult getter 없으면, 보스/앨리스 IsAlive로 임시 판단 가능)
        if (m_stopWhenBossDies && m_enemy2 && !m_enemy2->IsAlive())
            return;
        if (m_stopWhenAliceDies && m_alice && !m_alice->IsAlive())
            return;
    }


    void SetBattleGrid(BattleGrid* g) { m_grid = g; }
    void SetCombatController(CombatController* c) { m_controller = c; }

    void SetAllyUnit0(AllyUnit* u) { m_ally0 = u; }
    void SetAllyUnit1(AllyUnit* u) { m_ally1 = u; }
    void SetAllyUnit2(AllyUnit* u) { m_ally2 = u; }

    void SetEnemyUnit0(EnemyUnit* u) { m_enemy0 = u; }
    void SetEnemyUnit1(EnemyUnit* u) { m_enemy1 = u; }
    void SetEnemyUnit2(EnemyUnit* u) { m_enemy2 = u; }

    void SetAliceUnit(AliceUnit* u) { m_alice = u; }


private:
    // ====== 너가 원한 (1) 초기 세팅 하나 ======
    void SetupScenario()
    {
        if (!m_grid || !m_controller || !m_alice) return;

        // 1) 임의 타일맵 (10x10 전부 타일)
        const int W = 10, H = 10;
        m_grid->Initialize(W, H);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                m_grid->SetTile(Vector2{ (float)x, (float)y });

        // 2) 임의 장애물
        m_grid->SetSolidWall(Vector2{ 4,4 });
        m_grid->SetSolidWall(Vector2{ 4,5 });
        m_grid->SetSolidWall(Vector2{ 4,6 });

        m_grid->SetBreakableWall(Vector2{ 6,3 });
        m_grid->SetBreakableWall(Vector2{ 6,4 });

        m_grid->SetDefenseTile(Vector2{ 1,1 });
        m_grid->SetDefenseTile(Vector2{ 2,1 });
        m_grid->SetDefenseTile(Vector2{ 1,2 });

        // 3) 유닛 세팅 (씬에 배치된 컴포넌트 포인터를 받아온다고 가정)
        if (m_ally0)
        {
            m_ally0->SetPos(Vector2{ 1,8 });
        }
        if (m_ally1)
        {
            m_ally1->SetPos(Vector2{ 2,8 });
        }
        if (m_ally2)
        {
            m_ally2->SetPos(Vector2{ 3,8 });
        }

        if (m_enemy0)
        {
            m_enemy0->SetPos(Vector2{ 8,2 });
            m_enemy0->SetUnitType(UnitType::Rook);
            m_enemy0->SetBoss(false);
        }
        if (m_enemy1)
        {
            m_enemy1->SetPos(Vector2{ 8,3 });
            m_enemy1->SetUnitType(UnitType::Knight);
            m_enemy1->SetBoss(false);
        }
        if (m_enemy2)
        {
            m_enemy2->SetPos(Vector2{ 8,4 });
            m_enemy2->SetUnitType(UnitType::Bishop);
            m_enemy2->SetBoss(true);
        }

        m_alice->SetPos(Vector2{ 1,5 });

        // 4) 컨트롤러에 주입 + setup
        m_controller->SetBattleGrid(m_grid);
        m_controller->SetAliceUnit(m_alice);

        m_controller->SetAllyUnit0(m_ally0);
        m_controller->SetAllyUnit1(m_ally1);
        m_controller->SetAllyUnit2(m_ally2);

        m_controller->SetEnemyUnit0(m_enemy0);
        m_controller->SetEnemyUnit1(m_enemy1);
        m_controller->SetEnemyUnit2(m_enemy2);

        m_controller->Setup();

        // 5) 동기화
        std::vector<AllyUnit*> allies{ m_ally0, m_ally1, m_ally2 };
        std::vector<EnemyUnit*> enemies{ m_enemy0, m_enemy1, m_enemy2 };

        m_grid->ClearDynamicGrid();
        m_grid->SyncUnitsPos(allies, enemies, m_alice);
    }

    // ====== 너가 원한 (2) 매 루프 출력 하나 ======
    void PrintFrame()
    {
        if (!m_grid) return;

        auto isSame = [](const Vector2& a, const Vector2& b) {
            return (int)a.x == (int)b.x && (int)a.y == (int)b.y;
            };

        std::vector<AllyUnit*> allies{ m_ally0, m_ally1, m_ally2 };
        std::vector<EnemyUnit*> enemies{ m_enemy0, m_enemy1, m_enemy2 };

        std::cout << "\n---- frame " << m_frame << " ----\n";

        // 10x10 가정(SetupScenario랑 맞춤)
        for (int y = 0; y < 10; ++y)
        {
            for (int x = 0; x < 10; ++x)
            {
                Vector2 p{ (float)x, (float)y };
                const auto& s = m_grid->GetStaticTile(p);

                char c = ' ';
                if (!s.tile) c = ' ';
                else if (s.solidWall) c = '#';
                else if (s.breakableWall) c = '*';
                else if (s.defenseTile) c = 'D';
                else c = '.';

                if (m_alice && m_alice->IsAlive() && isSame(m_alice->GetPos(), p)) c = 'S';

                for (auto* a : allies)
                    if (a && a->IsAlive() && isSame(a->GetPos(), p)) c = 'A';

                for (auto* e : enemies)
                    if (e && e->IsAlive() && isSame(e->GetPos(), p)) c = (e->IsBoss() ? 'B' : 'E');

                std::cout << c << ' ';
            }
            std::cout << "\n";
        }

        auto printUnit = [](const char* name, const Unit* u)
            {
                if (!u) { std::cout << name << ": (null)\n"; return; }
                std::cout << name
                    << " pos(" << (int)u->GetPos().x << "," << (int)u->GetPos().y << ")"
                    << " hp=" << std::fixed << std::setprecision(1) << u->GetHp()
                    << " alive=" << (u->IsAlive() ? 1 : 0)
                    << " act=" << (int)u->GetAction()
                    << " done=" << (u->IsActionDone() ? 1 : 0)
                    << " move(" << (int)u->GetMovePos().x << "," << (int)u->GetMovePos().y << ")"
                    << " movetarget(" << (int)u->GetMoveTargetPos().x << "," << (int)u->GetMoveTargetPos().y << ")"
                    << " atkPos(" << (int)u->GetAttackPos().x << "," << (int)u->GetAttackPos().y << ")"
                    << "\n";
            };

        //if (m_alice) printUnit("Alice", m_alice);
        if (m_ally0) printUnit("Ally0", m_ally0);
        if (m_ally1) printUnit("Ally1", m_ally1);
        if (m_ally2) printUnit("Ally2", m_ally2);
        if (m_enemy0) printUnit("Enemy0", m_enemy0);
        if (m_enemy1) printUnit("Enemy1", m_enemy1);
        if (m_enemy2) printUnit("Enemy2", m_enemy2);
    }

private:
    int m_frame = 0;

    // 씬에서 주입(에디터 프로퍼티로 꽂기)
    BattleGrid* m_grid = nullptr;
    CombatController* m_controller = nullptr;

    AllyUnit* m_ally0 = nullptr;
    AllyUnit* m_ally1 = nullptr;
    AllyUnit* m_ally2 = nullptr;

    EnemyUnit* m_enemy0 = nullptr;
    EnemyUnit* m_enemy1 = nullptr;
    EnemyUnit* m_enemy2 = nullptr;

    AliceUnit* m_alice = nullptr;

    bool m_stopWhenBossDies = true;
    bool m_stopWhenAliceDies = true;
};

