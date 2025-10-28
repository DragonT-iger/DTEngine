﻿#include "pch.h"

#include "GameObject.h"
#include "MonoBehaviour.h"
#include "ComponentFactory.h"

GameObject::GameObject(const std::string& name , bool isUI)
    : m_name{ name }, m_active{ true } {
    this->AddComponent<Transform>(); 
}

void GameObject::RemoveComponent(Component* component)
{
    if (!component) return;

    // Transform 을 제거하면 안됨

    if (m_isIterating)
        m_pendingRemove.push_back(component);
    else {
        auto it = std::remove_if(m_components.begin(), m_components.end(),
            [component](auto& up) { return up.get() == component; });
        m_components.erase(it, m_components.end());
    }
}

/* Activation -------------------------------------------------- */
void GameObject::SetActive(bool active) { m_active = active; }
bool GameObject::IsActive() const { return m_active; }



/* Lifecycle Methods -------------------------------------------- */

void GameObject::Awake() {

    if (m_awakeCalled) return;
    m_phase = Phase::Awake;
    m_isIterating = true;
    for (auto& comp : m_components)
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
            mb->Awake();
    m_isIterating = false;
    FlushPending();
    m_awakeCalled = true;
}

void GameObject::Start() {
    if (m_startCalled) return;
    m_phase = Phase::Start;
    m_isIterating = true;
    for (auto& comp : m_components)
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
            mb->Start();
    m_isIterating = false;
    FlushPending();
    m_startCalled = true;
}

void GameObject::Update(float deltaTime) {
    m_phase = Phase::Update;
    m_isIterating = true;
    for (auto& comp : m_components) {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
            if (IsActive())
                if (mb->IsActive())
                    mb->Update(deltaTime);
    }
    m_isIterating = false;
    FlushPending();
}

void GameObject::FixedUpdate(float fixedDeltaTime) {
    m_phase = Phase::FixedUpdate;
    m_isIterating = true;
    for (auto& comp : m_components)
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
            if (IsActive())
                if (mb->IsActive())
                    mb->FixedUpdate(fixedDeltaTime);
    m_isIterating = false;
    FlushPending();
}

void GameObject::LateUpdate(float deltaTime) {
    m_phase = Phase::LateUpdate;
    m_isIterating = true;
    for (auto& comp : m_components)
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
            if (IsActive())
                if (mb->IsActive())
                    mb->LateUpdate(deltaTime);
    m_isIterating = false;
    FlushPending();
}

void GameObject::BroadcastTriggerEnter(Collider* other)
{
    for (auto& comp : m_components) {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get())) {
            mb->OnTriggerEnter(other);
        }
    }
}

void GameObject::BroadcastTriggerStay(Collider* other)
{
    for (auto& comp : m_components) {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get())) {
            mb->OnTriggerStay(other);
        }
    }
}

void GameObject::BroadcastTriggerExit(Collider* other)
{
    for (auto& comp : m_components) {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get())) {
            mb->OnTriggerExit(other);
        }
    }
}
Component* GameObject::AddComponent(const std::string& typeName)
{
    std::unique_ptr<Component> newComponent =
        ComponentFactory::Instance().Create(typeName);

    if (!newComponent) {
        return nullptr;
    }

    Component* raw = newComponent.get();

    m_components.emplace_back(std::move(newComponent));

    return raw;
}
GameObject* GameObject::Find(std::string name)
{
    //Scene* curScene = SceneManager::Instance().GetActiveScene();
    //return curScene->FindGameObject(name);

    return nullptr;

}
void GameObject::FlushPending()
{
    std::vector<Component*> added;
    for (auto& up : m_pendingAdd)
    {
        Component* raw = up.get();
        m_components.emplace_back(std::move(up));
        added.push_back(raw);
    }
    m_pendingAdd.clear();


    if (!added.empty())
    {
        for (auto* comp : added)
            if (auto* mb = dynamic_cast<MonoBehaviour*>(comp))
            {
                if (IsActive() && mb->IsActive()) {
                    mb->Awake();
                    mb->Start();
                }
            }
    }

    for (auto* dead : m_pendingRemove)
    {
        auto it = std::remove_if(m_components.begin(), m_components.end(),
            [dead](auto& up) { return up.get() == dead; });
        m_components.erase(it, m_components.end());
    }
    m_pendingRemove.clear();
}
