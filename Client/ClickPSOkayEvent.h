#pragma once
#include "MonoBehaviour.h"

class UIButton;
class TilemapGenerator;
class Prefab;
class GameObject;

class ClickPSOkayEvent : public MonoBehaviour
{
		DTGENERATED_BODY(ClickPSOkayEvent);

public:
		void Start() override;
		//void Update(float deltaTime) override;
		void SetClick();
private:

		UIButton* m_button = nullptr;
		TilemapGenerator* m_mapData = nullptr;
		Prefab* m_selectPrefab = nullptr;

		Prefab* m_knight = nullptr;
		Prefab* m_rook = nullptr;
		Prefab* m_bishop = nullptr;

		GameObject* m_RSWindow = nullptr;
		GameObject* m_combatObj = nullptr;

		int index = 0;
};

