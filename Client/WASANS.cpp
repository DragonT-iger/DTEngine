#include "WASANS.h"
#include "InputManager.h"

#include "GameObject.h"

BEGINPROPERTY(WASANS)
DTPROPERTY(WASANS, M_SANS)

ENDPROPERTY()





void WASANS::Update(float deltaTime) {
	if (InputManager::Instance().GetKeyDown(KeyCode::Backspace)) {
		M_SANS->SetActive(true);
	}
}




