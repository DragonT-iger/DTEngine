#pragma once
#include "MonoBehaviour.h"
#include <functional>

class GameObject;
class UIButton;
class Image;
class AllyUnit;
class Unit;
class Texture;

class RuleSelectWindow : public MonoBehaviour
{
    DTGENERATED_BODY(RuleSelectWindow);
public:
    // 결과로 이동 규칙과 공격 규칙 두 개를 넘겨줌
    //using RuleCallback = std::function<void(MoveRule moverule, BattleRule batterule)>;

    RuleSelectWindow() = default;
    ~RuleSelectWindow() override = default;

    void Awake() override { CreateOnce(); }
    void CreateOnce();
    void InitializeWindow();

    // 타겟 유닛 설정 및 창 열기.
    //void Open(GameObject* target, UnitType type);
    void Close();

    int GetMoveIndex() const { return m_moveIndex; }
    int GetAttackIndex() const { return m_attackIndex; }
    GameObject* SetModel(GameObject* model) { m_model = model; }
   
    // 버튼 클릭 시 인덱스 업데이트 
    void SetMoveIndex(int idx) { m_moveIndex = idx; UpdateButtonVisuals(); }
    void SetAttackIndex(int idx) { m_attackIndex = idx; UpdateButtonVisuals(); }

    void UpdateButtonVisuals();
private:
    GameObject* m_Window = nullptr;
    Image* m_titleBarImg = nullptr;

    Texture* m_rookTex = nullptr;
    Texture* m_knightTex = nullptr;
    Texture* m_bishopTex = nullptr;

    GameObject* m_model = nullptr; // model 생성 시 최초로 호출 할 떄 받아야 하는거.

    // 현재 선택된 인덱스 0: 선택안함, 1~3: 규칙 타입
    int m_moveIndex = 0;
    int m_attackIndex = 0;
};