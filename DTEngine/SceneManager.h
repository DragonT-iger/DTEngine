#pragma once
#include "Singleton.h"
#include <memory>
#include <string>
#include <unordered_map>

class Scene;

class SceneManager : public Singleton<SceneManager>
{

public:
    Scene*  GetActiveScene() const;

    void    RegisterScene(const std::string& name, Scene* scene);

    void    LoadScene(const std::string& name);

    void    ProcessSceneChange();

private:

    Scene* m_active = nullptr; 
    std::string m_nextName;    
    std::unordered_map<std::string, Scene*> m_scenes;
};
