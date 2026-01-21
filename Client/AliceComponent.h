#pragma once
#include "MonoBehaviour.h"


// Input처리 컴포넌트.
// Picking 처리 
// Button으로 받아요



class Alice : public MonoBehaviour
{
	DTGENERATED_BODY(Alice)
public:
	Alice() = default;
	~Alice() = default;

public:
	void Update(float dTime) override;
	void Awake() override;
	void Start() override;

	
	// 마우스 입력에 대한 처리를 받는 Component도 필요함. 
	// 입력에 대한 Component도 필요하잖아. Chess랑 Alice  -> FSM -> 
	

public:


};