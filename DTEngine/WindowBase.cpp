#include "pch.h"
#include "WindowBase.h"

LRESULT CALLBACK WindowBase::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* self = reinterpret_cast<WindowBase*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    // 먼저 파생 클래스에게 기회 제공
    if (self) {
        if (self->OnWndProc(hWnd, msg, wParam, lParam))
            return 0;
    }

    switch (msg)
    {
    case WM_SIZE:
        if (self) self->OnResize(LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_CLOSE:
        if (self) self->OnClose();
        if (self) self->Destroy();
        return 0;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    default:
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

WindowBase::~WindowBase() = default;

bool WindowBase::Create(const wchar_t* windowName, int width, int height,
    WindowMode, int posX, int posY)
{
    WNDCLASSEXW wc{}; wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = &WindowBase::WndProc; // ★ 멤버 정적 함수 등록
    wc.hInstance = ::GetModuleHandleW(nullptr);
    wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"D3D11Engine";

    ATOM atom = ::RegisterClassExW(&wc);
    assert(atom);

    RECT rc{ 0,0,width,height };
    ::AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);

    m_hWnd = ::CreateWindowExW(
        0, wc.lpszClassName, windowName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, wc.hInstance, nullptr);
    assert(m_hWnd);

    ::SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ResizeWindow(width, height, posX, posY);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::UpdateWindow(m_hWnd);
    return true;
}

bool WindowBase::ResizeWindow(int width, int height, int posX, int posY)
{
    assert(m_hWnd && "ResizeWindow: no window handle");


    RECT oldrc; ::GetWindowRect((HWND)m_hWnd, &oldrc);
    DWORD style = (DWORD)::GetWindowLongW((HWND)m_hWnd, GWL_STYLE);
    DWORD exstyle = (DWORD)::GetWindowLongW((HWND)m_hWnd, GWL_EXSTYLE);


    RECT newrc{ 0, 0, width, height };
    ::AdjustWindowRectEx(&newrc, style, FALSE, exstyle);


    int newWidth = newrc.right - newrc.left;
    int newHeight = newrc.bottom - newrc.top;


    ::SetWindowPos((HWND)m_hWnd, HWND_NOTOPMOST, oldrc.left, oldrc.top, newWidth, newHeight, SWP_SHOWWINDOW);


    RECT desk{}; ::GetClientRect(::GetDesktopWindow(), &desk);
    int cx = desk.right / 2 - width / 2;
    int cy = desk.bottom / 2 - height / 2;
    ::MoveWindow((HWND)m_hWnd, cx + posX, cy + posY, width, height, TRUE);


    return true;
}

bool WindowBase::OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam)
{
    return false; 
}

void WindowBase::Destroy()
{
    SafeClose(m_hWnd, DestroyWindow);
}

bool WindowBase::PumpEvents()
{
    MSG msg{};
    while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
    return true;
}
