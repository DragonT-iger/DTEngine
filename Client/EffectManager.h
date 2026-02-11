#pragma once
#include "Singleton.h"

#include <string>
#include <vector>
#include <map>
#include <functional>

class Prefab;
class GameObject;

class EffectManager :public Singleton<EffectManager>
{
	friend class Singleton<EffectManager>;

public:
    bool Initialize();

    void PlayEffect(const std::string& key, GameObject* target);
    void PlayEffect_Targeting(const std::string& key, GameObject* targe, GameObject* targett);


private:
    EffectManager() = default;
    ~EffectManager();


    std::map<std::string, Prefab*> m_EffectMap;
};




