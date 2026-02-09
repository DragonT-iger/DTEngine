#include "pch.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"     
#include "GameObject.h"
#include "Transform.h"
#include "ResourceManager.h"  
#include "Scene.h"
#include "SceneManager.h"
#include "AssetDatabase.h"
#include "Prefab.h"

#include "../Client/EnemyUnit.h"
#include "../Client/BreakableWall.h"

BEGINPROPERTY(TilemapGenerator)

DTPROPERTY(TilemapGenerator, m_mapData)

DTPROPERTY(TilemapGenerator, m_prefab0)
DTPROPERTY(TilemapGenerator, m_prefab1)
DTPROPERTY(TilemapGenerator, m_prefab2)
DTPROPERTY(TilemapGenerator, m_prefab3)
DTPROPERTY(TilemapGenerator, m_prefab4)
DTPROPERTY(TilemapGenerator, m_prefab5)
DTPROPERTY(TilemapGenerator, m_prefab6)
DTPROPERTY(TilemapGenerator, m_prefab7)
DTPROPERTY(TilemapGenerator, m_prefab8)
DTPROPERTY(TilemapGenerator, m_prefab9)
DTPROPERTY(TilemapGenerator, m_prefab10)

DTPROPERTY(TilemapGenerator, m_enemy0)
DTPROPERTY(TilemapGenerator, m_enemy1)
DTPROPERTY(TilemapGenerator, m_enemy2)

DTPROPERTY(TilemapGenerator, m_alice)
DTPROPERTY(TilemapGenerator, m_redQueen)

ENDPROPERTY()

void TilemapGenerator::Start()
{
	//std::cout << "TilemapGenerator Start 호출됨" << std::endl;
    BuildMap();
    SpawnUnits();
}

void TilemapGenerator::BuildMap()
{
    m_spawnedTiles.clear();

    if (m_mapData == nullptr) return;

    int width = m_mapData->GetExpandedWidth();
    int height = m_mapData->GetExpandedHeight();

    float tileSize = TilemapData::TILE_SIZE;

    Transform* myTr = GetTransform();

    Prefab* palette[] = { 
        m_prefab0, m_prefab1, m_prefab2, m_prefab3, m_prefab4, 
        m_prefab5, m_prefab6, m_prefab7, m_prefab8, m_prefab9, m_prefab10 
    };

    m_spawnedTiles.resize(width * height, nullptr);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = m_mapData->GetTileIndex(x, y);

            int arrayIndex = y * width + x;
            
            if (index < 0 || index >= PALETTE_SIZE) continue;

            Prefab* prefab = palette[index];
            if (!prefab) continue;

            GameObject* instance = prefab->Instantiate();
            if (!instance) continue;

            if (index == 5)
            {
                BreakableWall* bw = instance->GetComponent<BreakableWall>();
                if (bw)
                {
                    Vector2 gridPos{ (float)(x - 1), (float)(y - 1) };
                    bw->Init(this, gridPos);
                }
            }

            instance->GetTransform()->SetParent(myTr);
            instance->GetTransform()->SetPosition(Vector3((x - 1) * tileSize, 0, (y - 1) * tileSize));
            instance->SetActive(true);

            m_spawnedTiles[arrayIndex] = instance;
        }
    }
}

void TilemapGenerator::SetTileAt(int x, int y, int paletteIndex)
{
    if (m_mapData)
    {
        // 메모리 상의 데이터 수정
        m_mapData->SetTileIndex(x, y, paletteIndex);
    }
}

void TilemapGenerator::ReplaceTile(int x, int y, Prefab* newPrefab)
{
    if (!m_mapData || !newPrefab) return;

    int width = m_mapData->GetExpandedWidth();
    int height = m_mapData->GetExpandedHeight();

    if (x < 0 || x >= width || y < 0 || y >= height) return;

    int arrayIndex = y * width + x;

    if (arrayIndex >= m_spawnedTiles.size()) return;

    if (m_spawnedTiles[arrayIndex] != nullptr)
    {
        SceneManager::Instance().GetActiveScene()->Destroy(m_spawnedTiles[arrayIndex]);
        m_spawnedTiles[arrayIndex] = nullptr;
    }

    GameObject* instance = newPrefab->Instantiate();
    if (instance)
    {
        Transform* myTr = GetTransform();
        float tileSize = TilemapData::TILE_SIZE;

        instance->GetTransform()->SetParent(myTr);
        instance->GetTransform()->SetPosition(Vector3((x - 1) * tileSize, 0, (y - 1) * tileSize));
        instance->SetActive(true);

        m_spawnedTiles[arrayIndex] = instance;
    }
}

void TilemapGenerator::RebuildFromCurrentData()
{
    Scene* activeScene = SceneManager::Instance().GetActiveScene();

    // 기존 타일 오브젝트 제거
    for (GameObject* tile : m_spawnedTiles)
    {
        if (tile && activeScene)
        {
            activeScene->Destroy(tile);
        }
    }
    m_spawnedTiles.clear();

    // 기존 적 오브젝트 제거
    for (GameObject* enemy : m_spawnedEnemys)
    {
        if (enemy && activeScene)
        {
            activeScene->Destroy(enemy);
        }
    }
    m_spawnedEnemys.clear();

    // 새 데이터 기준 재생성
    BuildMap();
    SpawnUnits();
}

GameObject* TilemapGenerator::GetSpawnedTileAtGrid(const Vector2& gridPos) const
{
    if (!m_mapData) return nullptr;

    int ex = (int)gridPos.x + 1;
    int ey = (int)gridPos.y + 1;

    int ew = m_mapData->GetExpandedWidth();
    int eh = m_mapData->GetExpandedHeight();
    if (ex < 0 || ex >= ew || ey < 0 || ey >= eh) return nullptr;

    int idx = ey * ew + ex;
    if (idx < 0 || idx >= (int)m_spawnedTiles.size()) return nullptr;

    return m_spawnedTiles[idx];
}

void TilemapGenerator::ReplaceTileAtGrid(const Vector2& gridPos)
{
    if (!m_mapData) return;

    int ex = (int)gridPos.x + 1;
    int ey = (int)gridPos.y + 1;

    int ew = m_mapData->GetExpandedWidth();
    int eh = m_mapData->GetExpandedHeight();
    if (ex < 0 || ex >= ew || ey < 0 || ey >= eh) return;

    int tileIdx = m_mapData->FindDefaultGrid(ex, ey);

    Prefab* prefab = nullptr;
    if (tileIdx == 0) prefab = m_prefab0;
    else if (tileIdx == 1) prefab = m_prefab1;

    if (!prefab) return;

    ReplaceTile(ex, ey, prefab);
}

void TilemapGenerator::SpawnUnits()
{
    m_spawnedEnemys.clear();
    m_spawnedAlice = nullptr;
    m_spawnedRedQueen = nullptr;

    if (m_mapData == nullptr) return;

    Prefab* enemys[] = { m_enemy0, m_enemy1, m_enemy2 }; // 룩 나 비

    m_spawnedEnemys.resize(NUM_ENEMIES, nullptr);
    for (int i = 0; i < NUM_ENEMIES; ++i)
    {
        const auto& info = m_mapData->GetEnemy(i);

        if (!info.enabled) continue;

        Vector2 pos = info.pathPoints[0];
        if (pos.x < 0 || pos.y < 0) continue;

        Prefab* prefab = enemys[info.type];
        if (!prefab) continue;

        GameObject* instance = prefab->Instantiate();
        if (!instance) continue;

        EnemyUnit* enemy = instance->GetComponent<EnemyUnit>();
        if (!enemy) continue;

        instance->GetTransform()->SetPosition(Vector3{ pos.x * 2.0f, 1.0f, pos.y * 2.0f });
        instance->SetActive(true);
        
        enemy->SetBoss(info.isBoss);
        enemy->SetUnitType(info.type);
        enemy->SetPath(info.pathPoints);

        m_spawnedEnemys[i] = instance;

    }

    // 앨리스
    {
        Vector2 pos = m_mapData->GetAlicePos();
        if (pos.x >= 0 && pos.y >= 0 && m_alice)
        {
            GameObject* instance = m_alice->Instantiate();
            if (instance)
            {
                instance->GetTransform()->SetPosition(Vector3{ pos.x * 2.0f, 1.0f, pos.y * 2.0f });
                instance->SetActive(true);

                m_spawnedAlice = instance;
            }
        }
    }

    // 붉은여왕
    {
        const auto& rq = m_mapData->GetRedQueen();

        if (rq.enabled && rq.pos.x >= 0 && rq.pos.y >= 0 && m_redQueen)
        {
            GameObject* instance = m_redQueen->Instantiate();
            if (instance)
            {
                instance->GetTransform()->SetPosition(Vector3{ rq.pos.x * 2.0f, 1.0f, rq.pos.y * 2.0f });
                instance->SetActive(true);

                m_spawnedRedQueen = instance;
            }
        }
    }
}
