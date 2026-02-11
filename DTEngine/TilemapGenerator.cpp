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

#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
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

DTPROPERTY(TilemapGenerator, m_bgWall0)
DTPROPERTY(TilemapGenerator, m_bgWall1)

DTPROPERTY(TilemapGenerator, m_bgStain)

DTPROPERTY(TilemapGenerator, m_alice)
DTPROPERTY(TilemapGenerator, m_redQueen)

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
    m_spawnedBgWall == nullptr;
    m_spawnedBgStain0 = nullptr;
    m_spawnedBgStain1 = nullptr;

    if (m_mapData == nullptr) return;

    int width = m_mapData->GetExpandedWidth();
    int height = m_mapData->GetExpandedHeight();
    float tileSize = TilemapData::TILE_SIZE;

    Transform* myTr = GetTransform();
    if (myTr == nullptr) return;

    if (width == 10 && height == 14 && m_bgWall0)
    {
        GameObject* instance = m_bgWall0->Instantiate();
        GameObject* instance2 = m_bgStain->Instantiate();
        if (instance && instance2)
        {
            Transform* tr = instance->GetTransform();
            if (tr)
            {
                tr->SetParent(myTr);
                tr->SetPosition(Vector3(7.0f, 0.0f, 11.0f));
                instance->SetActive(true);

                m_spawnedBgWall = instance;
            }

            Transform* tr2 = instance2->GetTransform();
            if (tr2)
            {
                tr2->SetParent(myTr);
                tr2->SetPosition(Vector3(7.0f, 0.52f, 11.0f));
                instance2->SetActive(true);

                m_spawnedBgStain0 = instance2;
            }
        }
    }
    else if (width == 10 && height == 26 && m_bgWall1)
    {
        GameObject* instance = m_bgWall1->Instantiate();
        GameObject* instance2 = m_bgStain->Instantiate();
        GameObject* instance3 = m_bgStain->Instantiate();
        if (instance && instance2 && instance3)
        {
            Transform* tr = instance->GetTransform();
            if (tr)
            {
                tr->SetParent(myTr);
                tr->SetPosition(Vector3(7.0f, 0.0f, 22.0f));
                instance->SetActive(true);

                m_spawnedBgWall = instance;
            }

            Transform* tr2 = instance2->GetTransform();
            if (tr2)
            {
                tr2->SetParent(myTr);
                tr2->SetPosition(Vector3(7.0f, 0.52f, 11.0f));
                instance2->SetActive(true);

                m_spawnedBgStain0 = instance2;
            }

            Transform* tr3 = instance3->GetTransform();
            if (tr3)
            {
                tr3->SetParent(myTr);
                tr3->SetPosition(Vector3(7.0f, 0.52f, 35.0f));
                instance3->SetActive(true);

                m_spawnedBgStain1 = instance3;
            }
        }
    }

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

            // 8, 9, 10 필요없어졌는데.. 모든곳에서 다 지우기 귀찮으니까... 일단 걍 생성만 안하게 막아두겠음..
            if (index == 8 || index == 9 || index == 10) continue;

            int arrayIndex = y * width + x;

            if (index < 0 || index >= PALETTE_SIZE) continue;

            Prefab* prefab = palette[index];
            if (prefab == nullptr) continue;

            GameObject* instance = prefab->Instantiate();
            if (instance == nullptr) continue;

            Transform* tr = instance->GetTransform();
            if (tr == nullptr) continue;

            Vector3 position((x - 1) * tileSize, 0.0f, (y - 1) * tileSize);

            Vector3 scale(1.0f, 1.0f, 1.0f);

            float pitch = 0.0f;
            float yaw = 0.0f;
            float roll = 0.0f;


            if (index == 0 || index == 1 || index == 2 || index == 3 || index == 4 || index == 6 || index == 7)
            {
                pitch = 90.0f;
            }

            if (index == 5)
            {
                BreakableWall* bw = instance->GetComponent<BreakableWall>();
                if (bw)
                {
                    Vector2 gridPos((float)(x - 1), (float)(y - 1));
                    bw->Init(this, gridPos);
                }
            }

            if (index == 8 || index == 9)
            {
                scale = Vector3(1.0f, 1.0f, 1.0f);

                if      (x == 0)          yaw = 90.0f;
                else if (x == width - 1)  yaw = -90.0f;
                else if (y == 0)          yaw = 0.0f;
                else if (y == height - 1) yaw = 180.0f;

                if      (x == 0)          position.x += 0.3f;
                else if (x == width - 1)  position.x -= 0.3f;
                else if (y == 0)          position.z += 0.3f;
                else if (y == height - 1) position.z -= 0.3f;
            }
            else if (index == 10)
            {
                if      (x == 0 && y == 0)                  yaw = 90.0f;
                else if (x == width - 1 && y == 0)          yaw = 0.0f;
                else if (x == 0 && y == height - 1)         yaw = 180.0f;
                else if (x == width - 1 && y == height - 1) yaw = -90.0f;
            }

            if (index == 6 || index == 7)
            {
                position.y = -1.0f;
            }

            tr->SetScale(scale);
            tr->SetRotationEuler(Vector3(pitch, yaw, roll));
            tr->SetPosition(position);
            tr->SetParent(myTr);

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

bool StartsWith(const std::string& fullString, const std::string& prefix)
{
    if (fullString.length() < prefix.length()) return false;
    return fullString.rfind(prefix, 0) == 0;
}

void TilemapGenerator::ChangeMark(GameObject* obj)
{
    for (Transform* child : _GetOwner()->GetTransform()->GetChildren())
    {
        if (child->GetName().rfind("Mark", 0) == 0)
        {
            auto meshRenderer = child->GetComponent<MeshRenderer>();

            if (meshRenderer)
            {
                int m_type = obj->GetComponent<Unit>()->GetUnitType();

                if (m_type == UnitType::Bishop)
                {
                    std::string path = "Assets/Models/UI/Alice_UI/HP_Bishop_boss.png";
                    Texture* texture = ResourceManager::Instance().Load<Texture>(path);
                    meshRenderer->GetSharedMaterial()->SetTexture(0, texture);
                }

                else if (m_type == UnitType::Knight)
                {
                    std::string path = "Assets/Models/UI/Alice_UI/HP_Knight_boss.png";
                    Texture* texture = ResourceManager::Instance().Load<Texture>(path);
                    meshRenderer->GetSharedMaterial()->SetTexture(0, texture);
                }


                else if (m_type == UnitType::Rook)
                {
                    std::string path = "Assets/Models/UI/Alice_UI/HP_Rook_boss.png";
                    Texture* texture = ResourceManager::Instance().Load<Texture>(path);
                    meshRenderer->GetSharedMaterial()->SetTexture(0, texture);
                }


            }

            break;
        }
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


        float yOffset = 0;
        if (instance->GetName() == "Defense") {
            yOffset = -1;
        }

        instance->GetTransform()->SetParent(myTr);
        instance->GetTransform()->SetPosition(Vector3((x - 1) * tileSize, yOffset, (y - 1) * tileSize));
        instance->SetActive(true);

        m_spawnedTiles[arrayIndex] = instance;
    }
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

    int firstTileIdx = m_mapData->GetTileIndex(1, 1);

    Prefab* prefab = nullptr;

    if (firstTileIdx == 1 || firstTileIdx == 3)
    {
        if (tileIdx == 0) prefab = m_prefab1;
        else if (tileIdx == 1) prefab = m_prefab0;
    }
    else 
    {
        if (tileIdx == 0) prefab = m_prefab0;
        else if (tileIdx == 1) prefab = m_prefab1;
    }
    


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
        instance->GetTransform()->SetParent(GetTransform());
        
        enemy->SetBoss(info.isBoss);

       // if (enemy->IsBoss()) ChangeMark(instance);

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
                instance->GetTransform()->SetParent(GetTransform());

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
