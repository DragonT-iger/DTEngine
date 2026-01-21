#include "pch.h"
#include "FSMController.h"


BEGINPROPERTY(FSMController)
DTPROPERTY(FSMController, m_Idle);
DTPROPERTY(FSMController, m_Die);
DTPROPERTY(FSMController, m_Skill);
DTPROPERTY(FSMController, m_Move);
DTPROPERTY(FSMController, Attack);


ENDPROPERTY()






FSMController::FSMController()
{  
    m_States.assign(static_cast<size_t>(e_States::e_Max), {});
}

//Play 시작시.
void FSMController::Start()
{
    //Editor에서 작업했다 치고.
    AdjustFSM(); //할당 된 것들을 넣고 


    //현재 상태의 OnEnter 실행 
    if (m_States[static_cast<size_t>(m_curState)].onEnter)
        m_States[static_cast<size_t>(m_curState)].onEnter(*this->_GetOwner());
}

void FSMController::Update(float dtime)
{
    size_t idx = static_cast<size_t>(m_curState);
    if (idx < m_States.size() && m_States[idx].onUpdate)
    {
        m_States[idx].onUpdate(*this->_GetOwner(), dtime);
    }
		
}

void FSMController::ChangeState(e_States nextState)
{
    if (m_curState == nextState) return;

    size_t curIdx = static_cast<size_t>(m_curState);
    if (m_States[curIdx].onExit) m_States[curIdx].onExit(*this->_GetOwner());

    m_curState = nextState;

    size_t nextIdx = static_cast<size_t>(m_curState);
    if (m_States[nextIdx].onEnter) m_States[nextIdx].onEnter(*this->_GetOwner());
}

//참조로 받아서 editor에서 해당 string값만 수정하는 형식으로. 
//해당 String 순회하면서 처리하자.



void FSMController::AdjustFSM()
{
    if (m_States.size() == static_cast<int>(e_States::e_Max))
    {
        m_States[static_cast<int>(e_States::e_Idle)] = FSMRegister::Instance().GetState(m_Idle);
        m_States[static_cast<int>(e_States::e_Move)] = FSMRegister::Instance().GetState(m_Move);
        m_States[static_cast<int>(e_States::e_Die)] = FSMRegister::Instance().GetState(m_Die);
        m_States[static_cast<int>(e_States::e_Attack)] = FSMRegister::Instance().GetState(Attack);
        m_States[static_cast<int>(e_States::e_Skill)] = FSMRegister::Instance().GetState(m_Skill);

    }




}
