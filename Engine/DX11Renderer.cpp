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
	// 1�ܰ� : ������ ��ġ ���� �ʼ� ����
	//----------------------------------------
	// D3D ������ ��ġ Device �� ����ü�� Swap-Chain ����. 
	CreateDeviceSwapChain(hwnd);

	// ����Ÿ��(�����) ����(ȹ��)
	CreateRenderTarget();

	// ��ġ ��º��ձ�(Output Merger) �� ���͸� Ÿ�� �� ����-���ٽ� ���� ���.
	m_pDXDC->OMSetRenderTargets(
		1,				// ����Ÿ�� ����.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// ����Ÿ��("�����") ���.	
		nullptr
	);

	// ����Ʈ ����.
	SetViewPort();


	//��ġ ���� ȹ��
	//GetDeviceInfo();
	return true;
}

HRESULT DX11Renderer::CreateDeviceSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC sd = {};
	//ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = m_bWindowMode;								//Ǯ��ũ�� �Ǵ� â��� ����.  ��üȭ��(Fullscreen) ������ ���� �ڼ��� ������ ������ �����Ͻʽÿ� : https://msdn.microsoft.com/en-us/library/bb205075(v=vs.85).aspx 
	sd.OutputWindow = hWnd;										//����� ������ �ڵ�.
	sd.BufferCount = 1;											//����� ����.
	sd.BufferDesc.Width = m_Mode.Width;							//�ػ� ����.(����� ũ��)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;						//����� ����԰� (A8R8G8B8) â��忡���� ���� ���� 
	sd.BufferDesc.RefreshRate.Numerator = m_bVSync ? 60 : 0;	//���� ������.(��������ȭ VSync Ȱ��ȭ�� ǥ�ذ����� ���� : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;					//���� ������.(��������ȭ VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//�뵵 ����: '����Ÿ��' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = 1;									//AA ����
	sd.SampleDesc.Quality = 0;


	hr = D3D11CreateDeviceAndSwapChain(
		NULL,								//���� ��� ������ (�⺻��ġ�� NULL)
		D3D_DRIVER_TYPE_HARDWARE,			//HW ���� 
		NULL,								//SW Rasterizer DLL �ڵ�.  HW ���ӽÿ��� NULL.
		0,									//����̽� ���� �÷���.(�⺻��)
		//D3D11_CREATE_DEVICE_DEBUG,		//����̽� ���� �÷���.(�����)
		&m_FeatureLevels,					//(������) ����̽� ��� ����(Feature Level) �迭
		1,									//(������) ����̽� ��� ����(Feature Level) �迭 ũ��.
		D3D11_SDK_VERSION,					//DX SDK ����.
		&sd,								//����̽� ���� �� �ɼ�.
		&m_pSwapChain,						//[���] ����ü�� �������̽� ���.
		&m_pDevice,							//[���] ����̽� �������̽� ���.
		NULL,								//[���] (������) ����̽� ��� ����. �ʿ���ٸ� NULL ����. 
		&m_pDXDC							//[���] ����̽� ���ؽ�Ʈ ���.
	);
	if (FAILED(hr))
	{
		assert(false && "����̽� / ����ü�� ���� ����.");
	}


	//-----------------------
	// ��ġ �߰� ����.
	//-----------------------
	//��üȭ�� ����.
	//pFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);


	//�ӽ� �������̽� ����.
	//...


	return hr;
}

HRESULT DX11Renderer::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// ����� ȹ��.
	ComPtr<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));							// ����� ���, 2D �ؽ��� ����, ȭ����¿� (��ũ��)
	//hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);	// ����� ���, 2D �ؽ��� ����, ȭ����¿� (��)
	if (FAILED(hr))	return hr;

	//ȹ���� ����ۿ� ����Ÿ�� �� ����.(����Ÿ��'��'���� ������)
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_pRTView);			// '����Ÿ�ٺ�'�� ����.	
	if (FAILED(hr))
	{
		assert(false && "����� - ����Ÿ�ٺ� ���� ����.");
		//return hr;
	}

	//���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)

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
	m_pDXDC->ClearRenderTargetView(m_pRTView.Get(), (float*)&col);			//����Ÿ�� �����.

	return 0;
}

int DX11Renderer::Flip()
{
	m_pSwapChain->Present(m_bVSync, 0);			//ȭ����� : Flip! (+��������ȭ)

	return 0;
}