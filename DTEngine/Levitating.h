#pragma once
#include "MonoBehaviour.h"


class Levitating : public MonoBehaviour
{
	DTGENERATED_BODY(Levitating);

public:
	Levitating() = default;
	virtual ~Levitating() = default;

public:


void Start() override;
void Update(float deltaTime)  override;

void SetBoundary(float Boundary) { m_Boundary = Boundary; }
void SetSeed(float Speed) { m_Speed = Speed; }

private:
	float m_Boundary = 0.f; //절댓값을 범위로 처리 
	float m_Speed = 0.f;


	float m_InitialY = 0.f;  // 기준점, 은 그냥 본인 포지션 
	float m_TimeAcc = 0.f;
};