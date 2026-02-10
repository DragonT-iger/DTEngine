#include "pch.h"


#include <filesystem>
#include <iostream>


#include "Scene.h"
#include "GameObject.h"
#include "IDManager.h"
#include "ResourceManager.h"
#include "DX11Renderer.h"
#include "Light.h"
#include "Behaviour.h"
#include "SerializationUtils.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Image.h"
#include "Mesh.h"
#include "ShadowMap.h"

#include "RenderKey.h"

#include "Skeletal.h"
#include "UIButton.h"
#include "UISlider.h"
//#include "RectTransform.h"
#include "Animatior.h"
#include "FSMController.h"
#include "InputManager.h"

#include "Text.h"
#include "Effect.h"
#include "Rigid.h"

#include "Levitating.h"
#include "outline.h"
#include "SpriteEffect.h"
#include "Fog.h"

#include "SoundManager.h"
#include "MultiSpriteController .h"

GameObject* Scene::CreateGameObject(const std::string& name)
{
    auto go = std::make_unique<GameObject>(name);
    GameObject* raw = go.get();
	raw->_SetID(IDManager::Instance().GetNewUniqueID());

    if (m_isIterating)
        m_pendingAdd.emplace_back(std::move(go));
    else
        m_gameObjects.emplace_back(std::move(go));
    return raw;
}

//GameObject* Scene::CreateUIImage(const std::string& name)
//{
//    GameObject* go = CreateGameObject(name);
//    if (go && !go->GetComponent<Image>())
//    {
//        go->AddComponent<Image>();
//    }
//    return go;
//}
//
//GameObject* Scene::CreateUIButton(const std::string& name)
//{
//    GameObject* go = CreateUIImage(name);
//    if (go && !go->GetComponent<UIButton>())
//    {
//        go->AddComponent<UIButton>();
//    }
//    return go;
//}
//
//GameObject* Scene::CreateUISlider(const std::string& name)
//{
//    GameObject* go = CreateUIImage(name);
//    if (go && !go->GetComponent<UISlider>())
//    {
//        go->AddComponent<UISlider>();
//    }
//
//    if (go)
//    {
//        Transform* tf = go->GetTransform();
//        bool hasHandle = false;
//        if (tf)
//        {
//            for (Transform* child : tf->GetChildren())
//            {
//                if (child && child->_GetOwner()->GetName() == "Handle")
//                {
//                    hasHandle = true;
//                    break;
//                }
//            }
//        }
//
//        if (!hasHandle)
//        {
//            GameObject* handle = CreateUIImage("Handle");
//            handle->GetTransform()->SetParent(tf);
//
//            if (auto* handleTransform = handle->GetTransform())
//            {
//                handleTransform->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
//                handleTransform->SetScale(Vector3(24.0f, 24.0f, 1.0f));
//            }
//
//            if (auto* image = handle->GetComponent<Image>())
//            {
//                image->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
//                if (auto* parentImage = go->GetComponent<Image>())
//                {
//                    image->SetOrderInLayer(parentImage->GetOrderInLayer() + 1);
//                }
//            }
//
//        }
//    }
//    return go;
//}

void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject)
{
    if (m_isIterating)
        m_pendingAdd.emplace_back(std::move(gameObject));
    else
        m_gameObjects.emplace_back(std::move(gameObject));
}

void Scene::_Internal_AddGameObject(std::unique_ptr<GameObject> go)
{
    GameObject* raw = go.get();
    m_gameObjects.push_back(std::move(go));

    if (m_phase == ScenePhase::Awake)
    {
        raw->Awake();
    }
    else if (m_phase != ScenePhase::None)
    {
        raw->Awake();
        raw->Start();
    }
}

std::unique_ptr<GameObject> Scene::_Internal_RemoveGameObject(GameObject* go)
{
    auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
        [go](const auto& p) { return p.get() == go; });

    if (it != m_gameObjects.end())
    {
        std::unique_ptr<GameObject> found = std::move(*it);
        m_gameObjects.erase(it);
        return found;
    }

    it = std::find_if(m_pendingAdd.begin(), m_pendingAdd.end(),
        [go](const auto& p) { return p.get() == go; });

    if (it != m_pendingAdd.end())
    {
        std::unique_ptr<GameObject> found = std::move(*it);
        m_pendingAdd.erase(it);
        return found;
    }

    return nullptr;
}



Scene::Scene() : m_name("Untitled Scene") {}

Scene::Scene(const std::string& name) : m_name(name) {}

Scene::~Scene() = default;

bool Scene::LoadFile(const std::string& fullPath)
{
    auto rd = JsonReader::LoadJson(fullPath);
    if (!rd) return false;
    JsonReader& reader = *rd;


    try
    {
        std::filesystem::path p(fullPath);
        m_name = p.stem().string(); 
    }
    catch (const std::exception& e)
    {
        std::cerr << "Warning: Could not parse scene name from path. " << e.what() << std::endl;
        
        m_name = "ErrorSceneName";
    }

    // --- Pass 1: 객체 생성, ID 매핑, 값 설정 ---


    std::unordered_map<uint64_t, Component*> idToComponentMap;
    std::unordered_map<uint64_t, GameObject*> idToGameObjectMap;

    std::vector<FixupTask> fixupList;

    const int n = reader.BeginArray("gameObjects");
    while (reader.NextArrayItem())
    {
        std::string goName = reader.ReadString("name", "GameObject");
        GameObject* go = CreateGameObject(goName);

		//std::cout << "Deserializing GameObject: " << goName << std::endl;

        uint64_t go_id = reader.ReadUInt64("id", 0);
        go->_SetID(go_id);


        idToGameObjectMap[go_id] = go;


        go->SetTag(reader.ReadString("tag", "Untagged"));
        go->SetActive(reader.ReadBool("active", true));


        if (reader.BeginObject("transform"))
        {
            Transform* tf = go->GetTransform();
            uint64_t tf_id = reader.ReadUInt64("id", 0);
            tf->_SetID(tf_id);

            idToComponentMap[tf_id] = tf; 

			//std::cout << "Deserializing Transform ID: " << tf_id << " for GameObject: " << goName << std::endl;

            DeserializeComponentProperties(reader, tf, fixupList);

            reader.EndObject();
        }

        const int compCount = reader.BeginArray("components");
        while (reader.NextArrayItem())
        {
            std::string typeName = reader.ReadString("typeName", "");
            uint64_t comp_id = reader.ReadUInt64("id", 0);

            if (typeName.empty() || comp_id == 0) continue;

            Component* newComp = go->AddComponent(typeName);
            
            if (!newComp)
            {
                std::cout << "Warning: Failed to create component '" << typeName
                    << "' on GameObject '" << goName << "'" << std::endl;
                continue;
            }


            newComp->_SetID(comp_id);
            idToComponentMap[comp_id] = newComp;

            if (auto* behaviour = dynamic_cast<Behaviour*>(newComp))
            {
                bool isActive = reader.ReadBool("Active", true);
                behaviour->SetActive(isActive);
            }

            DeserializeComponentProperties(reader, newComp, fixupList);
            reader.EndArrayItem();
        }
        reader.EndArray();

		reader.EndArrayItem();
    }
    reader.EndArray();

    // --- Pass 2 ---

    for (const auto& task : fixupList)
    {
        uint64_t targetID = task.targetID;
        if (targetID == 0) continue; 

        if (task.property.m_type == typeid(GameObject*))
        {
            auto it = idToGameObjectMap.find(targetID);
            GameObject* foundGO = (it != idToGameObjectMap.end()) ? it->second : nullptr;

            if (foundGO) {
                task.property.m_setter(task.targetObject, &foundGO);
            }
            else {
                std::cout << "Warning: Failed to find GameObject ID " << targetID << std::endl;
            }
        }
        else
        {
            auto it = idToComponentMap.find(targetID);
            Component* foundComp = (it != idToComponentMap.end()) ? it->second : nullptr;

            if (foundComp) {
                task.property.m_setter(task.targetObject, &foundComp);
            }
            else {
                std::cout << "Warning: Failed to find Component ID " << targetID << std::endl;
            }
        }
    }

    return true;
}

bool Scene::SaveFile(const std::string& solutionPath)
{
    JsonWriter writer;

    std::string assetPath = ResourceManager::Instance().ResolveFullPath(solutionPath);

    writer.BeginArray("gameObjects");

    for (auto& go : m_gameObjects)
    {
        if (!go) continue;

        if (go->HasFlag(GameObject::Flags::DontSave))
            continue;

        writer.NextArrayItem();

        writer.Write("id", go->_GetID());
        writer.Write("name", go->GetName());
        writer.Write("tag", go->GetTag());
        writer.Write("active", go->IsActive());

        if (auto* tf = go->GetTransform())
        {
            writer.BeginObject("transform");

            writer.Write("id", tf->_GetID());

            tf->Serialize(writer);

            writer.EndObject();
        }

        writer.BeginArray("components");
        for (auto& comp : go->_GetComponents())
        {
            if (!comp) continue;

            writer.NextArrayItem();

            writer.Write("typeName", comp->_GetTypeName());
            
            writer.Write("id", comp->_GetID());

            comp->Serialize(writer);

            //if (auto* mr = dynamic_cast<MeshRenderer*>(comp.get()))
            //{
            //    mr->SaveInstanceData(writer);
            //}

            writer.EndArrayItem();
        }
        writer.EndArray();

        writer.EndArrayItem();
    }
    writer.EndArray();

    return writer.SaveFile(assetPath);
}

void Scene::Unload()
{

}

GameObject* Scene::FindGameObject(std::string name)
{
    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); it++) {
        if ((*it)->GetName() == name) {
            return (*it).get();
        }
    }

    return nullptr;
}
void Scene::Destroy(GameObject* object)
{
    auto it = std::find(m_pendingDestroy.begin(), m_pendingDestroy.end(), object);
    if (it != m_pendingDestroy.end()) return;


    m_pendingDestroy.push_back(object);


    Transform* tf = object->GetTransform();
    if (tf)
    {
        const auto& children = tf->GetChildren();
        for (Transform* child : children)
        {
            Destroy(child->_GetOwner());
        }
    }
}


void Scene::Awake()
{

    m_phase = ScenePhase::Awake;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->Awake();
    m_isIterating = false;
    FlushPending();


}

void Scene::Start()
{
    m_phase = ScenePhase::Start;
    m_isIterating = true;
    for (auto& obj : m_gameObjects) obj->Start();
    m_isIterating = false;
    FlushPending();

    if (!m_bgmPlayed) m_bgmPlayed = SoundManager::Instance().PlaySceneBGM(m_name, true);    
}

void Scene::Update(float deltaTime)
{
    m_phase = ScenePhase::Update;
    m_isIterating = true;
    for (auto& obj : m_gameObjects)
    {
        obj->Update(deltaTime);
    }
    m_isIterating = false;
    FlushPending();
}

void Scene::FixedUpdate(float fixedDelta)
{
    m_phase = ScenePhase::FixedUpdate;

    m_isIterating = true;
    for (auto& obj : m_gameObjects)
    {
        obj->FixedUpdate(fixedDelta);
    }
    m_isIterating = false;
    FlushPending();

    //PhysicsManager::Instance().Step(fixedDelta);
}

void Scene::LateUpdate(float deltaTime)
{
    m_phase = ScenePhase::LateUpdate;
    m_isIterating = true;
    for (auto& obj : m_gameObjects)
    {
        obj->LateUpdate(deltaTime);
    }
    m_isIterating = false;
    FlushPending();

    
    if (m_mainCamera == nullptr) return;

    std::vector<Light*> temp;
    for (const auto& lights : Light::GetAllLights())
    {
        if (FindGameObject(lights->_GetOwner()->GetName()) != nullptr)
            temp.push_back(lights);
    }

    DX11Renderer::Instance().UpdateLights_CBUFFER(temp, m_mainCamera);

   // DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights() , m_mainCamera->GetComponent<Camera>());

    // 물리 업데이트
}

Camera* Scene::GetMainCamera()
{
    return m_mainCamera;
}

void Scene::SetMainCamera(Camera* mainCamera)
{
    m_mainCamera = mainCamera;
}

Camera* Scene::GetEditorCamera()
{
	return m_editorCamera;
}

void Scene::SetEditorCamera(Camera* editorCamera)
{
    m_editorCamera = editorCamera;
}


// 이건 쓰지말래
bool Scene::Raycast(const Ray& ray, GameObject*& outHit, float& outT)
{
    using namespace DirectX;

    outHit = nullptr;
    outT = FLT_MAX; // 거리 파라미터. 레이 방정식 P(t) = origin + direction * t 에서 t. 
    //일단 매우 큰 값으로 해두고, 작은 t가 나오면 갱신.

    XMVECTOR rayOrigin = XMLoadFloat3((const XMFLOAT3*)&ray.origin);
    XMVECTOR rayDir = XMLoadFloat3((const XMFLOAT3*)&ray.direction);

    bool hitAny = false; // 일단 뭔가 하나라도 맞았으면 true

    for (const auto& up : m_gameObjects)
    {
        GameObject* go = up.get();
        if (!go || !go->IsActiveInHierarchy()) continue;

        if (go->GetComponent<Image>()) continue;

        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        if (!mr || !mr->IsActive()) continue;

        Mesh* mesh = mr->GetMesh();
        if (!mesh) continue;

        // 로컬 AABB 가져오기
        const BoundingBox& localBox = mesh->GetLocalBoundingBox();

        // 월드 AABB 만들기
        Transform* tf = go->GetTransform();
        if (!tf) continue;

        const Matrix& world = tf->GetWorldMatrix();

        BoundingBox worldBox;
        {
            XMMATRIX W = XMLoadFloat4x4((const XMFLOAT4X4*)&world);
            localBox.Transform(worldBox, W); // 자동으로 월드 AABB 만들어줌.
        }

        // Ray랑 AABB 충돌 검사.
        float t = 0.0f;
        if (worldBox.Intersects(rayOrigin, rayDir, t)) 
        {
            if (t >= 0.0f && t < outT) // 작은 t가 나오면 최소값으로 갱신.
            {
                outT = t;
                outHit = go;
                hitAny = true;
            }
        }
    }

    return hitAny;
}

// moller-trumbore 알고리즘.
static bool IntersectsRayTriangle(
    const Vector3& rayO, const Vector3& rayD, // Ray를 로컬로 변환한 결과 넣어주기.
    const Vector3& v0, const Vector3& v1, const Vector3& v2,
    float& outT)
{
    const float EPS = 1e-6f;

    Vector3 e1 = v1 - v0;
    Vector3 e2 = v2 - v0;

    Vector3 p = rayD.Cross(e2);
    float det = e1.Dot(p);

    if (fabsf(det) < EPS) return false;

    float invDet = 1.0f / det;

    Vector3 tvec = rayO - v0;
    float u = tvec.Dot(p) * invDet;
    if (u < 0.0f || u > 1.0f) return false;

    Vector3 q = tvec.Cross(e1);
    float v = rayD.Dot(q) * invDet;
    if (v < 0.0f || (u + v) > 1.0f) return false;

    float t = e2.Dot(q) * invDet;
    if (t < 0.0f) return false;

    outT = t;
    return true;
}

// Ray를 로컬로 바꿔주는 함수
static Ray TransformRayToLocal(const Ray& worldRay, const Matrix& world)
{
    Matrix invW = SimpleMathHelper::Inverse(world);

    Vector3 o = Vector3::Transform(worldRay.origin, invW);
    Vector3 d = Vector3::TransformNormal(worldRay.direction, invW);
    d.Normalize();

    return Ray{ o, d };
}

bool Scene::Raycast2(const Ray& rayWorld, GameObject*& outHit, float& outTWorld)
{
    using namespace DirectX;

    outHit = nullptr;
    outTWorld = FLT_MAX;

    Ray rw = rayWorld;
    rw.direction.Normalize();

    XMVECTOR rayO = XMLoadFloat3((const XMFLOAT3*)&rw.origin);
    XMVECTOR rayD = XMLoadFloat3((const XMFLOAT3*)&rw.direction);

    bool hitAny = false;

    for (const auto& up : m_gameObjects)
    {
        GameObject* go = up.get();

        if (go->GetName() == "Skybox(temp)") continue; // 나중에 스카이박스 처리 바꿔주기.

        if (!go || !go->IsActiveInHierarchy()) continue;
        if (go->GetComponent<Image>()) continue;

        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        if (!mr || !mr->IsActive()) continue;

        Mesh* mesh = mr->GetMesh();
        if (!mesh) continue;

        Transform* tf = go->GetTransform();
        if (!tf) continue;

        const Matrix& world = tf->GetWorldMatrix();

        // 로컬 AABB -> 월드 AABB
        const BoundingBox& localBox = mesh->GetLocalBoundingBox();

        BoundingBox worldBox;
        {
            XMMATRIX W = XMLoadFloat4x4((const XMFLOAT4X4*)&world);
            localBox.Transform(worldBox, W);
        }

        // 월드 레이, 월드 AABB 충돌 검사. 
        float tAabb = 0.0f;
        if (!worldBox.Intersects(rayO, rayD, tAabb))
            continue;

        // 이미 더 가까운게 있으면 컷.
        if (tAabb > outTWorld)
            continue;

        // 여기까지 통과했으면 레이를 로컬로 바꿔서 삼각형 검사
        Ray rl = TransformRayToLocal(rw, world);

        const auto& pos = mesh->GetPositions();
        const auto& ind = mesh->GetIndices();
        if (pos.empty() || ind.size() < 3) continue;

        float bestThis = FLT_MAX;
        bool hitThis = false;

        // 삼각형 루프
        for (size_t i = 0; i + 2 < ind.size(); i += 3)
        {
            uint32_t i0 = ind[i + 0];
            uint32_t i1 = ind[i + 1];
            uint32_t i2 = ind[i + 2];

            if (i0 >= pos.size() || i1 >= pos.size() || i2 >= pos.size()) continue;

            float tLocal = 0.0f;
            if (!IntersectsRayTriangle(rl.origin, rl.direction, pos[i0], pos[i1], pos[i2], tLocal)) continue;

            // 로컬 히트포인트 -> 월드 변환 -> 월드 t로 변환
            Vector3 hitLocal = rl.origin + rl.direction * tLocal;
            Vector3 hitWorld = Vector3::Transform(hitLocal, world);

            //float tWorld = (hitWorld - rw.origin).Length();
            float tWorld = (hitWorld - rw.origin).Dot(rw.direction);
            if (tWorld < 0.0f) continue;

            if (tWorld < bestThis)
            {
                bestThis = tWorld;
                hitThis = true;
            }
        }

        if (hitThis && bestThis < outTWorld)
        {
            outTWorld = bestThis;
            outHit = go;
            hitAny = true;
        }
    }

    return hitAny;
}


void Scene::Clear()
{
    m_gameObjects.clear();     
    m_pendingAdd.clear();      
    m_pendingDestroy.clear();
    m_mainCamera = nullptr;
}

GameObject* Scene::FindGameObjectByID(uint64_t id)
{
    for (const auto& go : m_gameObjects)
    {
        if (go->_GetID() == id)
            return go.get();
    }
    return nullptr;
}

void Scene::Enter()
{
    for (const auto& go : m_gameObjects)
    {
        if (go->IsActive())
        {
            for (const auto& comp : go->_GetComponents())
            {
                if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
                {
                    if (mb->IsActive())
                    {
                        mb->OnEnable();
                    }
                }
            }
        }
    }
}

void Scene::Exit()
{
    m_bgmPlayed = false;

    for (const auto& go : m_gameObjects)
    {
        if (go->IsActive())
        {
            for (const auto& comp : go->_GetComponents())
            {
                if (auto* mb = dynamic_cast<MonoBehaviour*>(comp.get()))
                {
                    if (mb->IsActive())
                    {
                        mb->OnDisable();
                    }
                }
            }
        }
    }
}

void Scene::Render(Camera* camera, RenderTexture* renderTarget)
{
    if (!camera) return;



    float width = (float)DX11Renderer::Instance().GetWidth();
    float height = (float)DX11Renderer::Instance().GetHeight();

    if (renderTarget != nullptr)
    {
        width = (float)renderTarget->GetWidth();
        height = (float)renderTarget->GetHeight();
    }

    float ratio = width / height;
    camera->SetAspectRatio(ratio);

    DX11Renderer::Instance().ResetRenderState();
    DX11Renderer::Instance().ClearCache();

    camera->Bind();
    

    Sorter::Instance().SetCamParameters(camera);
    const Matrix& viewTM = camera->GetViewMatrix();
    const Matrix& projTM = camera->GetProjectionMatrix();

    DX11Renderer::Instance().UpdateFrame_CBUFFER(viewTM, projTM);
    DX11Renderer::Instance().BindGlobalResources();

    std::vector<GameObject*> opaqueQueue;
    std::vector<GameObject*> transparentQueue;
    std::vector<GameObject*> uiQueue;

    for (const auto& go : GetGameObjects())
    {
        if (!go || !go->IsActiveInHierarchy()) continue;

        Image* img = go->GetComponent<Image>();
        if (img && img->IsActive())
        {
            uiQueue.push_back(go.get());
            continue; 
        }


        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        if (!mr || !mr->IsActive()) continue;

        Material* mat = mr->GetSharedMaterial();
        if (!mat) mat = ResourceManager::Instance().Load<Material>("Materials/Error");
        if (!mat) continue;

        if (img) {
            uiQueue.push_back(go.get());
        }

        else if (mat->GetRenderMode() == RenderMode::Transparent)
        {
            transparentQueue.push_back(go.get());
        }
        else
        {
            opaqueQueue.push_back(go.get());
        }
    }

   
   

       // Sorter::Instance().CreateKeyOpaque(opaqueQueue);
        // std::vector<GameObject*>& SortedVector = Sorter::Instance().GetOpaqueVec();

        RenderOpaque(opaqueQueue);
        RenderOutline(opaqueQueue); 


       //Sorting 해야 함... effect 겹치는 거 때문에
       // 
       // Sorter::Instance().CreateKeyTransparent(transparentQueue);
      //  std::vector<GameObject*>& SortedVectorTrans = Sorter::Instance().GetTransVec();
         RenderTrans(transparentQueue, viewTM);
       // RenderTrans(SortedVectorTrans, viewTM);


         

}

void Scene::RenderOpaque(std::vector<GameObject*>& OpaqueVec)
{

    for (const auto& val : OpaqueVec)
    {

        MeshRenderer* mr = val->GetComponent<MeshRenderer>();
        Transform* tf = val->GetTransform();

        Material* mat = mr->GetSharedMaterial();
        if (!mat) mat = ResourceManager::Instance().Load<Material>("Materials/Error");

        Mesh* mesh = mr->GetMesh();
        if (!mesh || !mat) return;

        Effect* eff = val->GetComponent<Effect>();
        if (eff) eff->BindEP();

        if (Skeletal* sk = val->GetComponent<Skeletal>(); sk != nullptr)
        {
            DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(sk->GetFinalMatrix());
        }

        if (Rigid* rg = val->GetComponent<Rigid>(); rg != nullptr)
        {
            DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(rg->GetFinalTransforms());
        }

        if (Fog* fg = val->GetComponent<Fog>(); fg != nullptr)
        {
            fg->BindFog();
        }

        const Matrix& worldTM = tf->GetWorldMatrix();
        Matrix worldInvT = tf->GetWorldInverseTransposeMatrix();

        mat->BindPipeLine();
        mat->BindPerObject(worldTM, worldInvT);
        mesh->Bind();
        mesh->Draw();

    }
}

// 일단 예정은 Plane; Sprite Effect 정도임. 
// Plane은 Tile위에 Alpha Blending으로 


// Pre multiplied state로 기존 alpha blending statee를 변경시킬거임. 
// 
// Alpha Blending이 필요한 Object는 shader에서 alpha를 곱하는 형태로 처리 
// 


void Scene::RenderTrans(std::vector<GameObject*>& TransVec , const Matrix& cam)
{

    Matrix invRotation = cam;
    invRotation._41 = 0.0f; invRotation._42 = 0.0f; invRotation._43 = 0.0f;
    //invRotation = invRotation  //전치 한 번 더 하기 싫어서 그냥 안 함. 
   
    //


    auto DrawObject = [&](GameObject* go) 
        {
        MeshRenderer* mr = go->GetComponent<MeshRenderer>();
        Transform* tf = go->GetTransform();

        Material* mat = mr->GetSharedMaterial();
        if (!mat) mat = ResourceManager::Instance().Load<Material>("Materials/Error");

        Mesh* mesh = mr->GetMesh();
        if (!mesh || !mat) return;

        SpriteEffect* spt = go->GetComponent<SpriteEffect>();
        if (spt)
        {
            spt->SetCameraInverrse(invRotation);
            spt->BindWrapped();
        }

        const Matrix& worldTM = tf->GetWorldMatrix();
        Matrix worldInvT = tf->GetWorldInverseTransposeMatrix();

        mat->Bind(worldTM, worldInvT);
        mesh->Bind();
        mesh->Draw();
        };



    for (auto* go : TransVec)
    {
        DrawObject(go);
    }
}

// 걍 한 번에 Sorting을 해버리는 건 어떨까? 
// 뭐 나중에 궁금하면 해보자 .


void Scene::RenderOutline(std::vector<GameObject*>& OutlineVec)
{
    DX11Renderer::Instance().SetBlendMode(BlendMode::Opaque);
    DX11Renderer::Instance().SetCullMode(CullMode::Front);
    
    for (const auto& val : OutlineVec)
    {
        auto cmp = val->GetComponent<Outline>();
        if (cmp)
        {
            MeshRenderer* mr = val->GetComponent<MeshRenderer>();
            Transform* tf = val->GetTransform();

            Material* mat = mr->GetSharedMaterial();
            if (!mat) mat = ResourceManager::Instance().Load<Material>("Materials/Error");

            Mesh* mesh = mr->GetMesh();
            if (!mesh || !mat) return;

            //
            Effect* eff = val->GetComponent<Effect>();
            if (eff) eff->BindEP();

            if (Skeletal* sk = val->GetComponent<Skeletal>(); sk != nullptr)
            {
                DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(sk->GetFinalMatrix());
            }

            if (Rigid* rg = val->GetComponent<Rigid>(); rg != nullptr)
            {
                DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(rg->GetFinalTransforms());
            }
 
            if (Shader* shader = cmp->GetShader(); shader != nullptr)
                DX11Renderer::Instance().BindShader(shader);

            mat->FixeddBindPipeLine();
            const Matrix& worldTM = tf->GetWorldMatrix();
            Matrix worldInvT = tf->GetWorldInverseTransposeMatrix();

            mat->BindPerObject(worldTM, worldInvT);
            mesh->Bind();
            mesh->Draw();

        }

    }

    DX11Renderer::Instance().SetCullMode(CullMode::Back);
}

void Scene::RenderUI(Camera* camera, RenderTexture* renderTarget)
{
    if (!camera) return;

    float width = (float)DX11Renderer::Instance().GetWidth();
    float height = (float)DX11Renderer::Instance().GetHeight();

    if (renderTarget != nullptr)
    {
        width = (float)renderTarget->GetWidth();
        height = (float)renderTarget->GetHeight();
    }

    DX11Renderer::Instance().BeginUIRender(width, height);

    std::vector<UIBase*> uiList;
    uiList.reserve(m_gameObjects.size());

    for (const auto& go : GetGameObjects())
    {
        if (!go || !go->IsActiveInHierarchy()) continue;

        for (const auto& comp : go->_GetComponents())
        {
            if (auto ui = dynamic_cast<UIBase*>(comp.get()))
            {
                if (ui->IsActive())
                {
                    uiList.push_back(ui);
                }
            }
        }
    }

    std::sort(uiList.begin(), uiList.end(), [](UIBase* a, UIBase* b) {
        return a->GetOrderInLayer() < b->GetOrderInLayer();
        });

    for (auto* ui : uiList)
    {
        ui->RenderUI();
    }

    //for (const auto& go : GetGameObjects())
    //{
    //    if (!go || !go->IsActiveInHierarchy()) continue;

    //    auto cmp = go->GetComponent<Text>();
    //    if (cmp && cmp->IsActive())
    //    {
    //        cmp->Render();
    //    }
    //}

    DX11Renderer::Instance().EndUIRender();
}

//DepthStencil Flag; 
//Stencil 중복 
void Scene::RenderShadows()
{

    ShadowMap* shadow = nullptr;
    for (const auto& go : m_gameObjects)
    {
        if (go->IsActiveInHierarchy())
        {
            if (auto s = go->GetComponent<ShadowMap>())
            {
                shadow = s;
                break;
            }
        }
    }

    if (!shadow) return;

    Transform* tf = shadow->GetTransform();
    DX11Renderer::Instance().BeginShadowPass(
        tf->GetPosition(),
        tf->Forward(),
        true,           
        shadow->m_size  
    );


    for (const auto& go : m_gameObjects)
    {
        if (!go || !go->IsActiveInHierarchy()) continue;

        if (go->GetComponent<UIBase>()) continue;

        if (go->GetName() == "HPBarEdge_Final") continue;
        if (go->GetName() == "HPBarVoid") continue;
        if (go->GetName() == "HPBarFill") continue;
        

        //if (go->GetComponent<HPBarFollowEvent>()) continue;

        MeshRenderer* mr = go->GetComponent<MeshRenderer>();

        if (!mr || !mr->IsActive()) continue;

        Material* mat = mr->GetSharedMaterial();

        if (!mat || mat->GetRenderMode() == RenderMode::Transparent) continue; // 임시로 그냥 pass 시킴 

        Mesh* mesh = mr->GetMesh();
        if (!mesh) continue;

        Transform* transform = go->GetTransform();
        mat->Bind(transform->GetWorldMatrix(), transform->GetWorldInverseTransposeMatrix());

        if (Skeletal* sk = go->GetComponent<Skeletal>(); sk != nullptr)
        {
            DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(sk->GetFinalMatrix());
        }

        if (Rigid* rg = go->GetComponent<Rigid>(); rg != nullptr)
        {
            DX11Renderer::Instance().UpdateMatrixPallette_CBUFFER(rg->GetFinalTransforms());
        }


        mesh->Bind();
        mesh->Draw();
    }
}

void Scene::FlushPending()
{
    std::vector<GameObject*> added;
    for (auto& up : m_pendingAdd)
    {
        GameObject* raw = up.get();
        m_gameObjects.emplace_back(std::move(up));
        added.push_back(raw);
    }
    m_pendingAdd.clear();

    if (m_phase == ScenePhase::Awake)
    {
        for (auto* obj : added)
            obj->Awake();
    }
    else
    {
        for (auto* obj : added)
        {
            obj->Awake();
            obj->Start();

        }
    }

    for (auto* dead : m_pendingDestroy)
    {
        auto it = std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
            [dead](auto& up) { return up.get() == dead; });
        m_gameObjects.erase(it, m_gameObjects.end());
    }
    m_pendingDestroy.clear();
}