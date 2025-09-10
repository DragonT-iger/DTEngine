#pragma once


class Game : public WindowBase
{
public:
	Game() = default; 
	virtual ~Game() = default; 
	bool Init(); 
	void Run(); 
	void Release();

	void Update(float deltaTime); // ���߿� �����丵 ����

private: 
	bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override; 
	void OnResize(int width, int height) override; 
	void OnClose() override; 
	
	GameTimer m_timer;
};

