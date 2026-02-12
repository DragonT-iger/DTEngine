#pragma once
#include "MonoBehaviour.h"
#include <vector>

class Prefab;
class GameObject;

class ArrowObjectPool : public MonoBehaviour
{
	DTGENERATED_BODY(ArrowObjectPool);
public:
	ArrowObjectPool() = default;
	~ArrowObjectPool() = default;

	void Start() override;

	std::vector<GameObject*>& GetHeads() { return m_heads; }
	std::vector<GameObject*>& GetBodys() { return m_bodys; }
	//std::vector<GameObject*>& GetStarts() { return m_starts; }

	void DeactivateAll();

private:
	Prefab* m_head = nullptr;
	Prefab* m_body = nullptr;
	Prefab* m_start = nullptr;

	std::vector<GameObject*> m_heads;
	std::vector<GameObject*> m_bodys;
	//std::vector<GameObject*> m_starts;
};