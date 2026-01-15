#pragma once
#include "Singleton.h" 
#include <unordered_map>
#include <string>
#include <typeindex>
#include <vector>
#include <functional>
#include <unordered_set>

class Component;

struct PropertyInfo
{
    std::string m_name;
    std::type_index m_type; 
    std::function<void* (void*)> m_getter;
    std::function<void(void*, void*)> m_setter;

    std::function<void* (GameObject*)> m_componentFinder;
    ////enum 인 경우 특수처리
    //std::vector<std::string> m_enumNames;
};

struct ClassInfo
{
    std::string m_name;
    std::vector<PropertyInfo> m_properties;
};


class ReflectionDatabase : public Singleton<ReflectionDatabase>
{
    friend class Singleton<ReflectionDatabase>;

public:
    //void RegisterClass(const char* className); // <-- Windows.h 
    void RegisterDTGENERATED_BODY(const char* className);

    void RegisterDTPROPERTY(const char* className, const char* propName,
        const std::type_info& type, std::function<void* (void*)> getter , std::function<void(void*, void*)> setter, 
        const std::vector<std::string>& enumNames = {}, std::function<void* (GameObject*)> componentSearcher = nullptr);
    const ClassInfo* GetClassInfomation(const std::string& className) const;

    void Clear() {
        m_classes.clear(); 
    }

    bool IsComponentPointer(std::type_index type) const {
        return m_componentPtrTypes.find(type) != m_componentPtrTypes.end();
    }

    void RegisterComponentPointerType(std::type_index type) {
        m_componentPtrTypes.insert(type);
    }

private:

    ReflectionDatabase();
    ~ReflectionDatabase();

    std::unordered_map<std::string, ClassInfo> m_classes;
    std::unordered_set<std::type_index> m_componentPtrTypes;
};