#include "pch.h"
#include "WindowBase.h"
#include "GameTimer.h"
#include "Game.h"
#include "DX11Renderer.h"

bool Game::Init()
{
	// Ÿ�̸� �ʱ�ȭ
	m_timer = std::make_unique<GameTimer>();
	m_timer->Reset();

	// â �����
	if (!WindowBase::Create(L"D3DEngine", 1920, 1080, WindowMode::Windowed, 500, 0)) { return false; };

	// ������ �ʱ�ȭ
	m_renderer = std::make_unique<DX11Renderer>();
	if (!m_renderer->Init(m_hWnd)) { return false; }




	return true; 
}

void Game::Run()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer->Tick();
			Update(m_timer->DeltaTime()); // ms �ƴ�
		}
	}
}

void Game::Release()
{
	__super::Destroy();
}

void Game::Update(float deltaTime)
{
	printf("%f\n", deltaTime);
}

bool Game::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void Game::OnResize(int width, int height)
{
}

void Game::OnClose()
{
}
