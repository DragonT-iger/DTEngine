#include "pch.h"
#include <filesystem>
#include "Scene.h"
#include "GameObject.h"
#include "IDManager.h"
#include "ResourceManager.h"
#include <iostream>



struct FixupTask
{
    void* targetObject;             // 속성을 설정할 객체 (e.g. Transform*)
    PropertyInfo    property;       // 사용할 Setter 정보 (e.g. m_parent의 setter)
    uint64_t        targetID;       // 연결할 객체의 ID (e.g. 부모의 ID)
};


static void ReadPropertyRecursive(
    JsonReader& reader,
    const PropertyInfo& prop,
    void* base, 
    std::vector<FixupTask>& fixupList)
{
    const std::type_index& type = prop.m_type;
    const char* name = prop.m_name.c_str();

    if (!reader.Has(name)) return;


    if (type == typeid(int)) {
        int v = reader.ReadInt(name);
        prop.m_setter(base, &v);
    }
    else if (type == typeid(float)) {
        float v = reader.ReadFloat(name);
        prop.m_setter(base, &v);
    }
    else if (type == typeid(uint64_t)) {
        uint64_t v = reader.ReadUInt64(name);
        prop.m_setter(base, &v);
    }
    else if (type == typeid(bool)) {
        bool v = reader.ReadBool(name);
        prop.m_setter(base, &v);
    }
    else if (type == typeid(std::string)) {
        std::string v = reader.ReadString(name);
        prop.m_setter(base, &v);
    }


    else if (type == typeid(Transform*)) {
        uint64_t id = reader.ReadUInt64(name);
        fixupList.push_back({ base, prop, id });
    }


    else
    {
        const ClassInfo* structInfo = ReflectionDatabase::Instance().GetClassInfomation(type.name());

        if (structInfo)
        {
            if (reader.BeginObject(name))
            {
                void* memberData = prop.m_getter(base);

                for (const PropertyInfo& structProp : structInfo->m_properties)
                {
                    ReadPropertyRecursive(reader, structProp, memberData, fixupList);
                }
                reader.EndObject();
            }
        }
        else {
            std::cout << "Warning: No deserializer for type " << type.name() << std::endl;
        }
    }
}

static void DeserializeComponentProperties(
    JsonReader& reader,
    Component* comp, 
    std::vector<FixupTask>& fixupList)
{
    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(comp->_GetTypeName());
    if (!info) return;

    for (const PropertyInfo& prop : info->m_properties)
    {
        ReadPropertyRecursive(reader, prop, comp, fixupList);
    }
}

GameObject* Scene::CreateGameObject(const std::string& name)
{
    auto go = std::make_unique<GameObject>(name);
    GameObject* raw = go.get();
	raw->_SetID(IDManager::Instance().GetNewUniqueID());

    if (m_isIterating)
        m_pendingAdd.emplace_back(std::move(go));
    else
        m_gameObjects.emplace_back(std::move(go));
    return raw;
}



Scene::Scene() : m_name("Untitled Scene") {}

Scene::Scene(const std::string& name) : m_name(name) {}

Scene::~Scene() = default;

bool Scene::LoadFile(const std::string& fullPath)
{
    auto rd = JsonReader::LoadJson(fullPath);
    if (!rd) return false;
    JsonReader& reader = *rd;

    // --- Pass 1: 객체 생성, ID 매핑, 값 설정 ---


    std::unordered_map<uint64_t, Component*> idToComponentMap;


    std::vector<FixupTask> fixupList;

    const int n = reader.BeginArray("gameObjects");
    while (reader.NextArrayItem())
    {
        std::string goName = reader.ReadString("name", "GameObject");
        GameObject* go = CreateGameObject(goName);

        uint64_t go_id = reader.ReadUInt64("id", 0);
        go->_SetID(go_id);
        go->SetTag(reader.ReadString("tag", "Untagged"));
        go->SetActive(reader.ReadBool("active", true));


        if (reader.BeginObject("transform"))
        {
            Transform* tf = go->GetTransform();
            uint64_t tf_id = reader.ReadUInt64("id", 0);
            tf->_SetID(tf_id);

            idToComponentMap[tf_id] = tf; 

			//std::cout << "Deserializing Transform ID: " << tf_id << " for GameObject: " << goName << std::endl;

            DeserializeComponentProperties(reader, tf, fixupList);

            reader.EndObject();
        }

        const int compCount = reader.BeginArray("components");
        while (reader.NextArrayItem())
        {
            std::string typeName = reader.ReadString("typeName", "");
            uint64_t comp_id = reader.ReadUInt64("id", 0);

            if (typeName.empty() || comp_id == 0) continue;

            Component* newComp = go->AddComponent(typeName);
            if (!newComp) continue;

            newComp->_SetID(comp_id);
            idToComponentMap[comp_id] = newComp;

            DeserializeComponentProperties(reader, newComp, fixupList);

            reader.EndArray();
        }
        reader.EndArray();

        reader.EndArray();
    }
    reader.EndArray();

    // --- Pass 2 ---

    for (const auto& task : fixupList)
    {
        uint64_t targetID = task.targetID;
        Component* targetPtr = nullptr; 

        if (targetID != 0)
        {
            auto it = idToComponentMap.find(targetID);
            if (it != idToComponentMap.end())
            {
                targetPtr = it->second; 
            }
            else
            {
                std::cout << "Warning: Failed to find targetID " << targetID << std::endl;
            }
        }

        task.property.m_setter(task.targetObject, &targetPtr);
    }

    return true;
}

bool Scene::SaveFile(const std::string& solutionPath)
{
    JsonWriter writer;


    std::string assetPath = ResourceManager::Instance().ResolveFullPath(solutionPath);

    writer.BeginArray("gameObjects");

    for (auto& go : m_gameObjects)
    {
        if (!go) continue;

        writer.NextArrayItem();

        writer.Write("id", go->_GetID());
        writer.Write("name", go->GetName());
        writer.Write("tag", go->GetTag());
        writer.Write("active", go->IsActive());

        if (auto* tf = go->GetTransform())
        {
            writer.BeginObject("transform");

            writer.Write("id", tf->_GetID());

            tf->Serialize(writer);

            writer.EndObject();
        }

        writer.BeginArray("components");
        for (auto& comp : go->_GetComponents())
        {
            if (!comp) continue;

            writer.NextArrayItem();

            comp->Serialize(writer);

            writer.EndArrayItem();
        }
        writer.EndArray();

        writer.EndArrayItem();
    }
    writer.EndArray();

    return writer.SaveFile(assetPath);
}

void Scene::Unload()
{

}

GameObject* Scene::FindGameObject(std::string name)
{
    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++) {
        if ((*it)->GetName() == name) {
            return (*it).get();
        }
    }

    return nullptr;
}
void Scene::Destroy(GameObject* object)
{
    if (object)
        m_pendingDestroy.push_back(object);
}


void Scene::Awake()
{
    Initialize();

    m_phase = ScenePhase::Awake;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->Awake();
    m_isIterating = false;
    FlushPending();
}

void Scene::Start()
{
    m_phase = ScenePhase::Start;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->Start();
    m_isIterating = false;
    FlushPending();
}

void Scene::Update(float deltaTime)
{
    m_phase = ScenePhase::Update;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->Update(deltaTime);
    m_isIterating = false;
    FlushPending();
}

void Scene::FixedUpdate(float fixedDelta)
{
    m_phase = ScenePhase::FixedUpdate;

    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->FixedUpdate(fixedDelta);
    m_isIterating = false;
    FlushPending();

    //PhysicsManager::Instance().Step(fixedDelta);
}

void Scene::LateUpdate(float deltaTime)
{
    m_phase = ScenePhase::LateUpdate;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->LateUpdate(deltaTime);
    m_isIterating = false;
    FlushPending();
}

void Scene::FlushPending()
{
    std::vector<GameObject*> added;
    for (auto& up : m_pendingAdd)
    {
        GameObject* raw = up.get();
        m_gameObjects.emplace_back(std::move(up));
        added.push_back(raw);
    }
    m_pendingAdd.clear();

    if (m_phase == ScenePhase::Awake)
    {
        for (auto* obj : added)
            obj->Awake();
    }
    else
    {
        for (auto* obj : added)
        {
            obj->Awake();
            obj->Start();

        }
    }

    for (auto* dead : m_pendingDestroy)
    {
        auto it = std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [dead](auto& up) { return up.get() == dead; });
        m_gameObjects.erase(it, m_gameObjects.end());
    }
    m_pendingDestroy.clear();
}