#pragma once
#include "Singleton.h" 
#include <unordered_map>
#include <string>
#include <typeindex>
#include <vector>
#include <functional>

class Component;

struct PropertyInfo
{
    std::string m_name;
    std::type_index m_type; 
    std::function<void* (void*)> m_getter;
    std::function<void(void*, void*)> m_setter;
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
        const std::type_info& type, std::function<void* (void*)> getter , std::function<void(void*, void*)> setter);
    const ClassInfo* GetClassInfomation(const std::string& className) const;

private:
    ReflectionDatabase();
    ~ReflectionDatabase();

    std::unordered_map<std::string, ClassInfo> m_classes;
};