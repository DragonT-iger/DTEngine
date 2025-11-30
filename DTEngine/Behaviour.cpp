#include "pch.h"
#include "Behaviour.h"
#include "MonoBehaviour.h"
#include "GameObject.h"




void Behaviour::SetActive(bool state)
{
    m_active = state;

    if (auto* mb = dynamic_cast<MonoBehaviour*>(this)) {
        if (_GetOwner()->IsActiveInHierarchy()) {
            if (m_active) mb->OnEnable();
            else mb->OnDisable();
        }
    }
}