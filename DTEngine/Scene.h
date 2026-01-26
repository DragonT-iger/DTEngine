#pragma once


#include <vector>
#include <memory>
#include <string>


#include "IResource.h"


class GameObject;
class Camera;
class Texture;
class RenderTexture;

struct DeltaTime // 배속용. 
{
    float rawTime; // 배속 적용 x.
    float scaledTime; // 배속 적용. ( rawTime * m_timeScale )
};

struct Ray // picking용
{
    Vector3 origin;
    Vector3 direction;
};

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

    GameObject* FindGameObjectByID(uint64_t id);

    void Enter();
    void Exit();

    // 엔진 전용 public 함수들

    GameObject* CreateGameObject(const std::string& name = "GameObject");
    //GameObject* CreateUIImage(const std::string& name = "UIImage");
    //GameObject* CreateUIButton(const std::string& name = "UIButton");
    //GameObject* CreateUISlider(const std::string& name = "UISlider");
    void AddGameObject(std::unique_ptr<GameObject> gameObject);

    // Undo Redo 호환용 내부 커멘드 (소유권을 넘긴다) 왜냐면 ID (포인터)가 깨져서 완전 삭제 대신 소유권을 커멘드에 넘길 필요가 있음
    // 굳이 pending 에 넣을 필요는 없는거 같음 

    void _Internal_AddGameObject(std::unique_ptr<GameObject> go);
    std::unique_ptr<GameObject> _Internal_RemoveGameObject(GameObject* go);

    void Render(Camera* camera, RenderTexture* renderTarget = nullptr, bool renderUI = false);
    void RenderShadows();

    void Awake();
    void Start();
    void Update(float deltaTime);
    void FixedUpdate(float fixedDelta);
    void LateUpdate(float deltaTime);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

    Camera* GetMainCamera();
    void SetMainCamera(Camera* mainCamera);

    Camera* GetEditorCamera();
	void SetEditorCamera(Camera* editorCamera);

    const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

    // 배속용.
    float GetTimeScale() const { return m_timeScale; }
    void SetTimeScale(float timeScale) { m_timeScale = timeScale; }

    // 피킹용.
    bool Raycast(const Ray& ray, GameObject*& outHit, float& outT);
    bool Raycast2(const Ray& rayWorld, GameObject*& outHit, float& outTWorld); // 이게 삼각형 검사까지 추가된거.

    void Clear();

    void RayUpdate(float deltaTime);

private:
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
    std::string m_name;

    Camera* m_mainCamera;
	Camera* m_editorCamera;

    inline static float m_timeScale = 1.0f; // 배속용.


    // 지연처리
    bool                                            m_isIterating = false;
    std::vector<std::unique_ptr<GameObject>>        m_pendingAdd;
    std::vector<GameObject*>                        m_pendingDestroy;

    enum class ScenePhase { None, Awake, Start, Update, FixedUpdate, LateUpdate };
    ScenePhase m_phase = ScenePhase::None;
    void FlushPending();



    
    GameObject* m_hitObject;



};

