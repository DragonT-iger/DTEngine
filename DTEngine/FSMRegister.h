#pragma once
#include "Singleton.h"
#include <functional>
#include <map>
#include <string>

using StateEnter = std::function<void(GameObject&)>;
using StateUpdate = std::function<void(GameObject& , float)>;
using StateExit = std::function<void(GameObject&)>;

struct State { StateEnter onEnter; StateUpdate onUpdate; StateExit onExit; }; //이걸 넣고

// Awake보다 전, 함수 객체를 생성

class FSMRegister : public Singleton<FSMRegister>
{
	friend class Singleton<FSMRegister>;


public:
	bool Initalize();
	State GetState(std::string); //ex A_Idle 

	void RegisterState(const std::string& key, State state)
	{ m_FunctionMap[key] = state; }

private:
	FSMRegister() = default;
	~FSMRegister();
private:
	std::map<std::string, State> m_FunctionMap;



};