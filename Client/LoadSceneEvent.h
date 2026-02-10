#pragma once
#include "MonoBehaviour.h"

class LoadSceneEvent : public MonoBehaviour
{
	DTGENERATED_BODY(LoadSceneEvent)

public:
	void Start();




private:
	std::string m_sceneName;

};

