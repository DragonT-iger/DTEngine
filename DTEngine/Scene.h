#pragma once


#include <vector>
#include <memory>
#include <string>


#include "IResource.h"


class GameObject;

class Scene : public IResource
{

public:
    explicit Scene(const std::string& name);
    virtual ~Scene();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;

    GameObject* FindGameObject(std::string name);
    void Destroy(GameObject* object);

    virtual void Initialize() {} // 게임오브젝트 등 처음에 배치되어 있는거 초기화 해주는 함수

    // 엔진 전용 public 함수들

    GameObject* CreateGameObject(const std::string& name = "GameObject");

        
    void Awake();
    void Start();
    void Update(float deltaTime);
    void FixedUpdate(float fixedDelta);
    void LateUpdate(float deltaTime);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::string m_name;



    // 지연처리
    bool                                            m_isIterating = false;
    std::vector<std::unique_ptr<GameObject>>        m_pendingAdd;
    std::vector<GameObject*>                        m_pendingDestroy;

    enum class ScenePhase { None, Awake, Start, Update, FixedUpdate, LateUpdate };
    ScenePhase m_phase = ScenePhase::None;
    void FlushPending();



};

