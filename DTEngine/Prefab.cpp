#include "pch.h"
#include "Prefab.h"
#include "JsonIO.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "SerializationUtils.h"
#include <fstream>
#include <sstream>

bool Prefab::LoadFile(const std::string& fullPath)
{
    std::ifstream file(fullPath);
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_jsonString = buffer.str();

    return !m_jsonString.empty();
}

bool Prefab::SaveFile(const std::string& fullPath)
{
    std::ofstream file(fullPath);
    if (!file.is_open())
        return false;

    file << m_jsonString;
    return true;
}

void Prefab::Unload()
{
    m_jsonString.clear();
}

GameObject* Prefab::Instantiate()
{
    if (m_jsonString.empty())
        return nullptr;

    JsonReader reader(m_jsonString);

    Scene* scene = SceneManager::Instance().GetActiveScene();
    if (!scene) return nullptr;

    std::unordered_map<uint64_t, Component*> fileIDToNewComponent;
    std::unordered_map<uint64_t, GameObject*> fileIDToNewGameObject;
    std::vector<FixupTask> fixupList;
    std::vector<std::pair<GameObject*, uint64_t>> parentLinks;

    GameObject* rootObject = nullptr;

    reader.BeginArray("gameObjects");

    while (reader.NextArrayItem())
    {
        std::string goName = reader.ReadString("name", "GameObject");
        uint64_t fileGoID = reader.ReadUInt64("id", 0);
        uint64_t fileParentID = reader.ReadUInt64("parentID", 0);

        GameObject* newGO = scene->CreateGameObject(goName);

        fileIDToNewGameObject[fileGoID] = newGO;
        if (fileParentID != 0)
        {
            parentLinks.push_back({ newGO, fileParentID });
        }

        if (!rootObject) rootObject = newGO;

        newGO->SetTag(reader.ReadString("tag", "Untagged"));
        newGO->SetActive(reader.ReadBool("active", true));

        if (reader.BeginObject("transform"))
        {
            Transform* tf = newGO->GetTransform();
            uint64_t fileTfID = reader.ReadUInt64("id", 0);

            if (fileTfID != 0) fileIDToNewComponent[fileTfID] = tf;

            DeserializeComponentProperties(reader, tf, fixupList);
            reader.EndObject();
        }

        reader.BeginArray("components");
        while (reader.NextArrayItem())
        {
            std::string typeName = reader.ReadString("typeName", "");
            uint64_t fileCompID = reader.ReadUInt64("id", 0);

            if (!typeName.empty())
            {
                Component* newComp = newGO->AddComponent(typeName);
                if (newComp)
                {
                    if (fileCompID != 0) fileIDToNewComponent[fileCompID] = newComp;
                    DeserializeComponentProperties(reader, newComp, fixupList);
                }
            }
            reader.EndArrayItem();
        }
        reader.EndArray(); 

        reader.EndArrayItem();
    }
    reader.EndArray(); 

    for (auto& link : parentLinks)
    {
        GameObject* child = link.first;
        uint64_t parentFileID = link.second;

        if (fileIDToNewGameObject.find(parentFileID) != fileIDToNewGameObject.end())
        {
            GameObject* parent = fileIDToNewGameObject[parentFileID];
            child->GetTransform()->SetParent(parent->GetTransform());
        }
    }

    for (const auto& task : fixupList)
    {
        uint64_t targetFileID = task.targetID;
        Component* targetPtr = nullptr;

        if (targetFileID != 0)
        {
            auto it = fileIDToNewComponent.find(targetFileID);
            if (it != fileIDToNewComponent.end())
            {
                targetPtr = it->second;
            }
        }
        task.property.m_setter(task.targetObject, &targetPtr);
    }

    return rootObject;
}