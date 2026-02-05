#pragma once
#include "MonoBehaviour.h"


class UIButton;

class TutorialSceneTriggerevent : public MonoBehaviour
{

	DTGENERATED_BODY(TutorialSceneTriggerevent);

public:
	 TutorialSceneTriggerevent() = default;
	~TutorialSceneTriggerevent() = default;
public:
	void Start() override;
private:
	UIButton* m_Button = nullptr;
};

