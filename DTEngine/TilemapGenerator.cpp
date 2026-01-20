#include "pch.h"
#include "TilemapGenerator.h"
#include "TilemapData.h"     
#include "GameObject.h"
#include "Transform.h"
#include "ResourceManager.h"  
#include "Scene.h"
#include "SceneManager.h"

BEGINPROPERTY(TilemapGenerator)

DTPROPERTY(TilemapGenerator, m_mapData)

DTPROPERTY(TilemapGenerator, m_prefab0)
DTPROPERTY(TilemapGenerator, m_prefab1)
DTPROPERTY(TilemapGenerator, m_prefab2)
DTPROPERTY(TilemapGenerator, m_prefab3)
DTPROPERTY(TilemapGenerator, m_prefab4)

ENDPROPERTY()

void TilemapGenerator::Start()
{
    BuildMap();
}

void TilemapGenerator::BuildMap()
{
    ClearMap();

    if (m_mapData == nullptr) return;

    int width = m_mapData->GetWidth();
    int height = m_mapData->GetHeight();

    float tileSize = TilemapData::TILE_SIZE;

    Transform* myTr = GetTransform();

    GameObject* palette[] = { m_prefab0, m_prefab1, m_prefab2, m_prefab3, m_prefab4 };

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = m_mapData->GetTileIndex(x, y);

            if (index < 0 || index >= 5) continue;

            GameObject* prefab = palette[index];
            if (!prefab) continue;


			Scene* curScene = SceneManager::Instance().GetActiveScene();



            auto clones = prefab->Clone();
            if (clones.empty()) continue;

            GameObject* instance = clones[0].get();

            for (auto& go : clones) {
                curScene->AddGameObject(std::move(go));
            }

            if (instance)
            {
                Transform* instanceTr = instance->GetTransform();
                if (instanceTr)
                {
                    instanceTr->SetParent(myTr);

                    instanceTr->SetPosition(Vector3(x * tileSize, 0, y * tileSize));
					instanceTr->_GetOwner()->SetActive(true);
                }
            }
        }
    }
}

void TilemapGenerator::ClearMap()
{
    Transform* myTr = GetTransform();

    // 자식 오브젝트들을 모두 파괴하는 로직 필요
    // 예시: 자식을 순회하며 DestroyGameObjectCommand 등을 호출하거나
    // 엔진 API에 따라 처리 (여기서는 생략)
}

void TilemapGenerator::SetTileAt(int x, int y, int paletteIndex)
{
    if (m_mapData)
    {
        // 메모리 상의 데이터 수정
        m_mapData->SetTileIndex(x, y, paletteIndex);
    }
}

void TilemapGenerator::SaveMapData()
{
    if (m_mapData)
    {
        // m_mapData는 IResource이므로 자신의 파일 경로(메타정보 등)를 알고 있다고 가정
        // 혹은 AssetDatabase를 통해 역추적해야 할 수도 있음.

        // 간단히 경로를 알고 있다면:
        // m_mapData->SaveFile(AssetDatabase::Instance().GetPathFromID(m_mapData->GetMeta().guid));

        // 현재 IResource 인터페이스에는 GetPath가 없으므로
        // AssetDatabase::Instance().GetPathFromID(m_mapData->GetMeta().guid) 등을 활용해야 합니다.
    }
}