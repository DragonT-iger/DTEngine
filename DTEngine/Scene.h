#pragma once


#include <vector>
#include <memory>
#include <string>


class GameObject;

class Scene
{

    explicit Scene(const std::string& name) : m_name(name) {};
    virtual ~Scene() = default;

    GameObject* FindGameObject(std::string name);
    void Destroy(GameObject* object);

    GameObject* CreateGameObject(const std::string& name = "GameObject");
    void Awake();
    void Start();
    void Update(float deltaTime);
    void FixedUpdate(float fixedDelta);
    void LateUpdate(float deltaTime);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }
private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::string m_name = "Scene";



    // 지연처리
    bool                                            m_isIterating = false;
    std::vector<std::unique_ptr<GameObject>>        m_pendingAdd;
    std::vector<GameObject*>                        m_pendingDestroy;

    enum class ScenePhase { None, Awake, Start, Update, FixedUpdate, LateUpdate };
    ScenePhase m_phase = ScenePhase::None;
    void FlushPending();



};

