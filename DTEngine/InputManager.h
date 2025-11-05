// InputManager.h
#pragma once

#include "Singleton.h"
#include <array>
#include <cstdint>   
#include "KeyCode.h" 

class InputManager : public Singleton<InputManager>
{
public:
    friend class Singleton<InputManager>;

    using UINT = unsigned int;
    using WPARAM = std::uintptr_t;
    using LPARAM = std::intptr_t;

    void Initialize();

    void HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    void EndFrame();

    bool GetKeyDown(KeyCode key) const; 
    bool GetKey(KeyCode key) const;     
    bool GetKeyUp(KeyCode key) const;   

private:
    InputManager() = default;
    ~InputManager() = default;

    std::array<bool, 256> m_keyState;
    std::array<bool, 256> m_keyDownState;
    std::array<bool, 256> m_keyUpState;

    int MapKeyCodeToVKey(KeyCode key) const;
};