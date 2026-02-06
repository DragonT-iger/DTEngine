#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"


class TutorialManager;

class Scene;
class ArrowObjectPool;
class EnemyUnit;

class RayCastHitEvent : public MonoBehaviour
{
		DTGENERATED_BODY(RayCastHitEvent);
public:
		void Update(float deltaTime) override;	  // ray check 해주기.

		void RaycastCheck();
		void ToggleSettingWindow();

		GameObject* GetHitObject() { return m_hitObj; }
		void SetHitObject(GameObject* hitobj) { m_hitObj = hitobj; }
		bool IsMouseInUI(GameObject* uiObj, const Vector2& mousePos);
		void CloseAllWindows();

		void SetHealSkill(bool healSkill) { m_isHealSkillOn = healSkill; }
		void SetAttackSkill(bool attackSkill) { m_isAttackSkillOn = attackSkill; }
		void SetStartBattle(bool battlestart) { m_isStartBattle = battlestart; }
		bool GetStartBattle() { return m_isStartBattle; }

		bool GetHealSkill() { return m_isHealSkillOn; }
		bool GetAttackSkill() { return m_isAttackSkillOn; }
		EnemyUnit* CheckEnemyObj(GameObject* obj);		// enemy check 용도로 따로 빼기.

		ArrowObjectPool* GetArrowPool() { return m_arrowPool; }
		void SetRay(bool value) { m_isRay = value; }
		bool GetRay() { return m_isRay; }
private:
		GameObject* m_hitObj = nullptr;

		GameObject* m_settingWindowBG = nullptr;

		GameObject* m_rightPSWinodwBG = nullptr;
		GameObject* m_rightRSWinodwBG = nullptr;

		GameObject* m_leftPSWinodwBG = nullptr;
		GameObject* m_leftRSWinodwBG = nullptr;

		GameObject* m_warningWindowBG = nullptr;
		GameObject* m_startButton = nullptr;

		TutorialManager* m_tutorialManager = nullptr;
		bool m_isHealSkillOn = false;
		bool m_isAttackSkillOn = false;

		bool m_isStartBattle = false;
		bool m_isRay = true;
		ArrowObjectPool* m_arrowPool = nullptr;
		EnemyUnit* m_lastSelectedEnemy = nullptr;

		int curTimeScale = 1;
};

