#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

struct HWND__;
using HWND = HWND__*;

class WindowBase {
public:
    enum class WindowMode { Windowed, Borderless, Fullscreen };
    virtual ~WindowBase();

    bool Create(const wchar_t* title = L"D3DEngine",
        int width = 1920, int height = 1080,
        WindowMode mode = WindowMode::Windowed,
        int posX = 0, int posY = 0);

    bool ResizeWindow(int width, int height, int posX, int posY);
    void Destroy();
    bool PumpEvents();

protected:
    
    HWND GetHwnd() const { return m_hWnd; }

    virtual bool OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam);
    virtual void OnResize(int width, int height) = 0;
    virtual void OnClose() {}

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private: 
    HWND m_hWnd = nullptr;
};