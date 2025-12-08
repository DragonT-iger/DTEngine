#pragma once
#include "Singleton.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>
#include <algorithm>

// 장기적으로 봤을때는 String으로 비교하는건 성능이 느림 일단은 큰 문제없다면 넘기기
// idManager로 만들까 이것도?
// Create할때 소유권을 잠시 가지고 있다가 move로 Scene에게 소유권을 넘길예정

class Component; 

using ComponentCreator = std::function<std::unique_ptr<Component>()>;

class ComponentFactory : public Singleton<ComponentFactory>
{
public:
    void Register(const std::string& typeName, ComponentCreator creator) {
        m_creators[typeName] = creator;
        //std::cout << typeName << " register ComponentFactory" << std::endl;
    }

    std::unique_ptr<Component> Create(const std::string& typeName) {
        auto it = m_creators.find(typeName);
        if (it != m_creators.end()) {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::string> GetAllRegisteredTypeNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_creators.size()); 

        for (const auto& pair : m_creators)
        {
            names.push_back(pair.first);
        }

        std::sort(names.begin(), names.end());

        return names;
    }

private:
    std::unordered_map<std::string, ComponentCreator> m_creators;
};