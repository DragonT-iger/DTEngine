#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include <vector>

class TilemapData;

class TilemapGenerator : public MonoBehaviour // 나중에 singleton MonoBehaviour로 변경 고려
{
    DTGENERATED_BODY(TilemapGenerator);

public:
    TilemapGenerator() = default;
    virtual ~TilemapGenerator() = default;

    virtual void Start() override;

    void BuildMap();
    void ClearMap();

    void SetTileAt(int x, int y, int paletteIndex);
    void SaveMapData();

    void SetMapData(TilemapData* data) { m_mapData = data; }
    TilemapData* GetMapData() const { return m_mapData; }

	static constexpr int PALETTE_SIZE = 5;

public:
    TilemapData* m_mapData = nullptr;

    GameObject* m_prefab0 = nullptr;
    GameObject* m_prefab1 = nullptr;
    GameObject* m_prefab2 = nullptr;
    GameObject* m_prefab3 = nullptr;
    GameObject* m_prefab4 = nullptr;

private:
    std::vector<GameObject*> m_spawnedTiles;
};