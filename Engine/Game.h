#pragma once

#include "WindowBase.h"
#include "GameTimer.h"
#include "DX11Renderer.h"

class Game : public WindowBase
{
public:
	Game() = default; 
	virtual ~Game() = default; 
	bool Init(); 
	void Run(); 
	void Release();

	void Update(float deltaTime); // 나중에 리팩토링 예정

private: 
	bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override; 
	void OnResize(int width, int height) override; 
	void OnClose() override; 
	

	std::unique_ptr<DX11Renderer> m_renderer;
	std::unique_ptr<GameTimer> m_timer;
};

