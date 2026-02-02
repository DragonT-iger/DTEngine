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

BEGINPROPERTY(TilemapGenerator)

DTPROPERTY(TilemapGenerator, m_mapData)

DTPROPERTY(TilemapGenerator, m_prefab0)
DTPROPERTY(TilemapGenerator, m_prefab1)
DTPROPERTY(TilemapGenerator, m_prefab2)
DTPROPERTY(TilemapGenerator, m_prefab3)
DTPROPERTY(TilemapGenerator, m_prefab4)
DTPROPERTY(TilemapGenerator, m_prefab5)
DTPROPERTY(TilemapGenerator, m_prefab6)

ENDPROPERTY()

void TilemapGenerator::Start()
{
	//std::cout << "TilemapGenerator Start 호출됨" << std::endl;
    BuildMap();
}

void TilemapGenerator::BuildMap()
{
    m_spawnedTiles.clear();

    if (m_mapData == nullptr) return;

    int width = m_mapData->GetWidth();
    int height = m_mapData->GetHeight();

    float tileSize = TilemapData::TILE_SIZE;

    Transform* myTr = GetTransform();

    Prefab* palette[] = { m_prefab0, m_prefab1, m_prefab2, m_prefab3, m_prefab4 , m_prefab5 , m_prefab6 };

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = m_mapData->GetTileIndex(x, y);

            if (index < 0 || index >= 7) continue;

            Prefab* prefab = palette[index];
            if (!prefab) continue;

            GameObject* instance = prefab->Instantiate();
            if (!instance) continue;

            instance->GetTransform()->SetParent(myTr);
            instance->GetTransform()->SetPosition(Vector3(x * tileSize, 0, y * tileSize));
            instance->SetActive(true);

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
