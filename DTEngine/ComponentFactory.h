#pragma once
#include "Singleton.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>

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
        std::cout << typeName << " 등록" << std::endl;
    }

    std::unique_ptr<Component> Create(const std::string& typeName) {
        auto it = m_creators.find(typeName);
        if (it != m_creators.end()) {
            return it->second();
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, ComponentCreator> m_creators;
};