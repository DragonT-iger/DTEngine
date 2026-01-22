#include "pch.h"
#include "AliceComponent.h"
#include "UIButton.h"
#include "FSMController.h"


BEGINPROPERTY(Alice)
DTPROPERTY(Alice, m_Button)
ENDPROPERTY()



void Alice::Update(float dTime)
{

}

// 클릭 시, Dance로 바꿔버려. 
// 람다에 GameObject의 수명을 보장해 줘야함. 

void Alice::Awake()
{
	if (m_Button)
	{

		auto Func = ([=]() 
			{
				auto Owner = this->_GetOwner();

				auto FSM = Owner->GetComponent<FSMController>();
				if (FSM) FSM->ChangeState(e_States::e_Dance);	
			});


		m_Button->SetOnClick(Func);
	}


}

void Alice::Start()
{

}

