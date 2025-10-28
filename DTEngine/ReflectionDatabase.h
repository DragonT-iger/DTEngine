#pragma once
#include "Singleton.h" 
#include <unordered_map>
#include <string>
#include <typeindex>
#include <vector>

struct PropertyInfo
{
    std::string m_name;
    std::type_index m_type;
    size_t m_offset;
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
    //void RegisterClass(const char* className); // <-- window.h ¸ÅÅ©·Î ÀÌ½´
    void RegisterDTCLASS(const char* className);

    void RegisterDTPROPERTY(const char* className, const char* propName,
        const std::type_info& type, size_t offset);
    const ClassInfo* GetClassInfo(const std::string& className) const;

private:
    ReflectionDatabase();
    ~ReflectionDatabase();

    std::unordered_map<std::string, ClassInfo> m_classes;
};