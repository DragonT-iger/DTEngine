// InputManager.h
#pragma once

#include "Singleton.h"
#include <array>
#include <cstdint>   
#include "KeyCode.h" 

struct MousePos
{
    int x;
    int y;
};


class InputManager : public Singleton<InputManager>
{
public:
    friend class Singleton<InputManager>;

    using UINT = unsigned int;
    using WPARAM = std::uintptr_t;
    using LPARAM = std::intptr_t;

    void Initialize();
    void Update();

    void HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    void SetWindowHandle(void* hwnd) { m_hWnd = hwnd; }

#ifdef _DEBUG
    void SetEditorMousePos(int x, int y) { m_gameMousePos = { x, y }; }

    void SetGameResolution(int width, int height) { m_gameResolution = { width, height }; }

    const MousePos& GetGameResolution() const { return m_gameResolution; }
#endif

    void EndFrame();

    bool GetKeyDown(KeyCode key) const;
    bool GetKey(KeyCode key) const;
    bool GetKeyUp(KeyCode key) const;

    const MousePos& GetMousePosition() const { return m_mousePos; }
    const MousePos& GetMouseDelta() const { return m_mouseDelta; }

    const MousePos& GetGameMousePosition() const { return m_gameMousePos; }

    float GetMouseWheelDelta() const { return m_mouseWheelDelta; }
private:
    InputManager() = default;
    ~InputManager() = default;

    std::array<bool, 256> m_keyState;
    std::array<bool, 256> m_keyDownState;
    std::array<bool, 256> m_keyUpState;

    MousePos m_mousePos = { 0, 0 };
    MousePos m_gameMousePos = { 0, 0 };
    MousePos m_prevMousePos = { 0, 0 };
    MousePos m_mouseDelta = { 0, 0 };
    float m_mouseWheelDelta = 0;
    void* m_hWnd = nullptr;

    int MapKeyCodeToVKey(KeyCode key) const;
#ifdef _DEBUG
    MousePos m_gameResolution = { 800, 600 };
#endif
};