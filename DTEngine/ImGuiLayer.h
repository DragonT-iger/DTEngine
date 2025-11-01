#pragma once
#include <cstdint>

struct HWND__;
using HWND = HWND__*;

class DX11Renderer;

class ImGuiLayer {
public:
    ImGuiLayer();
    ~ImGuiLayer();

    bool Initialize(DX11Renderer* renderer);

    void NewFrame();  
    void Render();    
    void Shutdown();  

    bool WndProcHandler(HWND hWnd, uint32_t msg, uintptr_t wParam, intptr_t lParam);

    bool WantsMouse() const;
    bool WantsKeyboard() const;

private:
    DX11Renderer* m_renderer = nullptr;
    HWND          m_hwnd = nullptr;
};
