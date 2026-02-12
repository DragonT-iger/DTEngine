#include "ArrowObjectPool.h"

#include "GameObject.h"
#include "Prefab.h"

BEGINPROPERTY(ArrowObjectPool)
DTPROPERTY(ArrowObjectPool, m_head)
DTPROPERTY(ArrowObjectPool, m_body)
DTPROPERTY(ArrowObjectPool, m_start)
ENDPROPERTY()


void ArrowObjectPool::Start()
{
    for (int i = 0; i < 7; i++)
    {
        if (m_head)
        {
            GameObject* head = m_head->Instantiate();
            head->GetTransform()->SetParent(_GetOwner()->GetTransform());
            head->SetActive(false);
            m_heads.push_back(head);
        }

        if (m_body)
        {
            GameObject* body = m_body->Instantiate();
            body->GetTransform()->SetParent(_GetOwner()->GetTransform());
            body->SetActive(false);
            m_bodys.push_back(body);
        }

        //if (m_start)
        //{
        //    GameObject* start = m_start->Instantiate();
        //    start->GetTransform()->SetParent(_GetOwner()->GetTransform());
        //    start->SetActive(false);
        //    m_starts.push_back(start);
        //}
    }
}

void ArrowObjectPool::DeactivateAll()
{
    for (int i = 0; i < 7; i++)
    {
        m_heads[i]->SetActive(false);
        m_bodys[i]->SetActive(false);
        //m_starts[i]->SetActive(false);
    }
}