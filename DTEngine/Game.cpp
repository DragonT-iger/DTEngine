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

Game::Game() = default;
Game::~Game() = default;


bool Game::Initialize()
{
	m_timer = std::make_unique<GameTimer>();
	m_timer->Reset();

	constexpr int initW = 1920, initH = 1080;
	if (!WindowBase::Create(L"D3DEngine", initW, initH)) {
		assert(false && "윈도우 창 만들기 실패");
		return false;
	}

	m_renderer = std::make_unique<DX11Renderer>();
	if (!m_renderer->Initialize(GetHwnd(), initW, initH)) {
		assert(false && "DX11Renderer 초기화 실패");
		return false;
	}

	m_imgui = std::make_unique<ImGuiLayer>();
	if (!m_imgui->Initialize(m_renderer.get())) {
		assert(false && "ImGui 초기화 실패");
		return false;
	}

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



	Scene* scene = ResourceManager::Instance().Load<Scene>("Scenes/SampleScene.scene");

	if(scene == nullptr)
	{
		assert(false && "기본 씬 로드 실패");
		return false;
	}

	SceneManager::Instance().RegisterScene("SampleScene", scene);
	SceneManager::Instance().LoadScene("SampleScene");


	Scene testScene("TestScene");

	GameObject* parentGO = testScene.CreateGameObject("Parent");
	Transform* parentTF = parentGO->GetTransform();
	parentTF->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	parentTF->SetRotationEuler(Vector3(0.0f, 45.0f, 0.0f));
	parentTF->SetScale(Vector3(1.0f, 2.0f, 1.0f));

	GameObject* childGO = testScene.CreateGameObject("Child");
	Transform* childTF = childGO->GetTransform();
	childTF->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	childTF->SetParent(parentTF);

	std::cout << "--- SAVING ---" << std::endl;
	std::cout << "Parent ID: " << parentTF->_GetID() << std::endl;
	std::cout << "Child Parent ID (before save): " << (childTF->GetParent() ? childTF->GetParent()->_GetID() : 0) << std::endl;
	std::cout << "Child Position (before save): " << childTF->GetPosition().y << std::endl;

	testScene.SaveFile("Scenes/SampleScene.scene");




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
	if (m_renderer) { m_renderer->Destroy(); m_renderer.reset(); }
	__super::Destroy();
}


void Game::LifeCycle(float deltaTime)
{
	SceneManager::Instance().ProcessSceneChange();

	m_renderer->BeginFrame(backBufferColor);

	Scene* scene = SceneManager::Instance().GetActiveScene();

	if (!scene) {
		std::cout << "현재 활성화된 씬이 없음" << std::endl;
		return;
	}

	static float elapsedTime = 0.0f;
	static float fixedDeltaTime = 0.02f;

	elapsedTime += deltaTime;

	scene->Update(deltaTime);
	if (scene)
	{
		while (elapsedTime >= fixedDeltaTime)
		{
			scene->FixedUpdate(fixedDeltaTime);
			elapsedTime -= fixedDeltaTime;
		}

		scene->Update(deltaTime);

		scene->LateUpdate(deltaTime);
	}

	m_imgui->NewFrame();

	m_editorUI->Render(scene);

	m_imgui->Render();

	m_renderer->EndFrame();
	m_renderer->Present();

}

bool Game::OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam)
{

	if (m_imgui && m_imgui->WndProcHandler(hWnd, msg, wparam, lparam))
		return true;


	return false;
}

void Game::OnResize(int width, int height)
{
	if (!m_renderer) return;
	if (width <= 0 || height <= 0) return;
	m_renderer->Resize(width, height);
}

void Game::OnClose()
{
}
