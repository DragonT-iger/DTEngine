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

Game::Game() = default;
Game::~Game() = default;


bool Game::Initialize()
{

	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);

	m_timer = std::make_unique<GameTimer>();
	m_timer->Reset();

	constexpr int initW = 1920, initH = 1080;
	if (!WindowBase::Create(L"D3DEngine", initW, initH)) {
		assert(false && "윈도우 창 만들기 실패");
		return false;
	}

	if (!DX11Renderer::Instance().Initialize(GetHwnd(), initW, initH)) {
		assert(false && "DX11Renderer 초기화 실패");
		return false;
	}

	m_imgui = std::make_unique<ImGuiLayer>();
	if (!m_imgui->Initialize(&DX11Renderer::Instance())) {
		assert(false && "ImGui 초기화 실패");
		return false;
	}

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	m_editorUI = std::make_unique<EditorUI>();

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
	if (m_editorUI) { m_editorUI.reset(); }
	if (m_imgui) { m_imgui->Shutdown(); m_imgui.reset(); }
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
	



	// 렌더링

	Camera* mainCamera = scene->GetMainCamera();

	const float* clearColor;

	if (mainCamera != nullptr) {
		clearColor = &mainCamera->GetClearColor().x;
	}
	else
	{
		static const float defaultColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		clearColor = defaultColor;
		std::cout << "메인 카메라가 존재하지 않습니다" << std::endl;
	}

	DX11Renderer::Instance().BeginFrame(clearColor);


	if (mainCamera)
	{
		const Matrix& viewTM = mainCamera->GetViewMatrix();

		//std::cout << viewTM << std::endl;

		const Matrix& projTM = mainCamera->GetProjectionMatrix();


		//std::cout << projTM << std::endl;

		DX11Renderer::Instance().UpdateFrameCBuffer(viewTM, projTM);
	}

	for (const auto& go : scene->GetGameObjects())
	{
		if (!go || !go->IsActive() || !go->IsActiveInHierarchy()) continue;

		MeshRenderer* meshRenderer = go->GetComponent<MeshRenderer>();
		Transform* transform = go->GetTransform();

		if (!meshRenderer || !transform) continue;

		if (meshRenderer->IsActive() == false) continue;

		//Mesh* mesh = meshRenderer->GetMesh();
		//Material* material = meshRenderer->GetMaterial();


		Mesh* mesh = ResourceManager::Instance().Load<Mesh>("TestCubeMesh");
		Material* material = ResourceManager::Instance().Load<Material>("Shaders/Default");

		if (!mesh || !material) continue;

		const Matrix& worldTM = transform->GetWorldMatrix();
		Matrix worldInvT_TM = transform->GetWorldInverseTransposeMatrix();
		material->Bind(worldTM, worldInvT_TM);

		mesh->Bind();

		mesh->Draw();
	}
	


	//Scene Save (CTAL + S), 

	bool ctrlPressed = InputManager::Instance().GetKey(KeyCode::Control);
	bool sPressed_Down = InputManager::Instance().GetKeyDown(KeyCode::S);

	if (ctrlPressed && sPressed_Down)
	{
		if (scene)
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
	}









	m_imgui->NewFrame();

	ImGuizmo::BeginFrame();

	ImGuizmo::SetRect(
		0, 0,
		(float)DX11Renderer::Instance().GetWidth(), 
		(float)DX11Renderer::Instance().GetHeight() 
	);

	m_editorUI->Render(scene);

	m_imgui->Render();

	DX11Renderer::Instance().EndFrame();
	DX11Renderer::Instance().Present();

	InputManager::Instance().EndFrame();

}

bool Game::OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam)
{

	if (m_imgui && m_imgui->WndProcHandler(hWnd, msg, wparam, lparam))
		return true;


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
