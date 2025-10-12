#pragma once

class GameObject;

class JsonWriter; 
class JsonReader;

class Component {
public:
    Component() = default;
    virtual ~Component() = default;

    virtual void OnInspectorGUI() {}

    virtual void Serialize(JsonWriter&) const {}
    virtual void Deserialize(JsonReader&) {}

    void Destroy(GameObject* gameObject);

    
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent() const;


    void _SetOwner(GameObject* owner) { m_owner = owner; }

private:

    GameObject* _GetOwner() const { return m_owner; }
    
    GameObject* m_owner = nullptr;
};
