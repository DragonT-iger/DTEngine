#include "UnitActionEvent.h"
#include "GameObject.h"


BEGINPROPERTY(UnitActionEvent)
ENDPROPERTY()

void UnitActionEvent::UnitMove(Vector3 pos)
{
		_GetOwner()->GetTransform()->SetPosition(pos);
}
