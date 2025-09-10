#include "pch.h"
#include <d3d11.h>
#include <wrl.h>
#pragma comment(lib, "d3d11.lib")
#include "DX11Renderer.h"

using namespace Microsoft::WRL;
using namespace DirectX;

bool DX11Renderer::Init(HWND hwnd)
{
	//----------------------------------------
	// 1단계 : 렌더링 장치 관련 필수 설정
	//----------------------------------------
	// D3D 렌더링 장치 Device 및 스왑체인 Swap-Chain 생성. 
	CreateDeviceSwapChain(hwnd);

	// 렌더타겟(백버퍼) 생성(획득)
	CreateRenderTarget();

	// 장치 출력병합기(Output Merger) 에 렌터링 타겟 및 깊이-스텐실 버퍼 등록.
	m_pDXDC->OMSetRenderTargets(
		1,				// 렌더타겟 개수.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// 렌더타겟("백버퍼") 등록.	
		nullptr
	);

	// 뷰포트 설정.
	SetViewPort();


	//장치 정보 획득
	//GetDeviceInfo();
	return true;
}

HRESULT DX11Renderer::CreateDeviceSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC sd = {};
	//ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = m_bWindowMode;								//풀스크린 또는 창모드 선택.  전체화면(Fullscreen) 구성에 대한 자세한 내용은 다음을 참고하십시오 : https://msdn.microsoft.com/en-us/library/bb205075(v=vs.85).aspx 
	sd.OutputWindow = hWnd;										//출력할 윈도우 핸들.
	sd.BufferCount = 1;											//백버퍼 개수.
	sd.BufferDesc.Width = m_Mode.Width;							//해상도 결정.(백버퍼 크기)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;						//백버퍼 색상규격 (A8R8G8B8) 창모드에서는 생략 가능 
	sd.BufferDesc.RefreshRate.Numerator = m_bVSync ? 60 : 0;	//버퍼 갱신율.(수직동기화 VSync 활성화시 표준갱신율 적용 : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;					//버퍼 갱신율.(수직동기화 VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//용도 설정: '렌더타겟' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = 1;									//AA 설정
	sd.SampleDesc.Quality = 0;


	hr = D3D11CreateDeviceAndSwapChain(
		NULL,								//비디오 어뎁터 포인터 (기본장치는 NULL)
		D3D_DRIVER_TYPE_HARDWARE,			//HW 가속 
		NULL,								//SW Rasterizer DLL 핸들.  HW 가속시에는 NULL.
		0,									//디바이스 생성 플래그.(기본값)
		//D3D11_CREATE_DEVICE_DEBUG,		//디바이스 생성 플래그.(디버그)
		&m_FeatureLevels,					//(생성할) 디바이스 기능 레벨(Feature Level) 배열
		1,									//(생성할) 디바이스 기능 레벨(Feature Level) 배열 크기.
		D3D11_SDK_VERSION,					//DX SDK 버전.
		&sd,								//디바이스 생성 상세 옵션.
		&m_pSwapChain,						//[출력] 스왑체인 인터페이스 얻기.
		&m_pDevice,							//[출력] 디바이스 인터페이스 얻기.
		NULL,								//[출력] (생성된) 디바이스 기능 레벨. 필요없다면 NULL 설정. 
		&m_pDXDC							//[출력] 디바이스 컨텍스트 얻기.
	);
	if (FAILED(hr))
	{
		assert(false && "디바이스 / 스왑체인 생성 실패.");
	}


	//-----------------------
	// 장치 추가 설정.
	//-----------------------
	//전체화면 방지.
	//pFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);


	//임시 인터페이스 제거.
	//...


	return hr;
}

HRESULT DX11Renderer::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// 백버퍼 획득.
	ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));							// 백버퍼 얻기, 2D 텍스쳐 행태, 화면출력용 (매크로)
	//hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);	// 백버퍼 얻기, 2D 텍스쳐 행태, 화면출력용 (상동)
	if (FAILED(hr))	return hr;

	//획득한 백버퍼에 렌더타겟 뷰 생성.(렌더타겟'형'으로 설정함)
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_pRTView);			// '렌더타겟뷰'를 생성.	
	if (FAILED(hr))
	{
		assert(false && "백버퍼 - 렌더타겟뷰 생성 실패.");
		//return hr;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)

	return hr;
}

void DX11Renderer::SetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_Mode.Width;
	vp.Height = (FLOAT)m_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pDXDC->RSSetViewports(1, &vp);
}

int DX11Renderer::ClearBackBuffer(XMFLOAT4 col)
{
	m_pDXDC->ClearRenderTargetView(m_pRTView.Get(), (float*)&col);			//렌더타겟 지우기.

	return 0;
}

int DX11Renderer::Flip()
{
	m_pSwapChain->Present(m_bVSync, 0);			//화면출력 : Flip! (+수직동기화)

	return 0;
}