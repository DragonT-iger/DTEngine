#include "pch.h"


#include <filesystem>
#include <iostream>


#include "Scene.h"
#include "GameObject.h"
#include "IDManager.h"
#include "ResourceManager.h"
#include "DX11Renderer.h"
#include "Light.h"
#include "Behaviour.h"
#include "SerializationUtils.h"




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

void Scene::_Internal_AddGameObject(std::unique_ptr<GameObject> go)
{
    GameObject* raw = go.get();
    m_gameObjects.push_back(std::move(go));

    if (m_phase == ScenePhase::Awake)
    {
        raw->Awake();
    }
    else if (m_phase != ScenePhase::None)
    {
        raw->Awake();
        raw->Start();
    }
}

std::unique_ptr<GameObject> Scene::_Internal_RemoveGameObject(GameObject* go)
{
    auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
        [go](const auto& p) { return p.get() == go; });

    if (it != m_gameObjects.end())
    {
        std::unique_ptr<GameObject> found = std::move(*it);
        m_gameObjects.erase(it);
        return found;
    }

    it = std::find_if(m_pendingAdd.begin(), m_pendingAdd.end(),
        [go](const auto& p) { return p.get() == go; });

    if (it != m_pendingAdd.end())
    {
        std::unique_ptr<GameObject> found = std::move(*it);
        m_pendingAdd.erase(it);
        return found;
    }

    return nullptr;
}



Scene::Scene() : m_name("Untitled Scene") {}

Scene::Scene(const std::string& name) : m_name(name) {}

Scene::~Scene() = default;

bool Scene::LoadFile(const std::string& fullPath)
{
    auto rd = JsonReader::LoadJson(fullPath);
    if (!rd) return false;
    JsonReader& reader = *rd;


    try
    {
        std::filesystem::path p(fullPath);
        m_name = p.stem().string(); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "Warning: Could not parse scene name from path. " << e.what() << std::endl;
        
        m_name = "ErrorSceneName";
    }

    // --- Pass 1: 객체 생성, ID 매핑, 값 설정 ---


    std::unordered_map<uint64_t, Component*> idToComponentMap;


    std::vector<FixupTask> fixupList;

    const int n = reader.BeginArray("gameObjects");
    while (reader.NextArrayItem())
    {
        std::string goName = reader.ReadString("name", "GameObject");
        GameObject* go = CreateGameObject(goName);

		//std::cout << "Deserializing GameObject: " << goName << std::endl;

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

            if (auto* behaviour = dynamic_cast<Behaviour*>(newComp))
            {
                bool isActive = reader.ReadBool("Active", true);
                behaviour->SetActive(isActive);
            }

            DeserializeComponentProperties(reader, newComp, fixupList);
            reader.EndArrayItem();
        }
        reader.EndArray();

		reader.EndArrayItem();
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
				//std::cout << "Fixup: Resolved targetID " << targetID << std::endl;
            }
            else
            {
                std::cout << "Warning: Failed to find targetID " << targetID << std::endl;
            }
        }
   //     else {
			//std::cout << "Fixup: targetID is 0, setting to nullptr" << std::endl;
   //     }
        
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

        if (go->HasFlag(GameObject::Flags::DontSave))
            continue;

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

            writer.Write("typeName", comp->_GetTypeName());
            
            writer.Write("id", comp->_GetID());

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
    auto it = std::find(m_pendingDestroy.begin(), m_pendingDestroy.end(), object);
    if (it != m_pendingDestroy.end()) return;


    m_pendingDestroy.push_back(object);


    Transform* tf = object->GetTransform();
    if (tf)
    {
        const auto& children = tf->GetChildren();
        for (Transform* child : children)
        {
            Destroy(child->_GetOwner());
        }
    }
}


void Scene::Awake()
{

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
    for (auto& obj : m_gameObjects)
    {
        if (obj->GetTransform()->GetParent() == nullptr)
        {
            obj->Update(deltaTime); 
        }
    }
    m_isIterating = false;
    FlushPending();
}

void Scene::FixedUpdate(float fixedDelta)
{
    m_phase = ScenePhase::FixedUpdate;

    m_isIterating = true;
    for (auto& obj : m_gameObjects)
    {
        if (obj->GetTransform()->GetParent() == nullptr)
        {
            obj->FixedUpdate(fixedDelta);
        }
    }
    m_isIterating = false;
    FlushPending();

    //PhysicsManager::Instance().Step(fixedDelta);
}

void Scene::LateUpdate(float deltaTime)
{
    m_phase = ScenePhase::LateUpdate;
    m_isIterating = true;
    for (auto& obj : m_gameObjects)
    {
        if (obj->GetTransform()->GetParent() == nullptr)
        {
            obj->LateUpdate(deltaTime);
        }
    }
    m_isIterating = false;
    FlushPending();




    DX11Renderer::Instance().UpdateLights(Light::GetAllLights());
}

Camera* Scene::GetMainCamera()
{
    return m_mainCamera;
}

void Scene::SetMainCamera(Camera* mainCamera)
{
    m_mainCamera = mainCamera;
}


void Scene::Clear()
{
    m_gameObjects.clear();     
    m_pendingAdd.clear();      
    m_pendingDestroy.clear();
    m_mainCamera = nullptr;
}

GameObject* Scene::FindGameObjectByID(uint64_t id)
{
    for (const auto& go : m_gameObjects)
    {
        if (go->_GetID() == id)
            return go.get();
    }
    return nullptr;
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