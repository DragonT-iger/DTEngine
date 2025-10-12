#include "pch.h"

#include <Windows.h>
#include <imgui.h>
#include <iostream>


#include "Game.h"
#include "DX11Renderer.h"
#include "GameTimer.h"
#include "ImGuiLayer.h"
#include "SceneManager.h"
#include "Scene.h"

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
	if (m_imgui) { m_imgui->Shutdown(); m_imgui.reset(); }
	if (m_renderer) { m_renderer->Destroy(); m_renderer.reset(); }
	Destroy();
}


void Game::LifeCycle(float deltaTime)
{
	m_renderer->BeginFrame(backBufferColor);
	SceneManager::Instance().ProcessSceneChange();

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



	// ImGui 프레임
	//m_imgui->NewFrame();
	//if (ImGui::Begin("Engine Stats")) {
	//	ImGui::Text("Delta: %.3f ms (%.1f FPS)", m_timer->DeltaTimeMS(),
	//		(deltaTime > 0.f ? 1.0f / deltaTime : 0.f));
	//	bool vsync = m_renderer->IsVSync();
	//	if (ImGui::Checkbox("VSync", &vsync)) m_renderer->ToggleVSync();
	//}
	//ImGui::End();

	//// ImGui 드로우
	//m_imgui->Render();

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
