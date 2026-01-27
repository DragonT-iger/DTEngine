#pragma once
#include "MonoBehaviour.h"

class UIButton;
class Prefab;
class TilemapGenerator;

enum MoveRule;
enum BattleRule;

class SelectOkayEvent : public MonoBehaviour
{
    DTGENERATED_BODY(SelectOkayEvent);

public:
    void Start() override;

    void SetSelectIndex(int val);
    const int& GetSelectIndex() const { return m_selectIndex; }
private:
    UIButton* m_button = nullptr;
    int m_selectIndex = 0;

    // tilemapdata 받아오기 위해서.
    TilemapGenerator* m_mapData = nullptr;

    // 걍 null 검사만 하고 그러면 window 켜주고 자식오브젝트에 titlebar 이미지 수정까지.
    GameObject* m_ruleWindow = nullptr; 
};


