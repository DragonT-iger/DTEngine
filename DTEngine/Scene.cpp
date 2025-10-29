#include "pch.h"
#include <filesystem>
#include "Scene.h"
#include "GameObject.h"
#include "IDManager.h"


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

    try {
        std::filesystem::path p(fullPath);
        m_name = p.stem().string(); 
    }
    catch (...) {
        m_name = "LOAD_FAILED";
    }

    auto rd = JsonReader::LoadJson(fullPath);
    if (!rd) {
        return false;
    }
    JsonReader& r = *rd;

    const int n = r.BeginArray("gameObjects");
    std::vector<GameObject*> gameObjects(n, nullptr);
    std::vector<int> parents(n, -1);

    int i = 0;
    while (r.NextArrayItem())
    {
        GameObject* go = CreateGameObject(r.ReadString("name", "GameObject"));
        gameObjects[i] = go;

        if (auto* tf = go->GetComponent<Transform>())
            //tf->Deserialize(r); 

        parents[i] = r.ReadUInt64("parent", -1);
        ++i;
    }
    r.EndArray();

    for (int ci = 0; ci < n; ++ci) {
        int pi = parents[ci];
        if (pi >= 0 && pi < n && gameObjects[ci] && gameObjects[pi]) {
            auto* cT = gameObjects[ci]->GetComponent<Transform>();
            auto* pT = gameObjects[pi]->GetComponent<Transform>();
            if (cT && pT) cT->SetParent(pT, true);
        }
    }
    return true;
}

bool Scene::SaveFile(const std::string& fullPath)
{
    JsonWriter w; 

    std::unordered_map<Transform*, int> transformIndexMap;
    transformIndexMap[nullptr] = -1; 

    for (int i = 0; i < m_gameObjects.size(); ++i)
    {
        Transform* tf = m_gameObjects[i]->GetTransform(); 
        if (tf) {
            transformIndexMap[tf] = i;
        }
    }

    w.BeginArray("gameObjects");

    for (int i = 0; i < m_gameObjects.size(); ++i)
    {
        GameObject* go = m_gameObjects[i].get();
        if (!go) continue;

        w.NextArrayItem(); 

         //w.Write("id", go->GetID()); 
        w.Write("name", go->GetName()); 
        w.Write("tag", go->GetTag());   

        Transform* tf = go->GetTransform(); 
        Transform* parentTf = tf ? tf->GetParent() : nullptr; 

        int parentIndex = -1;
        auto it = transformIndexMap.find(parentTf);
        if (it != transformIndexMap.end()) {
            parentIndex = it->second;
        }
        w.Write("parent", parentIndex); 

        
        if (tf)
        {
            //tf->Serialize(w);
        }

        w.BeginArray("components");

        for (auto& comp_ptr : go->_GetComponents())
        {
            Component* comp = comp_ptr.get();

            if (dynamic_cast<Transform*>(comp))
            {
                continue;
            }

            w.NextArrayItem(); 

            // meta
            w.BeginObject("meta"); 
            // w.Write("id", comp->GetID()); 
            w.Write("type", comp->_GetTypeName()); 
            w.EndObject(); 

            w.BeginObject("data");
            //comp->Serialize(w); 
            w.EndObject();

            w.EndArrayItem(); 
        }
        w.EndArray();     

        w.EndArrayItem();  
    }
    w.EndArray();         

    return w.SaveFile(fullPath);
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