#pragma once
#include "MonoBehaviour.h"
#include <vector>
#include <functional>

#include "FSMRegister.h"

class GameObject;


//GameObject Alice & Chess 에 국한됨
enum class e_States
{
	e_Idle,
	e_Move,
	e_Die,
	e_Attack,
	e_Skill,

	e_Max
};




class FSMController : public MonoBehaviour
{
	DTGENERATED_BODY(FSMController);

public:
	FSMController();
	~FSMController() = default;
public:
	void Start() override;
	void Update(float dTime) override;
public:

	void ChangeState(e_States nextState);
	void AdjustFSM();

private:

	e_States m_curState = e_States::e_Idle;
	std::vector<State> m_States;

	std::string m_Idle;
	std::string m_Die;
	std::string m_Skill;
	std::string m_Move;
	std::string Attack;


};

