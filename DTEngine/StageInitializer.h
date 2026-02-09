#pragma once
#include "MonoBehaviour.h"

class TilemapData;
class TilemapGenerator;
class BattleGrid;
class GameObject;
class Text;

class StageInitializer : public MonoBehaviour
{
    DTGENERATED_BODY(StageInitializer)

public:
    void Awake() override;

private:
    TilemapData* m_mapData1 = nullptr;
    TilemapData* m_mapData2 = nullptr;
    TilemapData* m_mapData3 = nullptr;
    TilemapData* m_mapData4 = nullptr;
    TilemapData* m_mapData5 = nullptr;
    TilemapData* m_mapData6 = nullptr;

    TilemapGenerator* m_generator = nullptr;

    Text* m_stageLevelText = nullptr;
    Text* m_stageDialogueText = nullptr;
};