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


	Scene* scene = ResourceManager::Instance().Load<Scene>("Scenes/SampleScene.scene");

	if(scene == nullptr)
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

	if(m_editorCameraObject)
	{
		//std::cout << "EditorCamera 게임 오브젝트를 씬에서 찾았습니다." << std::endl;
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
	


	SceneManager::Instance().RegisterScene("SampleScene", scene);
	SceneManager::Instance().LoadScene("SampleScene");


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


			if (SceneManager::Instance().GetActiveScene()->GetMainCamera() != nullptr) {
				SceneManager::Instance().GetActiveScene()->GetMainCamera()->LateUpdate(deltaTime);
			}

		}
	}

	// 렌더링


	//Scene Save (CTAL + S), 

	bool ctrlPressed = InputManager::Instance().GetKey(KeyCode::Control);
	bool sPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::S);

	if (ctrlPressed && sPressed_Down)
	{
		if (scene)
		{
			if (m_engineMode == EngineMode::Edit)
			{
				std::string sceneName = scene->GetName();
				if (!sceneName.empty())
				{
					std::string relativePath = "Scenes/" + sceneName + ".scene";

					std::cout << "Saving scene (" << sceneName << ") to: " << relativePath << std::endl;

					if (scene->SaveFile(relativePath))
					{
						std::cout << "Scene save successful." << std::endl;
						HistoryManager::Instance().MarkAsSaved();
					}
					else
					{
						std::cout << "Scene save FAILED." << std::endl;
					}
				}
				else
				{
					std::cout << "Cannot save: Scene name is empty." << std::endl;
				}
			}
			else {
				std::cout << "Cannot Save In PlayMode." << std::endl;
			}
		}
	}


#endif

#ifdef _DEBUG
	m_sceneRT->Bind();


	m_sceneRT->Clear(0.2f, 0.2f, 0.2f, 1.0f);
	// 씬 뷰

	Camera* editorCam = m_editorCameraObject->GetComponent<Camera>();

	RenderScene(scene, editorCam, m_sceneRT.get());


	m_gameRT->Bind();

	Camera* mainCamera = scene->GetMainCamera();

	if (mainCamera)
	{
		const auto& col = mainCamera->GetClearColor();
		m_gameRT->Clear(col.x, col.y, col.z, 1.0f);

		RenderScene(scene, mainCamera, m_gameRT.get());
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

	m_editorUI->Render(scene);

	m_imgui->Render();


#else


	static const float black[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	DX11Renderer::Instance().BeginFrame(black);

	Camera* mainCam = scene->GetMainCamera();

	const float* clearColor = (mainCam) ? (float*)&mainCam->GetClearColor() : new float[4] {0, 0, 0, 1};
	DX11Renderer::Instance().BeginFrame(clearColor);

	if (mainCam)
	{
		float ratio = DX11Renderer::Instance().GetAspectRatio();
		mainCam->SetAspectRatio(ratio);

		RenderScene(scene, mainCam, nullptr);
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
		

		SceneManager::Instance().RestoreActiveScene();

		Scene* scene = SceneManager::Instance().GetActiveScene();
		m_editorCameraObject = scene->FindGameObject("EditorCamera55");
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, true);




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

void Game::RenderScene(Scene* scene, Camera* camera, RenderTexture* rt)
{
	if (!scene || !camera) return;

	float ratio = 1.777f;

	if (rt != nullptr)
	{
		ratio = (float)rt->GetWidth() / (float)rt->GetHeight();
	}
	else
	{
		ratio = DX11Renderer::Instance().GetAspectRatio();
	}
	
	camera->SetAspectRatio(ratio);



	DX11Renderer::Instance().ResetRenderState();


	//if(rt != nullptr) std::cout << camera->_GetTypeName() << "화면비" << rt->GetWidth() << " " << rt->GetHeight() << std::endl;

	const Matrix& viewTM = camera->GetViewMatrix();
	const Matrix& projTM = camera->GetProjectionMatrix();
	DX11Renderer::Instance().UpdateFrameCBuffer(viewTM, projTM);

	for (const auto& go : scene->GetGameObjects())
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		MeshRenderer* meshRenderer = go->GetComponent<MeshRenderer>();
		Transform* transform = go->GetTransform();

		if (!meshRenderer || !transform || !meshRenderer->IsActive()) continue;

		//Mesh* mesh = ResourceManager::Instance().Load<Mesh>("Models/Dwarf.x");
		//Material* material = ResourceManager::Instance().Load<Material>("Shaders/Default");

		Mesh* mesh = meshRenderer->GetMesh();
		//Material* material = meshRenderer->GetMaterial();
		Material* material = ResourceManager::Instance().Load<Material>("Materials/Default");

		if (!mesh || !material) continue;

		const Matrix& worldTM = transform->GetWorldMatrix();
		Matrix worldInvT_TM = transform->GetWorldInverseTransposeMatrix();

		material->Bind(worldTM, worldInvT_TM);
		mesh->Bind();
		mesh->Draw();
	}
}
