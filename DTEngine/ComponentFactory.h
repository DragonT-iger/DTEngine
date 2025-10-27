#pragma once
#include "Singleton.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

// ��������� �������� String���� ���ϴ°� ������ ���� �ϴ��� ū �������ٸ� �ѱ��
// idManager�� ����� �̰͵�?
// Create�Ҷ� �������� ��� ������ �ִٰ� move�� Scene���� �������� �ѱ濹��

class Component; 

using ComponentCreator = std::function<std::unique_ptr<Component>()>;

class ComponentFactory : public Singleton<ComponentFactory>
{
public:
    void Register(const std::string& typeName, ComponentCreator creator) {
        m_creators[typeName] = creator;
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