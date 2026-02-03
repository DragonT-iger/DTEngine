#pragma once
#include "MonoBehaviour.h"


class ArrowObjectPool : public MonoBehaviour
{
	DTGENERATED_BODY(ArrowObjectPool);
public:
	ArrowObjectPool() = default;
	~ArrowObjectPool() = default;

	void Awake() override;
};