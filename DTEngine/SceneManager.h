#pragma once
#include "Singleton.h"
#include <memory>
#include <string>
#include <unordered_map>

class Scene;

class SceneManager : public Singleton<SceneManager>
{
    friend class Singleton<SceneManager>;

public:
    Scene*  GetActiveScene() const;

    // 씬 등록 (소유권 이전)
    void    RegisterScene(const std::string& name, std::unique_ptr<Scene> scene);

    void    LoadScene(const std::string& name);

    void    ProcessSceneChange();

private:

    Scene* m_active = nullptr; 
    std::string m_nextName;    
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
};
