#pragma once
#include "MonoBehaviour.h"
class SelectIndexEvent : public MonoBehaviour
{
		DTGENERATED_BODY(SelectIndexEvent);
public:
		void SetIndex(int value) { m_index = value; }
		const int& GetIndex() const { return m_index; }
private:
		int m_index = 0;
};

