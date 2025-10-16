#pragma once

class GameObject;

#include "JsonIO.h"

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


    void _SetOwner(GameObject* owner) { m_owner = owner; }

    virtual void Serialize(JsonWriter&) const {}
    virtual void Deserialize(JsonReader&) {}

private:

    GameObject* _GetOwner() const { return m_owner; }
    
    GameObject* m_owner = nullptr;
};
