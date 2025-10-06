#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>


#include "Component.h"
#include "Transform.h";


class Collider;



class GameObject
{
public:
    GameObject(const std::string& name = "GameObject");

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent() const;

    void RemoveComponent(Component* component);

    void SetActive(bool active);
    bool IsActive() const;

    static GameObject* Find(std::string name);


    Transform* GetTransform() { return m_transform.get(); }
    


    //  ------------------------엔진 전용 함수------------------------

    void _Awake();
    void _Start();
    void _Update(float deltaTime);
    void _FixedUpdate(float fixedDeltaTime);
    void _LateUpdate(float deltaTime);


    void _BroadcastTriggerEnter(Collider* other);
    void _BroadcastTriggerStay(Collider* other);
    void _BroadcastTriggerExit(Collider* other);

    const std::vector<std::unique_ptr<Component>>& _GetComponents() const { return m_components; }



    

    //Getter Setter

    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }

    const std::string& GetTag() const { return m_tag; }
    void SetTag(const std::string& tag) { m_tag = tag; }

private:

    std::string                                   m_name;
    std::string                                   m_tag = "Untagged";
    bool                                          m_active = true;
    std::vector<std::unique_ptr<Component>>       m_components;
    std::unique_ptr<Transform>                    m_transform;




    //지연처리 로직

    void FlushPending();

    bool                                          m_isIterating = false;
    std::vector<std::unique_ptr<Component>>       m_pendingAdd;
    std::vector<Component*>                       m_pendingRemove;

    enum class Phase { None, Awake, Start, Update, FixedUpdate, LateUpdate };

    Phase                                         m_phase = Phase::None;
    bool                                          m_awakeCalled = false;
    bool                                          m_startCalled = false;
};

// 템플릿은 cpp로 못옮겨

// 리턴 raw로 주니까 delete 절대하지마셈
template<typename T, typename... Args> // inline은 ODR 방지용
T* GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    // 컴파일 타임 타입 체크

    if (T* existing = this->GetComponent<T>())
    {
        // 유니티에서는 중복으로 컴포넌트가 되긴 하는데 실수할 여지가 너무 많아서 처리했음
        std::cout << L"[경고] " << typeid(T).name()
            << L" 컴포넌트가 이미 존재합니다. 중복 추가를 건너뛰고 기존거 리턴함.\n";
        // 디버그용으로 완전 차단하려면 assert 삽입
        // assert(false && "Duplicate component!");
        return existing;
    }

    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    T* raw = comp.get();

    raw->SetOwner(this);

    if (m_isIterating)
        m_pendingAdd.emplace_back(std::move(comp));
    else
        m_components.emplace_back(std::move(comp));
    return raw;
}
template<>
Transform* GameObject::AddComponent<Transform>() {
    if (m_transform)
    {
        std::cout << L"[GameObject] Transform already exists\n";
        return m_transform.get();
    }
    m_transform = std::make_unique<Transform>();
    auto* raw = m_transform.get();

    raw->_SetOwner(this);

    return m_transform.get();
}

template<typename T>
T* GameObject::GetComponent() const
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
    // 컴파일 타임 타입 체크

    for (auto& c : m_components)
        if (auto casted = dynamic_cast<T*>(c.get()))
            return casted;
    return nullptr;
}
template<>
Transform* GameObject::GetComponent<Transform>() const
{
    return m_transform.get();
}


template<typename T, typename... Args>
T* Component::AddComponent(Args&&... args) {
    auto* owner = _GetOwner();
    if (!owner) {
        assert(false && "Component has no owner");
        return nullptr;
    }
    return owner->template AddComponent<T>(std::forward<Args>(args)...);
}

template<typename T>
T* Component::GetComponent() const {
    auto* owner = _GetOwner();
    if (!owner) return nullptr;
    return owner->template GetComponent<T>();
}