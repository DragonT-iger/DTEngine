#include "pch.h"
#include "SerializationUtils.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "AssetDatabase.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Material.h"
#include "TilemapData.h"
#include "Prefab.h"

void ReadPropertyRecursive(JsonReader& reader, const PropertyInfo& prop, void* base, std::vector<FixupTask>& fixupList)
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

    // ---------- Component ----------//
    else if (ReflectionDatabase::Instance().IsComponentPointer(type))
    {
        uint64_t id = reader.ReadUInt64(name);
        fixupList.push_back({ base, prop, id });
    }
    else if (type == typeid(GameObject*)) {
        uint64_t id = reader.ReadUInt64(name);
        fixupList.push_back({ base, prop, id });
    }
	// --------- IResource ---------//

    else if (type == typeid(Texture*)) {
        uint64_t id = reader.ReadUInt64(name);

        std::string path = AssetDatabase::Instance().GetPathFromID(id);

        if (!path.empty()) {
            Texture* tex = ResourceManager::Instance().Load<Texture>(path);

            prop.m_setter(base, &tex);
        }
        else {
            Texture* nullTex = nullptr;
            prop.m_setter(base, &nullTex);
        }
    }

    else if (type == typeid(TilemapData*)) {
        uint64_t id = reader.ReadUInt64(name);

        std::string path = AssetDatabase::Instance().GetPathFromID(id);

        if (!path.empty()) {
            TilemapData* data = ResourceManager::Instance().Load<TilemapData>(path);
            prop.m_setter(base, &data);
        }
        else {
            TilemapData* nullData = nullptr;
            prop.m_setter(base, &nullData);
        }
	}

    else if(type == typeid(Prefab*))
    {
		uint64_t id = reader.ReadUInt64(name);
        std::string path = AssetDatabase::Instance().GetPathFromID(id);
        if (!path.empty()) {
            Prefab* prefab = ResourceManager::Instance().Load<Prefab>(path);
            prop.m_setter(base, &prefab);
        }
        else {
            Prefab* nullPrefab = nullptr;
            prop.m_setter(base, &nullPrefab);
        }
	}





    else if (type == typeid(std::wstring))
    {
        std::string utf8Str = reader.ReadString(prop.m_name.c_str(), "");

        if (!utf8Str.empty())
        {
            int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), NULL, 0);

            std::wstring wStr;
            wStr.resize(size_needed);
            MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), &wStr[0], size_needed);

            prop.m_setter(base, &wStr);
        }
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
			std::cout << "Warning: Unknown property type during deserialization: " << type.name() << std::endl;
        }
    }
}

void DeserializeComponentProperties(JsonReader& reader, Component* comp, std::vector<FixupTask>& fixupList)
{
    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(comp->_GetTypeName());
    if (!info) return;

    for (const PropertyInfo& prop : info->m_properties)
    {
        ReadPropertyRecursive(reader, prop, comp, fixupList);
    }

    if (MeshRenderer* mr = dynamic_cast<MeshRenderer*>(comp))
    {
        mr->LoadInstanceData(reader);
    }
}

void CopyComponentValues(Component* src, Component* dst)
{
    if (!src || !dst || std::string(src->_GetTypeName()) != dst->_GetTypeName()) return;


    if (auto* srcTf = dynamic_cast<Transform*>(src))
    {
        auto* dstTf = dynamic_cast<Transform*>(dst);

        dstTf->SetPosition(srcTf->GetPosition());
        dstTf->SetRotationQuat(srcTf->GetRotationQuat());
        dstTf->SetScale(srcTf->GetScale());
        return;
    }




    const ClassInfo* info = ReflectionDatabase::Instance().GetClassInfomation(src->_GetTypeName());
    if (!info) return;

    for (const PropertyInfo& prop : info->m_properties)
    {
        void* srcValuePtr = prop.m_getter(src);
        prop.m_setter(dst, srcValuePtr);
    }


    if (MeshRenderer* srcMR = dynamic_cast<MeshRenderer*>(src))
    {
        MeshRenderer* dstMR = dynamic_cast<MeshRenderer*>(dst);

        if (dstMR) {
            if (srcMR->IsMaterialInstanced())
            {
                Material* clonedMat = srcMR->GetSharedMaterial()->Clone();

                dstMR->SetMaterialInstance(clonedMat);
            }
        }
        
        
    }
}
