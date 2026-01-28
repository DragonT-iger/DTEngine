#pragma once
#include "MonoBehaviour.h"

class GameObject;
class Transform;

class OpenPSEvent : public MonoBehaviour
{
		DTGENERATED_BODY(OpenPSEvent);
public:
		//void Update(float deltaTime) override;

		void SetActivePSWindow();

		void SetRayObj(GameObject* obj) { m_rayObject = obj; }
		GameObject* GetRayObj() { return m_rayObject; }
		void SetIsOpen(bool value) { m_isOpen = value; }
		void SetRayObjTransform(Transform* tf) { m_rayobjTransform = tf; }
		Transform* GetRayObjTransform() { return m_rayobjTransform; }

private:
		GameObject* m_rayObject = nullptr;
		Transform* m_rayobjTransform = nullptr;
		bool m_isOpen = false;
};

