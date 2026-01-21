#pragma once
#include "IResource.h"
#include <string>

class GameObject;

class Prefab : public IResource
{
public:
    virtual ~Prefab() = default;

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    GameObject* Instantiate();

private:
    std::string m_jsonString; 
};