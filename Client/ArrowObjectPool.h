#pragma once
#include "MonoBehaviour.h"

class Prefab;
class GameObject;

class ArrowObjectPool : public MonoBehaviour
{
	DTGENERATED_BODY(ArrowObjectPool);
public:
	ArrowObjectPool() = default;
	~ArrowObjectPool() = default;

	void Start() override;

private:
	Prefab* m_arrow = nullptr;
};