#pragma once

#include "MonoBehaviour.h"

class GameObject;
class Image;
class UIButton;

class ImageSwitcher : public MonoBehaviour
{
		DTGENERATED_BODY(ImageSwitcher);
public:
		void Start() override;

private:
		GameObject* m_obj = nullptr;


		Image* m_img = nullptr;
};

