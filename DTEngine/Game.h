#pragma once
#include <cstdint>
#include <memory>

#include "WindowBase.h"
class GameTimer;
class ImGuiLayer;
class EditorUI;
class RenderTexture;
class Scene;
class Camera;
class GameObject;

class Game : public WindowBase {
public:
	Game();
	~Game() override;

	enum class EngineMode {
		Edit,
		Play
	};

	bool Initialize();
	void Run();
	void Release();

	
	void LifeCycle(float deltaTime);

	EngineMode GetEngineMode() const { return m_engineMode; }
	void SetEngineMode(EngineMode mode) { m_engineMode = mode; }

protected:
	bool OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam) override;
	void OnResize(int width, int height) override;
	void OnClose() override;

private: 

	void RenderScene(Scene* scene, Camera* camera , RenderTexture* rt);

	EngineMode m_engineMode = EngineMode::Edit;

	float backBufferColor[4] = { 0.10f, 0.10f, 0.12f, 1.0f };

	std::unique_ptr<GameTimer> m_timer;

	std::unique_ptr<RenderTexture> m_gameRT;

#ifdef _DEBUG
	std::unique_ptr<ImGuiLayer>    m_imgui;
	std::unique_ptr<EditorUI>      m_editorUI;
	std::unique_ptr<RenderTexture> m_sceneRT;
	GameObject* m_editorCameraObject = nullptr;
#endif
};