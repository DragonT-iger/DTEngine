#pragma once
#include "MonoBehaviour.h"
#include <functional>

class PrefabSelectWindow;
//class RuleSelectWindow;
class SettingsWindow;

class UIManager : public MonoBehaviour
{
    DTGENERATED_BODY(UIManager);
public:
    UIManager() = default;
    ~UIManager() override = default;

    void Awake() override;
    void Update(float deltaTime) override;

    // 모든 창을 닫아주는 함수
    void CloseAllUI();

private:
    void InitializeWindows(); // 윈도우들 생성 및 컴포넌트 주입
    void RayUpdate();     

private:
    // 관리 대상 윈도우들
    PrefabSelectWindow* m_prefabSelectWindow = nullptr;
    //RuleSelectWindow* m_ruleSelectWindow = nullptr;
    SettingsWindow* m_settingsWindow = nullptr;

    GameObject* m_hitObject = nullptr;
};