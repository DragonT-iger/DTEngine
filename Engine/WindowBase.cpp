#include "pch.h"
#include "WindowBase.h"


LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowBase* window = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); 
	// window가 이미 생성되었는지 안되어 있는지 검사 클래스의 bool 로는 할수가 없음 멤버 함수가 아니라서


	if (window != nullptr)
	{
		if (window->OnWndProc(hWnd, message, wParam, lParam)) {
			return 0;
		}
	}

	switch (message)
	{
	case WM_SIZE:
		if (window) window->OnResize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_CLOSE:
		if (window) window->OnClose();
					window->Destroy();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool WindowBase::Create(const wchar_t* windowName, int width, int height, WindowMode windowMode, int posX, int posY)
{

	//https://learn.microsoft.com/ko-kr/windows/win32/api/winuser/ns-winuser-wndclassexw

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);								// 구조체 크기
	wc.style = CS_CLASSDC;										// 
	wc.lpfnWndProc = WndProc;									// 윈도우 프로시저
	wc.cbClsExtra = 0;											
	wc.cbWndExtra = 0;											
	wc.hInstance = ::GetModuleHandle(nullptr);					
	wc.hIcon = nullptr;											
	wc.hCursor = nullptr;										
	wc.hbrBackground = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);	// 배경 브러시(어두운 회색)
	wc.lpszMenuName = nullptr;									// 메뉴 리소스 이름
	wc.lpszClassName = L"D3D11Engine";							// 윈도우 클래스 이름 내부 식별용
	wc.hIconSm = nullptr;										// 작은 아이콘 (왼쪽 위에 표시되는 아이콘)


	ATOM atom = RegisterClassEx(&wc);

	if (atom == 0) { assert(false && "RegisterClassEx 실패"); }



	//https://learn.microsoft.com/ko-kr/windows/win32/api/winuser/nf-winuser-createwindowa
	
	m_hWnd = CreateWindow(
		L"D3D11Engine",											// RegisterClassEx에서 등록된 클래스 이름
		windowName,												// 실제 창에 표시될 제목
		WS_OVERLAPPEDWINDOW,									// 창 스타일 WS_OVERLAPPEDWINDOW는 일반적으로 다 가능 ex) 크기조절, 최대최소
		0, 0,													// 창의 초기 위치 - 나중에 센터 기준으로 재설정 할 예정
		width, height,											// 창의 크기
		GetDesktopWindow(),											
		NULL,		
		wc.hInstance, 
		NULL
	);

	if (m_hWnd == NULL) { assert(false && "CreateWindow 실패"); }



	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);		// 윈도우 프로시저에서 생성되었는지 알 수 있게 함


	ResizeWindow(width, height, posX, posY);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);




    return true;
}

bool WindowBase::ResizeWindow(int width, int height, int posX, int posY)
{
	if (m_hWnd == NULL) { assert(false && "윈도우 핸들 없이 창 크기 조절 불가능"); }

	//현재 윈도 정보 획득. 
	RECT oldrc;	::GetWindowRect(m_hWnd, &oldrc);
	DWORD sytle = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//새로 생성될 윈도우의 '클라이언트' 영역.
	RECT newrc = { 0, 0, (LONG)width, (LONG)height };

	//newrc 만큼의 클라이언트 영역을 포함하는 윈도 '전체' 크기를 구합니다.
	//현재 메뉴는 없다는 가정하에 처리되고 있습니다.
	//계산된 결과는 newrc 에 다시 들어옵니다. (스크린 좌표)
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);

	//보정된 윈도우의 너비와 폭을 구합니다. 
	int newWidth = (newrc.right - newrc.left);
	int newHeight = (newrc.bottom - newrc.top);

	//새로운 크기를 윈도우에 설정합니다.
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, oldrc.left, oldrc.top,
		newWidth, newHeight, SWP_SHOWWINDOW);


	RECT rc = {};
	::GetClientRect(GetDesktopWindow(), &rc);
	rc.left = rc.right / 2 - width / 2;
	rc.top = rc.bottom / 2 - height / 2;

	::MoveWindow(m_hWnd, rc.left + posX, rc.top + posY, width, height, TRUE);

	return true;
}

bool WindowBase::SetResolution(int width, int height)
{
    return false;
}

bool WindowBase::SetFullScreen()
{
    return false;
}

bool WindowBase::SetBorderless()
{
    return false;
}

bool WindowBase::SetWindowed()
{
    return false;
}

void WindowBase::Destroy()
{
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}
