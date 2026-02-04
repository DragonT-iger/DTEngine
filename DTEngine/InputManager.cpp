#include "pch.h" 
#include "InputManager.h"
#include "Windows.h"


void InputManager::Initialize()
{
    m_keyState.fill(false);
    m_keyDownState.fill(false);
    m_keyUpState.fill(false);

}

void InputManager::Update()
{
    POINT pt;
    GetCursorPos(&pt); 

    m_mousePos.x = static_cast<int>(pt.x);
    m_mousePos.y = static_cast<int>(pt.y);

#ifndef _DEBUG
    if (m_hWnd)
    {
        ScreenToClient((HWND)m_hWnd, &pt);
        m_gameMousePos.x = static_cast<int>(pt.x);
        m_gameMousePos.y = static_cast<int>(pt.y);
    }
#endif
    m_mouseDelta.x = m_mousePos.x - m_prevMousePos.x;
    m_mouseDelta.y = m_mousePos.y - m_prevMousePos.y;

    m_prevMousePos = m_mousePos;

    
	//std::cout << "Game Mouse Pos: (" << m_gameMousePos.x << ", " << m_gameMousePos.y << ")\n";

	//std::cout << "Mouse Pos: (" << m_mousePos.x << ", " << m_mousePos.y << ")\n";
}

void InputManager::EndFrame()
{
    m_keyDownState.fill(false);
    m_keyUpState.fill(false);
    m_mouseWheelDelta = 0.0f;
}

void InputManager::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*unsigned int vKey = static_cast<unsigned int>(wParam);
    if (vKey >= 256)
    {
        return;
    }*/

    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: 
    {
        unsigned int vKey = static_cast<unsigned int>(wParam);
        if (vKey < 256)
        {
            if (!m_keyState[vKey])
            {
                m_keyDownState[vKey] = true;
            }
            m_keyDownState[vKey] = true;
        }
        
        break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        unsigned int vKey = static_cast<unsigned int>(wParam);
        if (vKey < 256)
        {
            m_keyUpState[vKey] = true;
            m_keyState[vKey] = false;
        }
        break;
    }


    case WM_LBUTTONDOWN:
        if (!m_keyState[VK_LBUTTON]) m_keyDownState[VK_LBUTTON] = true;
        m_keyState[VK_LBUTTON] = true;
        break;
    case WM_LBUTTONUP:
        m_keyUpState[VK_LBUTTON] = true;
        m_keyState[VK_LBUTTON] = false;
        break;
    case WM_RBUTTONDOWN:
        if (!m_keyState[VK_RBUTTON]) m_keyDownState[VK_RBUTTON] = true;
        m_keyState[VK_RBUTTON] = true;
        break;
    case WM_RBUTTONUP:
        m_keyUpState[VK_RBUTTON] = true;
        m_keyState[VK_RBUTTON] = false;
        break;
    case WM_MBUTTONDOWN:
        if (!m_keyState[VK_MBUTTON]) m_keyDownState[VK_MBUTTON] = true;
        m_keyState[VK_MBUTTON] = true;
        break;
    case WM_MBUTTONUP:
        m_keyUpState[VK_MBUTTON] = true;
        m_keyState[VK_MBUTTON] = false;
        break;
    case WM_MOUSEWHEEL:
    {
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        m_mouseWheelDelta = static_cast<float>(delta) / 120.0f;
        break;
    }
    }

}

bool InputManager::GetKeyDown(KeyCode key) const
{
    //if (!m_isGameInputActive) return false;
    int vKey = MapKeyCodeToVKey(key);
    return (vKey < 256) ? m_keyDownState[vKey] : false;
}

bool InputManager::GetKey(KeyCode key) const
{
    //if (!m_isGameInputActive) return false;
    int vKey = MapKeyCodeToVKey(key);
    return (vKey < 256) ? m_keyState[vKey] : false;
}

bool InputManager::GetKeyUp(KeyCode key) const
{
    //if (!m_isGameInputActive) return false;
    int vKey = MapKeyCodeToVKey(key);
    return (vKey < 256) ? m_keyUpState[vKey] : false;
}

int InputManager::MapKeyCodeToVKey(KeyCode key) const
{
    if ((key >= KeyCode::A && key <= KeyCode::Z) ||
        (key >= KeyCode::Num0 && key <= KeyCode::Num9))
    {
        return static_cast<int>(key);
    }

    switch (key)
    {
    case KeyCode::MouseLeft:    return VK_LBUTTON;
    case KeyCode::MouseRight:   return VK_RBUTTON;
    case KeyCode::MouseMiddle:  return VK_MBUTTON;
    case KeyCode::Control:      return VK_CONTROL;
    case KeyCode::Shift:        return VK_SHIFT;
    case KeyCode::Alt:          return VK_MENU; 
    case KeyCode::Space:        return VK_SPACE;
    case KeyCode::Enter:        return VK_RETURN;
    case KeyCode::Escape:       return VK_ESCAPE;
    case KeyCode::Tab:          return VK_TAB;
    case KeyCode::Backspace:    return VK_BACK; 
    case KeyCode::Delete:       return VK_DELETE;
    case KeyCode::Home:         return VK_HOME;
    case KeyCode::F1:           return VK_F1;
    case KeyCode::F2:           return VK_F2;
    case KeyCode::F3:           return VK_F3;
    case KeyCode::F4:           return VK_F4;
    case KeyCode::F5:           return VK_F5;
    case KeyCode::F6:           return VK_F6;
    case KeyCode::F7:           return VK_F7;
    case KeyCode::F8:           return VK_F8;
    case KeyCode::F9:           return VK_F9;
    case KeyCode::F10:          return VK_F10;
    case KeyCode::F11:          return VK_F11;
    case KeyCode::F12:          return VK_F12; // 왠진 모르겠지만 실제로 해보면 F12는 뭔가 안됨
    default:                    return 0;
    }
}