#pragma once

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



class WindowBase
{
public:
	enum class WindowMode
	{
		Windowed,
		Borderless,
		Fullscreen
	};

	bool Create(const wchar_t* windowName = L"D3DEngine", int width = 1920, int height = 1080,
				WindowMode windowMode = WindowMode::Windowed, int posX = 0, int posY = 0);

	// posX, posY�� â ��忡���� ����� �����ϸ鼭 �ܼ�â ���� �׽�Ʈ�� �� ���ϰ� �Ϸ���


	bool ResizeWindow(int width, int height, int posX, int posY);

	bool SetResolution(int width, int height);
	bool SetFullScreen();
	bool SetBorderless();
	bool SetWindowed();




	void Destroy();


protected:

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual bool OnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return false; }

	virtual void OnResize(int width, int height) abstract;
	virtual void OnClose() {};

	HWND m_hWnd;
};

