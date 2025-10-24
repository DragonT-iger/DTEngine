#pragma once
#include "Singleton.h"
#include <string>
#include <unordered_map>
#include <functional>

class Component; 

using ComponentCreator = std::function<Component*()>;

class ComponentFactory : public Singleton<ComponentFactory>
{
public:
    void Register(const std::string& typeName, ComponentCreator creator) {
        m_creators[typeName] = creator;
    }

    Component* Create(const std::string& typeName) {
        auto it = m_creators.find(typeName);
        if (it != m_creators.end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    std::unordered_map<std::uint64_t, ComponentCreator> m_creators;
};