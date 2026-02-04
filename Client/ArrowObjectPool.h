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

	std::vector<GameObject*>& GetArrows() { return m_arrows; }

private:
	Prefab* m_arrow = nullptr;
	std::vector<GameObject*> m_arrows;
};