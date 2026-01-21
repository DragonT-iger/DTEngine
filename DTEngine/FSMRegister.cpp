#include "pch.h"
#include "FSMRegister.h"


//여기서 Register를 임의로 만듦.

bool FSMRegister::Initalize()
{
    State idleState;
    idleState.onEnter = [](GameObject& Owner) {  };
    idleState.onUpdate = [](GameObject& Owner, float dt)
        {
      
        };
    RegisterState("P_Idle", idleState);

    State moveState;
    moveState.onEnter = [](GameObject& Owner) { };
    moveState.onUpdate = [](GameObject& Owner, float dt)
        {
      
        };

    RegisterState("P_Move", moveState);

    State attackState;
    attackState.onEnter = [](GameObject& Owner) { };
    attackState.onUpdate = [](GameObject& Owner, float dt) {
        };
    attackState.onExit = [](GameObject& Owner) {};
    RegisterState("P_Attack", attackState);

    State dieState;
    dieState.onEnter = [](GameObject& Owner) {
      
        };
    dieState.onUpdate = [](GameObject& Owner, float dt) {  };
    RegisterState("P_Die", dieState);

    return true;
}

State FSMRegister::GetState(std::string key)
{

    if (key == "") return State(); //비용좀 아낄래

    auto iter = m_FunctionMap.find(key);
    if (iter != m_FunctionMap.end())
    {
        return iter->second;
    }


    return State();
}

FSMRegister::~FSMRegister()
{
    m_FunctionMap.clear();
}


