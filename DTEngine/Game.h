#pragma once
#include <cstdint>
#include <memory>

#include "WindowBase.h"

class DX11Renderer; 
class GameTimer;
class ImGuiLayer;
class EditorUI;


class Game : public WindowBase {
public:
	Game();
	~Game() override;


	bool Initialize();
	void Run();
	void Release();

	
	void LifeCycle(float deltaTime);

protected:
	bool OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam) override;
	void OnResize(int width, int height) override;
	void OnClose() override;

private: 
	float backBufferColor[4] = { 0.10f, 0.10f, 0.12f, 1.0f };

	std::unique_ptr<DX11Renderer> m_renderer;
	std::unique_ptr<GameTimer> m_timer;
	std::unique_ptr<ImGuiLayer> m_imgui;
	std::unique_ptr<EditorUI> m_editorUI;
};