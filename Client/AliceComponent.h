#pragma once
#include "MonoBehaviour.h"



class UIButton;


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



public:
	UIButton* m_Button = nullptr;

};