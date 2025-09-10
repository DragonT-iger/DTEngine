#include "pch.h"
#include "WindowBase.h"


LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowBase* window = reinterpret_cast<WindowBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); 
	// window�� �̹� �����Ǿ����� �ȵǾ� �ִ��� �˻� Ŭ������ bool �δ� �Ҽ��� ���� ��� �Լ��� �ƴ϶�


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

	wc.cbSize = sizeof(WNDCLASSEX);								// ����ü ũ��
	wc.style = CS_CLASSDC;										// 
	wc.lpfnWndProc = WndProc;									// ������ ���ν���
	wc.cbClsExtra = 0;											
	wc.cbWndExtra = 0;											
	wc.hInstance = ::GetModuleHandle(nullptr);					
	wc.hIcon = nullptr;											
	wc.hCursor = nullptr;										
	wc.hbrBackground = (HBRUSH)::GetStockObject(DKGRAY_BRUSH);	// ��� �귯��(��ο� ȸ��)
	wc.lpszMenuName = nullptr;									// �޴� ���ҽ� �̸�
	wc.lpszClassName = L"D3D11Engine";							// ������ Ŭ���� �̸� ���� �ĺ���
	wc.hIconSm = nullptr;										// ���� ������ (���� ���� ǥ�õǴ� ������)


	ATOM atom = RegisterClassEx(&wc);

	if (atom == 0) { assert(false && "RegisterClassEx ����"); }



	//https://learn.microsoft.com/ko-kr/windows/win32/api/winuser/nf-winuser-createwindowa
	
	m_hWnd = CreateWindow(
		L"D3D11Engine",											// RegisterClassEx���� ��ϵ� Ŭ���� �̸�
		windowName,												// ���� â�� ǥ�õ� ����
		WS_OVERLAPPEDWINDOW,									// â ��Ÿ�� WS_OVERLAPPEDWINDOW�� �Ϲ������� �� ���� ex) ũ������, �ִ��ּ�
		0, 0,													// â�� �ʱ� ��ġ - ���߿� ���� �������� �缳�� �� ����
		width, height,											// â�� ũ��
		GetDesktopWindow(),											
		NULL,		
		wc.hInstance, 
		NULL
	);

	if (m_hWnd == NULL) { assert(false && "CreateWindow ����"); }



	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);		// ������ ���ν������� �����Ǿ����� �� �� �ְ� ��


	ResizeWindow(width, height, posX, posY);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);




    return true;
}

bool WindowBase::ResizeWindow(int width, int height, int posX, int posY)
{
	if (m_hWnd == NULL) { assert(false && "������ �ڵ� ���� â ũ�� ���� �Ұ���"); }

	//���� ���� ���� ȹ��. 
	RECT oldrc;	::GetWindowRect(m_hWnd, &oldrc);
	DWORD sytle = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//���� ������ �������� 'Ŭ���̾�Ʈ' ����.
	RECT newrc = { 0, 0, (LONG)width, (LONG)height };

	//newrc ��ŭ�� Ŭ���̾�Ʈ ������ �����ϴ� ���� '��ü' ũ�⸦ ���մϴ�.
	//���� �޴��� ���ٴ� �����Ͽ� ó���ǰ� �ֽ��ϴ�.
	//���� ����� newrc �� �ٽ� ���ɴϴ�. (��ũ�� ��ǥ)
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);

	//������ �������� �ʺ�� ���� ���մϴ�. 
	int newWidth = (newrc.right - newrc.left);
	int newHeight = (newrc.bottom - newrc.top);

	//���ο� ũ�⸦ �����쿡 �����մϴ�.
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
