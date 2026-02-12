#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class GameObject;
class UIButton;
class Image;

class SecretTrigger : public MonoBehaviour
{
		DTGENERATED_BODY(SecretTrigger);
public:
		void Start() override;

private:
		GameObject* m_obj = nullptr;
		GameObject* m_hiddenObj = nullptr;
		UIButton* m_button = nullptr;
		UIButton* m_hiddenButton = nullptr;
		Vector3 m_size;
};

