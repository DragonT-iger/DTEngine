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

    void InitializeWindows(); // 윈도우들 생성 및 컴포넌트 주입
    void RayUpdate();     
    // 이제 rayupdate도 안쓰고 update override도 안쓸거임.

    void HandleEditorHit(GameObject* hit);          // editorui에서 호출할 때 이전 ray 재사용 하기 위해서.
    void HandlePrefabSelection(GameObject* hit);    // hit 된 오브젝트가 프리팹 선택 대상이면 ui 열기 및 생성 콜백 연결

private:
    // 관리 대상 윈도우들
    PrefabSelectWindow* m_prefabSelectWindow = nullptr;
    //RuleSelectWindow* m_ruleSelectWindow = nullptr;
    SettingsWindow* m_settingsWindow = nullptr;

    GameObject* m_hitObject = nullptr;
};