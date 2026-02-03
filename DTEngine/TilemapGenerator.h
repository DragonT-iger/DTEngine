#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include <vector>

class TilemapData;
class Prefab;

class TilemapGenerator : public MonoBehaviour // 나중에 singleton MonoBehaviour로 변경 고려
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

	static constexpr int PALETTE_SIZE = 8;

public:

    Prefab* m_prefab0 = nullptr; 
    Prefab* m_prefab1 = nullptr;
    Prefab* m_prefab2 = nullptr;
    Prefab* m_prefab3 = nullptr;
    Prefab* m_prefab4 = nullptr;
    Prefab* m_prefab5 = nullptr;
    Prefab* m_prefab6 = nullptr;
    Prefab* m_prefab7 = nullptr;

private:
    
    TilemapData* m_mapData = nullptr;

    std::vector<GameObject*> m_spawnedTiles;
};