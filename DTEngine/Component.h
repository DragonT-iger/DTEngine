#pragma once

class GameObject;

#include "ReflectionMacros.h"

class Component {
public:
    Component() = default;
    virtual ~Component() = default;

    void Destroy(GameObject* gameObject);

    void Instantiate(std::string name);

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent() const;


    // ----------------- 엔진 전용 -------------------

    void _SetOwner(GameObject* owner) { m_owner = owner; }
    GameObject* _GetOwner() const { return m_owner; }



    uint64_t _GetID() const { return m_id; }
    void _SetID(uint64_t id) { m_id = id; }


    // DTGENERATED_BODY 매크로로 자동 생성
	virtual const char* _GetTypeName() const = 0; 

private:

    
    GameObject* m_owner = nullptr;
    uint64_t m_id = 0; 
};
