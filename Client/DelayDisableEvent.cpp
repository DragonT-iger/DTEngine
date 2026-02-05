#include "DelayDisableEvent.h"
#include "GameObject.h"

BEGINPROPERTY(DelayDisableEvent)
DTPROPERTY(DelayDisableEvent, delay)
ENDPROPERTY()

void DelayDisableEvent::Update(float deltaTime)
{
		// 켜진 경우 timer에 시간 누적 후 delay초 후 setactive false 해주기.
		if (_GetOwner()->IsActive())
		{
				timer += deltaTime;
				if (timer >= delay)
				{
						_GetOwner()->SetActive(false);
						timer = 0;
				}
		}
}

