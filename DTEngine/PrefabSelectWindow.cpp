#include "pch.h"
#include "PrefabSelectWindow.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Image.h"
#include "UIButton.h"
#include "ResourceManager.h"
#include "Prefab.h"

BEGINPROPERTY(PrefabSelectWindow)
DTPROPERTY_ACCESSOR(PrefabSelectWindow, m_nightPrefab, GetNightPrefab, SetNightPrefab)
DTPROPERTY_ACCESSOR(PrefabSelectWindow, m_rookPrefab, GetRookPrefab, SetRookPrefab)
DTPROPERTY_ACCESSOR(PrefabSelectWindow, m_bishopPrefab, GetBishopPrefab, SetBishopPrefab)
ENDPROPERTY()

void PrefabSelectWindow::CreateOnce()
{
		// 생성해주기. 생성하고는 바로 끄기.
		if (m_created) return;
		InitializeWindow();
		LoadPrefab();
		Close();
		m_created = true;
}

// window active 및 선택 초기화.
void PrefabSelectWindow::Open()
{
		if (!m_Window) return;
		m_selectIndex = 0;
		m_selectPrefab = nullptr;
		m_hasResult = false;
		UpdateButtonVisuals();
		m_Window->SetActive(true);
}

// 닫기.
void PrefabSelectWindow::Close()
{
		if (m_Window) m_Window->SetActive(false);
}

void PrefabSelectWindow::Awake()
{
		CreateOnce();
}

void PrefabSelectWindow::InitializeWindow()
{
		// 처음 전체 생성.
		Scene* scene = SceneManager::Instance().GetActiveScene();

		// window 생성.
		GameObject* settingWindow = scene->CreateGameObject("PrefabSelectWindow");
		m_Window = settingWindow;

		// BG 생성
		GameObject* settingWindowBG = scene->CreateGameObject("PrefabSelectWindowBG");
		settingWindowBG->GetTransform()->SetParent(settingWindow->GetTransform());
		settingWindowBG->AddComponent<Image>();

		// 크기는 임시값으로.
		settingWindowBG->GetTransform()->SetScale(Vector3(600, 800, 1));

		// titleBar 생성.
		GameObject* titleBar = scene->CreateGameObject("TitleBar");
		titleBar->GetTransform()->SetParent(settingWindowBG->GetTransform());
		titleBar->AddComponent<Image>();

		// selectImage 생성.
		GameObject* selectNight = scene->CreateGameObject("SelectNight");
		GameObject* selectRook = scene->CreateGameObject("SelectRook");
		GameObject* selectBishop = scene->CreateGameObject("SelectBishop");
		
		selectNight->GetTransform()->SetParent(settingWindowBG->GetTransform());
		selectNight->AddComponent<Image>();
		m_nightImg = selectNight->GetComponent<Image>();
		selectNight->AddComponent<UIButton>();
		selectNight->GetComponent<UIButton>()->SetOnClick([this]() 
				{
						// 이미 내가 선택되어 있다면? -> 선택 해제
						m_selectIndex = (m_selectIndex == 1) ? 0 : 1;
						UpdateButtonVisuals();
				});

		selectRook->GetTransform()->SetParent(settingWindowBG->GetTransform());
		selectRook->AddComponent<Image>();
		m_rookImg = selectRook->GetComponent<Image>();
		selectRook->AddComponent<UIButton>();
		selectRook->GetComponent<UIButton>()->SetOnClick([this]() 
				{
						m_selectIndex = (m_selectIndex == 2) ? 0 : 2;
						UpdateButtonVisuals();
				});
		selectRook->GetComponent<UIButton>()->SetNormalColor(Vector4(0.4f, 0.4f, 0.4f, 1.0f));
		selectRook->GetComponent<UIButton>()->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		selectRook->GetComponent<UIButton>()->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		selectBishop->GetTransform()->SetParent(settingWindowBG->GetTransform());
		selectBishop->AddComponent<Image>();
		m_bishopImg = selectBishop->GetComponent<Image>();
		selectBishop->AddComponent<UIButton>();
		selectBishop->GetComponent<UIButton>()->SetOnClick([this]() 
				{
						m_selectIndex = (m_selectIndex == 3) ? 0 : 3;
						UpdateButtonVisuals();
				});
		selectBishop->GetComponent<UIButton>()->SetNormalColor(Vector4(0.4f, 0.4f, 0.4f, 1.0f));
		selectBishop->GetComponent<UIButton>()->SetHoverColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		selectBishop->GetComponent<UIButton>()->SetPressedColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		// Okay Button 생성.
		GameObject* okayButton = scene->CreateGameObject("OkayButton");
		okayButton->GetTransform()->SetParent(settingWindowBG->GetTransform());
		okayButton->AddComponent<Image>();
		okayButton->AddComponent<UIButton>();
		okayButton->GetComponent<UIButton>()->SetOnClick([this]()
				{
						// 결국 값이 선택했으니 확인 누르면 창 toggle.
						switch (m_selectIndex)
						{
						case 1: m_selectPrefab = m_nightPrefab;  break;
						case 2: m_selectPrefab = m_rookPrefab;   break;
						case 3: m_selectPrefab = m_bishopPrefab; break;
						default: m_selectPrefab = nullptr;       break;
						}
						// 결과 있는거고 닫기.
						m_hasResult = true;
						Close();
				});

		// exitButton 생성
		GameObject* exitButton = scene->CreateGameObject("ExitButton");
		exitButton->GetTransform()->SetParent(settingWindowBG->GetTransform());
		exitButton->AddComponent<Image>();
		exitButton->AddComponent<UIButton>();
		exitButton->GetComponent<UIButton>()->SetOnClick([this]() 
				{
						// 창 닫기 및 결과 반환.
						m_selectPrefab = nullptr;
						m_hasResult = true;
						Close();
				});
}


void PrefabSelectWindow::LoadPrefab()
{
		// prefab 위치 고정이라서 수정해야함.
		m_rookPrefab = ResourceManager::Instance().Load<Prefab>("Prefab/Cube");
		m_nightPrefab = ResourceManager::Instance().Load<Prefab>("Prefab/Dwarf");
		m_bishopPrefab = ResourceManager::Instance().Load<Prefab>("Prefab/MasterChief_v3");
}

// 결과 넘기고 초기화 시키기.
bool PrefabSelectWindow::TryConsumeResult(Prefab*& out)
{
		if (!m_hasResult) return false;
		out = m_selectPrefab;
		m_hasResult = false;
		m_selectPrefab = nullptr;
		return true;
}

void PrefabSelectWindow::UpdateButtonVisuals() 
{
		// 없으면 update 안함.
		if (!m_nightImg || !m_rookImg || !m_bishopImg) return;

		// 각각 setcolor로 활성화인지 아닌지.
		m_nightImg->SetColor(m_selectIndex == 1 ? Vector4(1, 1, 1, 1) : Vector4(0.4f, 0.4f, 0.4f, 1.0f));
		m_rookImg->SetColor(m_selectIndex == 2 ? Vector4(1, 1, 1, 1) : Vector4(0.4f, 0.4f, 0.4f, 1.0f));
		m_bishopImg->SetColor(m_selectIndex == 3 ? Vector4(1, 1, 1, 1) : Vector4(0.4f, 0.4f, 0.4f, 1.0f));
}