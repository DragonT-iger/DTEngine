#include "ArrowObjectPool.h"

#include "GameObject.h"
#include "Prefab.h"

BEGINPROPERTY(ArrowObjectPool)
DTPROPERTY(ArrowObjectPool, m_arrow)
ENDPROPERTY()


void ArrowObjectPool::Start()
{
    for (int i = 0; i < 64; i++)
    {
        GameObject* arrow = m_arrow->Instantiate();
        arrow->GetTransform()->SetParent(_GetOwner()->GetTransform());
        // 필요 시 여기에 addcomponent 하는데 할려면 include 먼저.
        arrow->SetActive(false);

        m_arrows.push_back(arrow);
    }
}
