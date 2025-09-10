#pragma once
class Scene // 아직 사용하지 않음
{

    // 라이프 사이클 로직
    void Awake();
    void Start();
    void Update(float deltaTime);
    void FixedUpdate(float fixedDeltaTime);
    void LateUpdate(float deltaTime);


    // ON / OFF
    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }



public:
    //std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    bool m_active = true;
};

