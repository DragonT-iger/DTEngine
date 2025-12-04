#include "pch.h"
#include "SerializationUtils.h"
#include "Transform.h"
#include "MeshRenderer.h"

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
