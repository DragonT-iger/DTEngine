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

    void    RegisterScene(const std::string& filePath);

    void    LoadScene(const std::string& name);

    bool    ProcessSceneChange();

    bool    BackupActiveScene();   
    bool    RestoreActiveScene();  


	float  GetUnscaledDeltaTime() const { return m_unScaledDeltaTime; }
	void  SetUnscaledDeltaTime(float dt) { m_unScaledDeltaTime = dt; }

private:

    Scene* m_active = nullptr; 
    std::string m_nextName;    
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;


    const std::string m_backupPath = "Scenes/PlayMode_Backup.scene";
    std::string m_originalSceneName; 

    // 귀차나
	float m_unScaledDeltaTime = 0.0f;

};
