#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include <vector>

#include "../Client/AliceUnit.h"
#include "../Client/RedQueen.h"

class TilemapData;
class Prefab;

class TilemapGenerator : public MonoBehaviour 
{
    DTGENERATED_BODY(TilemapGenerator);

public:
    TilemapGenerator() = default;
    virtual ~TilemapGenerator() = default;

    void Start() override;

    void BuildMap();

    void SetTileAt(int x, int y, int paletteIndex);

    void SetMapData(TilemapData* data) { m_mapData = data; }
    TilemapData* GetMapData() const { return m_mapData; }

    void ReplaceTile(int x, int y, Prefab* newPrefab);

    void SpawnUnits(); // 적 생성
    //void RebuildFromCurrentData(); // 재생성시 호출.

    std::vector<GameObject*>& GetSpawnedEnemys() { return m_spawnedEnemys; }
    GameObject* GetSpawnedAlice() const { return m_spawnedAlice; }
    GameObject* GetSpawnedRedQueen() const { return m_spawnedRedQueen; }

    GameObject* GetSpawnedTileAtGrid(const Vector2& gridPos) const;
    void ReplaceTileAtGrid(const Vector2& gridPos);

	static constexpr int PALETTE_SIZE = 11;
    static constexpr int NUM_ENEMIES = 3;

public:

    Prefab* m_prefab0 = nullptr; 
    Prefab* m_prefab1 = nullptr;
    Prefab* m_prefab2 = nullptr;
    Prefab* m_prefab3 = nullptr;
    Prefab* m_prefab4 = nullptr;
    Prefab* m_prefab5 = nullptr;
    Prefab* m_prefab6 = nullptr;
    Prefab* m_prefab7 = nullptr;
    Prefab* m_prefab8 = nullptr;
    Prefab* m_prefab9 = nullptr;
    Prefab* m_prefab10 = nullptr;

    Prefab* m_bgWall0 = nullptr;
    Prefab* m_bgWall1 = nullptr;

    Prefab* m_enemy0 = nullptr;
    Prefab* m_enemy1 = nullptr;
    Prefab* m_enemy2 = nullptr;

    Prefab* m_alice = nullptr;
    Prefab* m_redQueen = nullptr;

private:
    
    TilemapData* m_mapData = nullptr;

    GameObject* m_spawnedBgWall = nullptr;
    std::vector<GameObject*> m_spawnedTiles;
    std::vector<GameObject*> m_spawnedEnemys;

    GameObject* m_spawnedAlice = nullptr;
    GameObject* m_spawnedRedQueen = nullptr;
};