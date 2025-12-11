#include "pch.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <filesystem>

#include <imgui.h>
#include "Game.h"
#include "DX11Renderer.h"
#include "GameTimer.h"
#include "ImGuiLayer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "AssetDatabase.h"
#include "ResourceManager.h"
#include "RegisterCoreTypes.h"
#include "EditorUI.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "InputManager.h"
#include "ImGuizmo.h"
#include "HistoryManager.h"
#include "FreeCamera.h"
#include "RenderTexture.h"
#include "move.h"
#include "Light.h"
//#include "Text3D.h"
#include "Text.h"
#include "Image.h"



Game::Game() = default;
Game::~Game() = default;


bool Game::Initialize()
{
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);

	m_timer = std::make_unique<GameTimer>();
	m_timer->Reset();

	constexpr int initW = 1920, initH = 1080;
	if (!WindowBase::Create(L"DTEngine", initW, initH)) {
		assert(false && "윈도우 창 만들기 실패");
		return false;
	}

	if (!DX11Renderer::Instance().Initialize(GetHwnd(), initW, initH)) {
		assert(false && "DX11Renderer 초기화 실패");
		return false;
	}

#ifdef _DEBUG

	m_imgui = std::make_unique<ImGuiLayer>();
	if (!m_imgui->Initialize(&DX11Renderer::Instance())) {
		assert(false && "ImGui 초기화 실패");
		return false;
	}
	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	m_editorUI = std::make_unique<EditorUI>();

#endif
	if(!ResourceManager::Instance().Initialize("Assets"))
	{
		assert(false && "ResourceManager 초기화 실패");
		return false;
	}
	// 리소스 매니저 먼저 초기화

	if (!AssetDatabase::Instance().Initialize()) {
		assert(false && "전역 id 초기화 실패");
		return false;
	};

	RegisterCoreTypes registerCoreTypes = RegisterCoreTypes();

	if(!registerCoreTypes.Initialize())
	{
		assert(false && "핵심 타입 등록 실패");
		return false;
	}


	InputManager::Instance().Initialize();


	SceneManager::Instance().RegisterScene("Scenes/SampleScene.scene");
	SceneManager::Instance().LoadScene("SampleScene");
	SceneManager::Instance().ProcessSceneChange();

	Scene* scene = SceneManager::Instance().GetActiveScene();
	if (scene == nullptr)
	{
		assert(false && "기본 씬 로드 실패");
		return false;
	}

#ifdef _DEBUG
	m_sceneRT = std::make_unique<RenderTexture>();
	m_sceneRT->Initialize(1280, 720);

	m_gameRT = std::make_unique<RenderTexture>();
	m_gameRT->Initialize(1280, 720);

	m_editorCameraObject = scene->FindGameObject("EditorCamera55");

	if (m_editorCameraObject)
	{
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, true);
	}
	else
	{
		std::cout << "EditorCamera 게임 오브젝트가 씬에 없음. 새로 생성합니다." << std::endl;
		m_editorCameraObject = scene->CreateGameObject("EditorCamera55");
		m_editorCameraObject->AddComponent<Camera>();
		m_editorCameraObject->AddComponent<FreeCamera>();
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, true);
	}

#else
	m_gameRT = std::make_unique<RenderTexture>();
	m_gameRT->Initialize(1920, 1080);
#endif
	




	//Scene testScene("TestScene");

	//GameObject* parentGO = testScene.CreateGameObject("Parent");
	//Transform* parentTF = parentGO->GetTransform();
	//parentTF->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	//parentTF->SetRotationEuler(Vector3(0.0f, 45.0f, 0.0f));
	//parentTF->SetScale(Vector3(1.0f, 2.0f, 1.0f));

	//GameObject* childGO = testScene.CreateGameObject("Child");
	//Transform* childTF = childGO->GetTransform();
	//childTF->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	//childTF->SetParent(parentTF);

	//GameObject* camera = testScene.CreateGameObject("Camera");
	//camera->GetTransform()->SetPosition(Vector3(0, 2, -5));
	//camera->GetTransform()->SetRotationEuler(Vector3(15.0f, 0.0f, 0.0f));
	//camera->AddComponent<Camera>();




	//GameObject* cubeGO = testScene.CreateGameObject("TestCube");

	//Mesh* cubeMesh = ResourceManager::Instance().Load<Mesh>("TestCubeMesh");
	//Material* defaultMat = ResourceManager::Instance().Load<Material>("Shaders/Default");

	//MeshRenderer* renderer = cubeGO->AddComponent<MeshRenderer>();
	//renderer->SetMesh(cubeMesh);
	//renderer->SetMaterial(defaultMat);





	//std::cout << "--- SAVING ---" << std::endl;
	//std::cout << "Parent ID: " << parentTF->_GetID() << std::endl;
	//std::cout << "Child Parent ID (before save): " << (childTF->GetParent() ? childTF->GetParent()->_GetID() : 0) << std::endl;
	//std::cout << "Child Position (before save): " << childTF->GetPosition().y << std::endl;




	//testScene.SaveFile("Scenes/SampleScene.scene");



	return true;
}


void Game::Run()
{
	while (true)
	{
		if (!PumpEvents())
			break;

		m_timer->Tick();
 		LifeCycle(m_timer->DeltaTime());
	}
}


void Game::Release()
{
#ifdef _DEBUG
	if (m_editorUI) { m_editorUI.reset(); }
	if (m_imgui) { m_imgui->Shutdown(); m_imgui.reset(); }
#endif
	DX11Renderer::Instance().Destroy();
	__super::Destroy();
	ReflectionDatabase::Instance().Clear();
}


void Game::LifeCycle(float deltaTime)
{
	SceneManager::Instance().ProcessSceneChange(); // Awake Start



	Scene* scene = SceneManager::Instance().GetActiveScene();


	if (!scene) {
		std::cout << "현재 활성화된 씬이 없음" << std::endl;
		return;
	}


	InputManager::Instance().Update();

#ifdef _DEBUG
	if (m_engineMode == EngineMode::Play)
	{
#endif  
		static float elapsedTime = 0.0f;
		static float fixedDeltaTime = 0.02f;

		elapsedTime += deltaTime;

		while (elapsedTime >= fixedDeltaTime)
		{
			scene->FixedUpdate(fixedDeltaTime);
			elapsedTime -= fixedDeltaTime;
		}

		scene->Update(deltaTime);





		scene->LateUpdate(deltaTime);
#ifdef _DEBUG
	}

	if (m_engineMode == EngineMode::Edit || m_engineMode == EngineMode::Pause) {

		if (m_editorCameraObject != nullptr) {
		
			m_editorCameraObject->Update(deltaTime);

			m_editorCameraObject->LateUpdate(deltaTime);

			DX11Renderer::Instance().UpdateLights(Light::GetAllLights());


			Scene* activeScene = SceneManager::Instance().GetActiveScene();

			if (activeScene)
			{
				const auto& gameObjects = activeScene->GetGameObjects();

				for (const auto& go : gameObjects)
				{
					if (!go->IsActiveInHierarchy()) continue;

					Camera* cam = go->GetComponent<Camera>();

					if (cam)
					{
						if (go.get() != m_editorCameraObject)
						{
							cam->LateUpdate(deltaTime);
						}
					}
				}
			}

		}
	}

	// 렌더링


	


#endif

#ifdef _DEBUG
	m_sceneRT->Bind();


	m_sceneRT->Clear(0.2f, 0.2f, 0.2f, 1.0f);
	// 씬 뷰

	Camera* editorCam = m_editorCameraObject->GetComponent<Camera>();

	RenderScene(scene, editorCam, m_sceneRT.get() , true);


	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		// 에디터 카메라는 위에서 이미 그렸으므로 패스
		if (!cam || !cam->IsActive() || cam == editorCam) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();

		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			RenderScene(scene, cam, targetRT , false);

		}
		else
		{
			m_gameRT->Bind();
			const auto& col = cam->GetClearColor();
			m_gameRT->Clear(col.x, col.y, col.z, col.w);

			RenderScene(scene, cam, m_gameRT.get() , true);
		}
	}


	static const float black[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	DX11Renderer::Instance().BeginFrame(black);

	m_imgui->NewFrame();
	ImGuizmo::BeginFrame();


	m_editorUI->RenderToolbar(m_engineMode, [this](EngineMode mode) {
		if (mode == EngineMode::Play) {
			if (m_engineMode == EngineMode::Edit) SetPlayState(true);
			else SetPlayState(false);
		}
		else if (mode == EngineMode::Pause) {
			if (m_engineMode == EngineMode::Play) m_engineMode = EngineMode::Pause;
			else if (m_engineMode == EngineMode::Pause) m_engineMode = EngineMode::Play;
		}
		});

	m_editorUI->RenderSceneWindow(m_sceneRT.get(), scene, editorCam);
	m_editorUI->RenderGameWindow(m_gameRT.get(), scene);




	//ImGuizmo::SetRect(
	//	0, 0,
	//	(float)DX11Renderer::Instance().GetWidth(), 
	//	(float)DX11Renderer::Instance().GetHeight() 
	//);

	m_editorUI->Render(scene , m_engineMode);

	m_imgui->Render();


#else


	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		if (!cam || !cam->IsActive()) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();
		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			RenderScene(scene, cam, targetRT, false);
		}
	}

	Camera* mainCam = scene->GetMainCamera();

	const float* clearColor = (mainCam) ? (float*)&mainCam->GetClearColor() : new float[4] {0.1f, 0.1f, 0.1f, 1.0f};

	DX11Renderer::Instance().BeginFrame(clearColor);

	if (mainCam)
	{
		float ratio = DX11Renderer::Instance().GetAspectRatio();
		mainCam->SetAspectRatio(ratio);

		RenderScene(scene, mainCam, nullptr, true);
	}



#endif







	DX11Renderer::Instance().EndFrame();
	DX11Renderer::Instance().Present();

	InputManager::Instance().EndFrame();

}
#ifdef _DEBUG
void Game::SetPlayState(bool isPlay)
{

	uint64_t lastSelectedID = 0;
	if (m_editorUI && m_editorUI->GetSelectedGameObject())
	{
		lastSelectedID = m_editorUI->GetSelectedGameObject()->_GetID();
	}

	if (m_editorUI) m_editorUI->ClearSelection();

	if (isPlay)
	{
		if (SceneManager::Instance().BackupActiveScene())
		{
			m_engineMode = EngineMode::Play;

		}
	}
	else
	{
		m_engineMode = EngineMode::Edit;
		Vector3 lastPos = Vector3(0, 0, 0);
		Vector3 lastRot = Vector3(0, 0, 0);
		bool hasLastState = false;

		if (m_editorCameraObject && m_editorCameraObject->GetTransform())
		{
			lastPos = m_editorCameraObject->GetTransform()->GetPosition();
			lastRot = m_editorCameraObject->GetTransform()->GetEditorEuler(); 
			hasLastState = true;
		}

		SceneManager::Instance().RestoreActiveScene();

		Scene* scene = SceneManager::Instance().GetActiveScene();
		m_editorCameraObject = scene->FindGameObject("EditorCamera55");
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, true);
		if (hasLastState)
		{
			Transform* camTf = m_editorCameraObject->GetTransform();
			camTf->SetPosition(lastPos);
			camTf->SetRotationEuler(lastRot);

			if (scene)
			{
				scene->Awake();
				scene->Start();
			}
		}



		Camera* newMainCam = nullptr;

		for (const auto& go : scene->GetGameObjects())
		{
			if (go.get() == m_editorCameraObject) continue;

			if (Camera* cam = go->GetComponent<Camera>())
			{
				newMainCam = cam;
				break; 
			}
		}

		scene->SetMainCamera(newMainCam);


		

	}

	if (lastSelectedID != 0 && m_editorUI)
	{
		Scene* currentScene = SceneManager::Instance().GetActiveScene();
		if (currentScene)
		{
			GameObject* foundGO = currentScene->FindGameObjectByID(lastSelectedID);

			if (foundGO)
			{
				m_editorUI->SetSelectedGameObject(foundGO);
			}
		}
	}

}
#endif

bool Game::OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam)
{
#ifdef _DEBUG
	if (m_imgui && m_imgui->WndProcHandler(hWnd, msg, wparam, lparam))
		return true;
#endif

	InputManager::Instance().HandleMessage((UINT)msg, (WPARAM)wparam, (LPARAM)lparam);

	return false;
}

void Game::OnResize(int width, int height)
{
	if (width <= 0 || height <= 0) return;
	DX11Renderer::Instance().Resize(width, height);
	
	SceneManager::Instance().GetActiveScene()->GetMainCamera()->SetViewDirty();
}

void Game::OnClose()
{
}

void Game::RenderScene(Scene* scene, Camera* camera, RenderTexture* rt, bool renderUI)
{
	if (!scene || !camera) return;


	float width = (float)DX11Renderer::Instance().GetWidth();
	float height = (float)DX11Renderer::Instance().GetHeight();

	if (rt != nullptr)
	{
		width = (float)rt->GetWidth();
		height = (float)rt->GetHeight();
	}

	float ratio = width / height;
	camera->SetAspectRatio(ratio);

	DX11Renderer::Instance().ResetRenderState();

	camera->Bind();
	//DX11Renderer::Instance().SetViewport(width, height);


	//if(rt != nullptr) std::cout << camera->_GetTypeName() << "화면비" << rt->GetWidth() << " " << rt->GetHeight() << std::endl;

	const Matrix& viewTM = camera->GetViewMatrix();
	const Matrix& projTM = camera->GetProjectionMatrix();
	DX11Renderer::Instance().UpdateFrameCBuffer(viewTM, projTM);

	std::vector<GameObject*> opaqueQueue;
	std::vector<GameObject*> transparentQueue;
	std::vector<GameObject*> uiQueue;

	for (const auto& go : scene->GetGameObjects())
	{
		if (!go || !go->IsActiveInHierarchy()) continue;
		MeshRenderer* mr = go->GetComponent<MeshRenderer>();
		Image* img = go->GetComponent<Image>();
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

	auto DrawObject = [&](GameObject* go) {
		MeshRenderer* mr = go->GetComponent<MeshRenderer>();
		Transform* tf = go->GetTransform();

		Material* mat = mr->GetSharedMaterial();
		if (!mat) mat = ResourceManager::Instance().Load<Material>("Materials/Error");

		Mesh* mesh = mr->GetMesh();
		if (!mesh || !mat) return;

		const Matrix& worldTM = tf->GetWorldMatrix();
		Matrix worldInvT = tf->GetWorldInverseTransposeMatrix();

		mat->Bind(worldTM, worldInvT); 
		mesh->Bind();
		mesh->Draw();
	};


	for (auto* go : opaqueQueue)
	{
		DrawObject(go);
	}

	for (auto* go : transparentQueue)
	{
		DrawObject(go);
	}

	if (renderUI) {
		DX11Renderer::Instance().BeginUIRender(); // 카메라 행렬 Identity , 직교투영 DTXK 초기화 

		std::sort(uiQueue.begin(), uiQueue.end(), [](GameObject* a, GameObject* b) {
			return a->GetComponent<Image>()->GetOrderInLayer() < b->GetComponent<Image>()->GetOrderInLayer();
		});

		for (auto* go : uiQueue)
		{
			DrawObject(go);
		}

		DX11Renderer::Instance().EndUIRender();
	}

	
}
