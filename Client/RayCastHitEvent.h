#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"


class TutorialManager;

class Scene;
class ArrowObjectPool;
class EnemyUnit;
class Text;

class RayCastHitEvent : public MonoBehaviour
{
		DTGENERATED_BODY(RayCastHitEvent);
public:
		void Update(float deltaTime) override;	  // ray check 해주기.

		void RaycastCheck();
		void ToggleSettingWindow();
		void ApplySettingWindow();
		void SetButtons(bool value);		// button 상호작용 막을려고 해두기.

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

		bool IsGameFinished() const { return m_finishGame; }
		void SetGameFinish(bool value) { m_finishGame = value; }
private:
		GameObject* m_hitObj = nullptr;

		GameObject* m_settingWindowBG = nullptr;

		GameObject* m_rightPSWinodwBG = nullptr;
		GameObject* m_rightRSWinodwBG = nullptr;

		GameObject* m_leftPSWinodwBG = nullptr;
		GameObject* m_leftRSWinodwBG = nullptr;

		GameObject* m_warningWindowBG = nullptr;
		GameObject* m_startButton = nullptr;
		GameObject* m_skillButton = nullptr;

		TutorialManager* m_tutorialManager = nullptr;
		bool m_isHealSkillOn = false;
		bool m_isAttackSkillOn = false;

		bool m_isStartBattle = false;
		bool m_isRay = true;
		ArrowObjectPool* m_arrowPool = nullptr;
		EnemyUnit* m_lastSelectedEnemy = nullptr;

		int curTimeScale = 1;

		bool m_finishGame = false;

		Text* m_attackCountText = nullptr;
		Text* m_healCountText = nullptr;
};

