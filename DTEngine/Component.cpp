#include "pch.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ReflectionDatabase.h"
#include "Transform.h"
#include "GameObject.h"

void Component::Destroy(GameObject* gameobject) {
	Scene* curScene = SceneManager::Instance().GetActiveScene();
	curScene->Destroy(gameobject);
}

void Component::Instantiate(std::string name)
{
	Scene* curScene = SceneManager::Instance().GetActiveScene();
	curScene->CreateGameObject(name);
}

Transform* Component::GetTransform() {
    return this->_GetOwner()->GetTransform();
}

static void WritePropertyRecursive(JsonWriter& writer, const std::type_index& type, const char* name, void* data)
{

    if (type == typeid(int)) {
        writer.Write(name, *static_cast<int*>(data));
    }
    else if (type == typeid(float)) {
        writer.Write(name, *static_cast<float*>(data));
    }
    else if (type == typeid(uint64_t)) {
        writer.Write(name, *static_cast<uint64_t*>(data));
    }
    else if (type == typeid(std::string)) {
        writer.Write(name, *static_cast<std::string*>(data));
	}
    else if (type == typeid(bool)) {
        writer.Write(name, *static_cast<bool*>(data));
	}

    

    //component 타입

    else if (type == typeid(Transform*)) { 
        Component* ptr = *static_cast<Component**>(data);
        uint64_t id = (ptr) ? ptr->_GetID() : 0;
        writer.Write(name, id);
    }




    else
    {
        const ClassInfo* structInfo = ReflectionDatabase::Instance().GetClassInfomation(type.name());
        if (structInfo)
        {
            writer.BeginObject(name);
            for (const PropertyInfo& structProp : structInfo->m_properties)
            {
                void* memberData = structProp.m_getter(data);
                WritePropertyRecursive(
                    writer,
                    structProp.m_type,
                    structProp.m_name.c_str(),
                    memberData
                );
            }
            writer.EndObject();
        }
        else {
            std::cout << "Warning: No serializer for type " << type.name() << std::endl;
        }
    }
}

void Component::Serialize(JsonWriter& writer) const
{
    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(_GetTypeName());
    if (!info) return;

    for (const PropertyInfo& prop : info->m_properties)
    {
        void* valuePtr = prop.m_getter(const_cast<Component*>(this));

        WritePropertyRecursive(
            writer,
            prop.m_type,
            prop.m_name.c_str(),
            valuePtr
        );
    }
}
