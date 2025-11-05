#pragma once


#include <vector>
#include <memory>
#include <string>


#include "IResource.h"


class GameObject;
class Camera;

class Scene : public IResource
{

public:
	Scene(); // 엔진에서 경로 받아서 그 씬의 이름으로 씬 생성할 때 사용 이름이 없는 씬을 만드는건 load가 불가능함 당연히
    explicit Scene(const std::string& name);
    virtual ~Scene();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& solutionPath) override;
	void Unload() override;

    GameObject* FindGameObject(std::string name);
    void Destroy(GameObject* object);

    // 엔진 전용 public 함수들

    GameObject* CreateGameObject(const std::string& name = "GameObject");

        
    void Awake();
    void Start();
    void Update(float deltaTime);
    void FixedUpdate(float fixedDelta);
    void LateUpdate(float deltaTime);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

    Camera* GetMainCamera();
    void SetMainCamera(Camera* mainCamera);

    const std::string& GetName() const { return m_name; }

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::string m_name;

    Camera* m_mainCamera;



    // 지연처리
    bool                                            m_isIterating = false;
    std::vector<std::unique_ptr<GameObject>>        m_pendingAdd;
    std::vector<GameObject*>                        m_pendingDestroy;

    enum class ScenePhase { None, Awake, Start, Update, FixedUpdate, LateUpdate };
    ScenePhase m_phase = ScenePhase::None;
    void FlushPending();



};

