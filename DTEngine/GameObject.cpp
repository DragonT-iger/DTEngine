#include "pch.h"

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
void GameObject::SetActive(bool active) { 
    m_active = active; 

    for (const auto& comp : m_components)
    {
        // MonoBehaviour를 상속받지 않은  Transform은 nullptr 반환되어 무시됨
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
        {
            if (m_active)
            {
                mb->OnEnable();
            }
            else
            {
                mb->OnDisable();
            }
        }
    }

}
bool GameObject::IsActive() const { return m_active; }



/* Lifecycle Methods -------------------------------------------- */

void GameObject::Awake() {

    if (m_awakeCalled) return;

    m_phase = Phase::Awake;
    m_isIterating = true;
    for (auto& comp : m_components)
        if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get())) {
            mb->Awake();
        }
            
    m_isIterating = false;
    FlushPending();
    m_awakeCalled = true;

    if (m_transform)
    {
        const auto& children = m_transform->GetChildren();
        for (Transform* childTf : children)
        {
            if (childTf && childTf->_GetOwner())
            {
                childTf->_GetOwner()->Awake();
            }
        }
    }
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

    if (m_transform)
    {
        const auto& children = m_transform->GetChildren();
        for (Transform* childTf : children)
        {
            if (childTf && childTf->_GetOwner())
            {
                childTf->_GetOwner()->Start();
            }
        }
    }
}

void GameObject::Update(float deltaTime) {

    if (!IsActiveInHierarchy()) return;

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

    if (!IsActiveInHierarchy()) return;

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

    if (!IsActiveInHierarchy()) return;

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

    raw->_SetOwner(this);
    raw->_SetID(IDManager::Instance().GetNewUniqueID());

    if (auto* mb = dynamic_cast<MonoBehaviour*>(raw)) {
        if (IsActive() && mb->IsActive() && IsActiveInHierarchy()) mb->OnEnable();
    }

    m_components.emplace_back(std::move(newComponent));

    return raw;
}
void GameObject::_Internal_AddComponent(std::unique_ptr<Component> comp)
{
    Component* raw = comp.get();
    raw->_SetOwner(this);
    if (raw->_GetID() == 0) 
    {
        raw->_SetID(IDManager::Instance().GetNewUniqueID());
    }

    m_components.push_back(std::move(comp));

    if (m_phase == Phase::Awake)
    {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(raw)) {
            mb->Awake();
            if (IsActive() && mb->IsActive() && IsActiveInHierarchy()) mb->OnEnable();
        }
    }
    else if (m_phase != Phase::None)
    {
        if (auto* mb = dynamic_cast<MonoBehaviour*>(raw))
        {
            mb->Awake();
            mb->Start();
            if (IsActive() && mb->IsActive() && IsActiveInHierarchy()) mb->OnEnable();
        }
    }
}

std::unique_ptr<Component> GameObject::_Internal_RemoveComponent(Component* comp)
{
    if (!comp || dynamic_cast<Transform*>(comp))
    {
        return nullptr;
    }

    auto it = std::find_if(m_components.begin(), m_components.end(),
        [comp](const auto& p) { return p.get() == comp; });

    if (it != m_components.end())
    {
        std::unique_ptr<Component> found = std::move(*it);
        m_components.erase(it);
        return found;
    }

    it = std::find_if(m_pendingAdd.begin(), m_pendingAdd.end(),
        [comp](const auto& p) { return p.get() == comp; });

    if (it != m_pendingAdd.end())
    {
        std::unique_ptr<Component> found = std::move(*it);
        m_pendingAdd.erase(it);
        return found;
    }

    return nullptr;
}
bool GameObject::IsActiveInHierarchy() const
{
    if (!m_active) return false;

    Transform* parent = m_transform->GetParent();
    if (parent)
    {
        return parent->_GetOwner()->IsActiveInHierarchy();
    }

    return true;
}

void CopyComponentProperties(Component* src, Component* dst)
{
    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(src->_GetTypeName());
    if (!info) return;

    for (const PropertyInfo& prop : info->m_properties)
    {
        void* srcValue = prop.m_getter(src);
        prop.m_setter(dst, srcValue);
    }
}

std::unique_ptr<GameObject> GameObject::Clone()
{

    auto newObj = std::make_unique<GameObject>(m_name);

    newObj->_SetID(IDManager::Instance().GetNewUniqueID());
    newObj->SetTag(m_tag);
    newObj->SetActive(m_active);

    Transform* srcTf = this->GetTransform();
    Transform* dstTf = newObj->GetTransform();

    dstTf->SetPosition(srcTf->GetPosition());
    dstTf->SetRotationQuat(srcTf->GetRotationQuat());
    dstTf->SetScale(srcTf->GetScale());

    for (const auto& srcComp : m_components)
    {
        if (dynamic_cast<Transform*>(srcComp.get())) continue;

        Component* dstComp = newObj->AddComponent(srcComp->_GetTypeName());
        if (dstComp)
        {
            dstComp->_SetID(IDManager::Instance().GetNewUniqueID());

            CopyComponentProperties(srcComp.get(), dstComp);
        }
    }

    const auto& children = srcTf->GetChildren();
    for (Transform* child : children)
    {
        GameObject* childGO = child->_GetOwner();
        if (childGO)
        {
            auto clonedChild = childGO->Clone();

            clonedChild->GetTransform()->SetParent(dstTf);

            // 소유권 이전 (Scene에 바로 넣는게 아니라 부모가 관리하는 구조라면 로직 수정 필요.
            // 보통 Scene 구조에서는 Scene이 모든 GO를 들고 있으므로, 
            // 여기서는 Clone만 하고 Command에서 Scene에 등록하도록 처리합니다.)

            // 주의: 현재 엔진 구조상 부모를 설정해도 Scene 리스트에 등록되어야 Update가 돕니다.
            // 따라서 Clone 함수는 순수 데이터 복제만 담당하고, 
            // 실제 Scene 등록은 재귀적으로 처리해주는 별도 로직이 필요할 수 있습니다.
            // 편의상 여기서는 '계층 구조'만 복사하고, 
            // Command에서 반환된 루트 객체를 Scene에 등록하면, 
            // Scene::Internal_AddGameObject 등에서 자식들도 순회하며 등록하는지 확인이 필요합니다.
            // 제공된 코드를 보면 Scene::AddGameObject는 단일 객체만 넣는 것으로 보입니다.
            // 일단 '계층구조 복사'를 위해 부모만 세팅해둡니다.
        }
    }

    return newObj;
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
