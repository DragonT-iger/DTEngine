#include "pch.h"
#include "Component.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ReflectionDatabase.h"
#include "Transform.h"
#include "GameObject.h"
#include "Behaviour.h"
#include "Texture.h"
#include "Camera.h"
#include "TilemapData.h"
#include "Prefab.h"


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

const Transform* Component::GetTransform() const {
    return this->_GetOwner()->GetTransform();
}

std::string Component::GetName() const {
    return this->_GetOwner()->GetName();
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
    else if(type == typeid(double))
    {
        writer.Write(name, *static_cast<double*>(data));
	}
    else if(type == typeid(wchar_t))
    {
        writer.Write(name, *static_cast<wchar_t*>(data));
	}
    else if (type == typeid(std::wstring))
    {
        std::wstring temp = *static_cast<std::wstring*>(data);

        if (temp.empty())
        {
            writer.Write(name, std::string(""));
        }
        else
        {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0, NULL, NULL);

            std::string strToSave(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, &temp[0], (int)temp.size(), &strToSave[0], size_needed, NULL, NULL);

            writer.Write(name, strToSave);
        }
    }

    

    //------------ COMPONENT ----------//

    
    else if (ReflectionDatabase::Instance().IsComponentPointer(type))
    {
        Component* ptr = *static_cast<Component**>(data);
        uint64_t id = (ptr) ? ptr->_GetID() : 0;
        writer.Write(name, id);
    }
    else if (type == typeid(GameObject*)) { // 이건 컴포넌트가 아니라 해줘야돼
        GameObject* go = *static_cast<GameObject**>(data);
        uint64_t id = (go) ? go->_GetID() : 0;
        writer.Write(name, id);
    }
    //else if (type == typeid(Transform*)) { 
    //    Component* ptr = *static_cast<Component**>(data);
    //    uint64_t id = (ptr) ? ptr->_GetID() : 0;
    //    writer.Write(name, id);
    //}
    
    //else if (type == typeid(Camera*)) {
    //    Camera* cam = *static_cast<Camera**>(data);
    //    uint64_t id = (cam) ? cam->_GetID() : 0;
    //    writer.Write(name, id);
    //}


    //--------------- IRESOURCE -----------------// IResource는 엔진 단이므로 그냥 노가다 하셈 아쉬운거임

	else if (type == typeid(Texture*)) { // IResource 는 Meta가 id를 가지고 있음
        Texture* tex = *static_cast<Texture**>(data);
        uint64_t id = (tex) ? tex->GetMeta().guid : 0;
        writer.Write(name, id);
	}

    else if(type == typeid(TilemapData*))
    {
        TilemapData* tilemapData = *static_cast<TilemapData**>(data);
        uint64_t id = (tilemapData) ? tilemapData->GetMeta().guid : 0;
        writer.Write(name, id);
	}

    else if (type == typeid(Prefab*))
    {
        Prefab* prefab = *static_cast<Prefab**>(data);
        uint64_t id = (prefab) ? prefab->GetMeta().guid : 0;
        writer.Write(name, id);
    }

    //크아아아아아악




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

    if (const Behaviour* behaviour = dynamic_cast<const Behaviour*>(this))
    {
        writer.Write("Active", behaviour->IsActive());
    }

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
