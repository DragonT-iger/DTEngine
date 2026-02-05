#include "pch.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <imgui.h>
#include "Game.h"
#include "DX11Renderer.h"
#include "GameTimer.h"
#include "ImGuiLayer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "AssetDatabase.h"
#include "ResourceManager.h"
#include "RegisterCoreTypes.h"
#include "EditorUI.h"
#include "Transform.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "InputManager.h"
#include "ImGuizmo.h"
#include "HistoryManager.h"
#include "FreeCamera.h"
#include "RenderTexture.h"
#include "move.h"
#include "Light.h"
//#include "Text3D.h"
#include "Text.h"
#include "Image.h"
#include "ReflectionProbe.h"
#include "UIButton.h"
#include "UISlider.h"
//#include "RectTransform.h"
#include "Canvas.h"

#include "FSMRegister.h"
#include "SoundManager.h"
#include "SkyBoxComponent.h"

#include "BGMUISlider.h"

#include "../Client/ClientSceneManager.h"



Game::Game() = default;
Game::~Game() = default;


bool Game::Initialize()
{
	SetConsoleOutputCP(CP_UTF8);

	m_timer = std::make_unique<GameTimer>();
	m_timer->Reset(); 

	constexpr int initW = 1920, initH = 1200;



	if (!WindowBase::Create(L"DTEngine", initW, initH)) {
		assert(false && "윈도우 창 만들기 실패");
		return false;
	}

	if (!DX11Renderer::Instance().Initialize(GetHwnd(), initW, initH)) {
		assert(false && "DX11Renderer 초기화 실패");
		return false;
	}

#ifdef _DEBUG

	m_imgui = std::make_unique<ImGuiLayer>();
	if (!m_imgui->Initialize(&DX11Renderer::Instance())) {
		assert(false && "ImGui 초기화 실패");
		return false;
	}
	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	m_editorUI = std::make_unique<EditorUI>();

#endif
	if(!ResourceManager::Instance().Initialize("Assets"))
	{
		assert(false && "ResourceManager 초기화 실패");
		return false;
	}
	// 리소스 매니저 먼저 초기화

	if (!AssetDatabase::Instance().Initialize()) {
		assert(false && "전역 id 초기화 실패");
		return false;
	};

	RegisterCoreTypes registerCoreTypes = RegisterCoreTypes();

	if(!registerCoreTypes.Initialize())
	{
		assert(false && "핵심 타입 등록 실패");
		return false;
	}


	if (!SoundManager::Instance().Initalize())
	{
		assert(false && "SoundManager 초기화 실패");
		return false;
	}

	InputManager::Instance().Initialize(); 
	InputManager::Instance().SetWindowHandle(GetHwnd());



	//SceneManager::Instance().RegisterScene("Scenes/GridTestScene.scene");
	//SceneManager::Instance().LoadScene("GridTestScene");
	//SceneManager::Instance().RegisterScene("Scenes/DTtestScene.scene");
	//SceneManager::Instance().LoadScene("DTtestScene");

	//SceneManager::Instance().RegisterScene("Scenes/TutorialScene.scene");
	//SceneManager::Instance().LoadScene("TutorialScene");


	 //SceneManager::Instance().RegisterScene("Scenes/SampleSceneBum.scene");
	 //SceneManager::Instance().LoadScene("SampleSceneBum");

	SceneManager::Instance().RegisterScene("Scenes/SampleScene.scene");
	SceneManager::Instance().LoadScene("SampleScene");

	 //SceneManager::Instance().RegisterScene("Scenes/DTtestScene.scene");
	 //SceneManager::Instance().LoadScene("DTtestScene");

	//SceneManager::Instance().RegisterScene("Scenes/DTtestScene.scene");
	//SceneManager::Instance().LoadScene("DTtestScene");

	 //SceneManager::Instance().RegisterScene("Scenes/SampleSceneBum.scene");
	 //SceneManager::Instance().LoadScene("SampleSceneBum");


	/* SceneManager::Instance().RegisterScene("Scenes/TitleScene.scene");
	 ClientSceneManager::Instance().LoadScene("TitleScene");*/


	 //SceneManager::Instance().RegisterScene("Scenes/Title.scene");
	 //SceneManager::Instance().LoadScene("Title");



	// SceneManager::Instance().RegisterScene("Scenes/SampleScenehshs.scene");
	// SceneManager::Instance().LoadScene("SampleScenehshs");

	SceneManager::Instance().ProcessSceneChange();

	Scene* scene = SceneManager::Instance().GetActiveScene();
	if (scene == nullptr)
	{
		assert(false && "기본 씬 로드 실패");
		return false;
	}



#ifdef _DEBUG
	m_sceneRT = std::make_unique<RenderTexture>();
	m_sceneRT->Initialize(1920, 1200, RenderTextureType::Tex2D, true , true);

	m_gameRT = std::make_unique<RenderTexture>();
	m_gameRT->Initialize(1920, 1200, RenderTextureType::Tex2D, true, false);

	SetEditorCamera(scene);




#else
	m_gameRT = std::make_unique<RenderTexture>();
	m_gameRT->Initialize(1920, 1200, RenderTextureType::Tex2D, true, false);


#endif


	m_captureRT = std::make_unique<RenderTexture>();
	m_captureRT->Initialize(initW, initH, RenderTextureType::Tex2D, false, true);



	if (!FSMRegister::Instance().Initalize())
	{
		assert(false && "FSMRegister 초기화 실패");
		return false;
	}


	//Scene testScene("TestScene");

	//GameObject* parentGO = testScene.CreateGameObject("Parent");
	//Transform* parentTF = parentGO->GetTransform();
	//parentTF->SetPosition(Vector3(1.0f, 0.0f, 0.0f));
	//parentTF->SetRotationEuler(Vector3(0.0f, 45.0f, 0.0f));
	//parentTF->SetScale(Vector3(1.0f, 2.0f, 1.0f));

	//GameObject* childGO = testScene.CreateGameObject("Child");
	//Transform* childTF = childGO->GetTransform();
	//childTF->SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	//childTF->SetParent(parentTF);

	//GameObject* camera = testScene.CreateGameObject("Camera");
	//camera->GetTransform()->SetPosition(Vector3(0, 2, -5));
	//camera->GetTransform()->SetRotationEuler(Vector3(15.0f, 0.0f, 0.0f));
	//camera->AddComponent<Camera>();




	//GameObject* cubeGO = testScene.CreateGameObject("TestCube");

	//Mesh* cubeMesh = ResourceManager::Instance().Load<Mesh>("TestCubeMesh");
	//Material* defaultMat = ResourceManager::Instance().Load<Material>("Shaders/Default");

	//MeshRenderer* renderer = cubeGO->AddComponent<MeshRenderer>();
	//renderer->SetMesh(cubeMesh);
	//renderer->SetMaterial(defaultMat);





	//std::cout << "--- SAVING ---" << std::endl;
	//std::cout << "Parent ID: " << parentTF->_GetID() << std::endl;
	//std::cout << "Child Parent ID (before save): " << (childTF->GetParent() ? childTF->GetParent()->_GetID() : 0) << std::endl;
	//std::cout << "Child Position (before save): " << childTF->GetPosition().y << std::endl;




	//testScene.SaveFile("Scenes/SampleScene.scene");


	return true;
}


void Game::Run()
{
	while (true)
	{
		if (!PumpEvents())
			break;

		m_timer->Tick();

		//UpdateTimeScale();
		DeltaTime dt{};
		dt.rawTime = m_timer->DeltaTime();
		float timeScale = SceneManager::Instance().GetActiveScene()->GetTimeScale();
		dt.scaledTime = dt.rawTime * timeScale;

		SceneManager::Instance().SetUnscaledDeltaTime(dt.rawTime);

		LifeCycle(dt);
 		//LifeCycle(m_timer->DeltaTime());
	}
}


void Game::Release()
{
#ifdef _DEBUG
	if (m_editorUI) { m_editorUI.reset(); }
	if (m_imgui) { m_imgui->Shutdown(); m_imgui.reset(); }
#endif
	DX11Renderer::Instance().Destroy();
	__super::Destroy();
	ReflectionDatabase::Instance().Clear();
}

void Game::LifeCycle(DeltaTime dt)
{
	if (SceneManager::Instance().ProcessSceneChange()) {

		

#ifdef _DEBUG

		Scene* curScene = SceneManager::Instance().GetActiveScene();
		SetEditorCamera(curScene);

#endif

	}; // Awake Start
	Scene* activeScene = SceneManager::Instance().GetActiveScene();
	if (activeScene)
	{
		activeScene->Awake();

#ifdef _DEBUG
		if (m_engineMode == EngineMode::Play)
#endif
		{
			activeScene->Start(); 
		}
	}


	Scene* scene = SceneManager::Instance().GetActiveScene();


	if (!scene) {
		std::cout << "현재 활성화된 씬이 없음" << std::endl;
		return;
	}


	InputManager::Instance().Update();
	SoundManager::Instance().Update(dt.rawTime);
#ifdef _DEBUG
	if (m_engineMode == EngineMode::Play)
	{
#endif  
		static float elapsedTime = 0.0f;
		static float fixedDeltaTime = 0.02f;

		elapsedTime += dt.scaledTime;

		while (elapsedTime >= fixedDeltaTime)
		{
			scene->FixedUpdate(fixedDeltaTime);
			elapsedTime -= fixedDeltaTime;
		}

		scene->Update(dt.scaledTime);

		scene->LateUpdate(dt.scaledTime);

#ifndef _DEBUG
	
#endif


#ifdef _DEBUG
	}




	if (m_engineMode == EngineMode::Edit || m_engineMode == EngineMode::Pause) {

		if (m_editorCameraObject != nullptr) {

			m_editorCameraObject->Update(dt.rawTime);

			m_editorCameraObject->LateUpdate(dt.rawTime);



			std::vector<Light*> temp;
			for (const auto& lights : Light::GetAllLights())
			{
				if (scene->FindGameObject(lights->_GetOwner()->GetName()) != nullptr)
					temp.push_back(lights);
			}

			DX11Renderer::Instance().UpdateLights_CBUFFER(temp, m_editorCameraObject->GetComponent<Camera>());

			//DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights(), m_editorCameraObject->GetComponent<Camera>());


			Scene* activeScene = SceneManager::Instance().GetActiveScene();

			if (activeScene)
			{
				const auto& gameObjects = activeScene->GetGameObjects();

				for (const auto& go : gameObjects)
				{
					if (!go->IsActiveInHierarchy()) continue;

					Camera* cam = go->GetComponent<Camera>();

					if (cam)
					{
						if (go.get() != m_editorCameraObject)
						{
							cam->LateUpdate(dt.rawTime);
						}
					}
				}
			}

		}
	}

	// 렌더링




#endif

	//if (scene)
	//{
	//	const auto& gameObjects = scene->GetGameObjects();
	//	for (const auto& go : gameObjects)
	//	{
	//		if (!go || !go->IsActiveInHierarchy()) continue;

	//		if (auto probe = go->GetComponent<ReflectionProbe>())
	//		{
	//			if (probe->IsActive()) {
	//				probe->Render();
	//			}
	//		}
	//	}
	//} // 나중에 필요하면 돌려이건


#ifdef _DEBUG

	if (scene) scene->RenderShadows();

	m_sceneRT->Bind();

	m_sceneRT->Clear(0.2f, 0.2f, 0.2f, 1.0f);
	// 씬 뷰
	Camera* editorCam = m_editorCameraObject->GetComponent<Camera>();


	std::vector<Light*> temp;
	for (const auto& lights : Light::GetAllLights())
	{
		if (scene->FindGameObject(lights->_GetOwner()->GetName()) != nullptr)
			temp.push_back(lights);
	}

	DX11Renderer::Instance().UpdateLights_CBUFFER(temp, m_editorCameraObject->GetComponent<Camera>());

	//DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights(), m_editorCameraObject->GetComponent<Camera>());

	scene->Render(editorCam, m_sceneRT.get());

	m_sceneRT->Unbind();

	Camera* mainCam = scene->GetMainCamera();


	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		// 에디터 카메라는 위에서 이미 그렸으므로 패스
		if (!cam || !cam->IsActive() || cam == editorCam) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();

		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			scene->Render(cam, targetRT);

			targetRT->Unbind();

		}
		else
		{
			if (m_captureRT)
			{
				if (m_gameRT && (m_captureRT->GetWidth() != m_gameRT->GetWidth() || m_captureRT->GetHeight() != m_gameRT->GetHeight()))
				{
					m_captureRT->Resize(m_gameRT->GetWidth(), m_gameRT->GetHeight());
				}
				// --------------------------------------------------------------------------

				m_captureRT->Bind(); 
				const auto& col = cam->GetClearColor();
				m_captureRT->Clear(col.x, col.y, col.z, col.w);



				std::vector<Light*> temp;
				for (const auto& lights : Light::GetAllLights())
				{
					if (scene->FindGameObject(lights->_GetOwner()->GetName()) != nullptr)
						temp.push_back(lights);
				}

				DX11Renderer::Instance().UpdateLights_CBUFFER(temp, cam);

				//DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights(), cam);

				scene->Render(cam, m_captureRT.get());

				m_captureRT->Unbind();
			}

			auto ppManager = DX11Renderer::Instance().GetPostProcessManager();

			if (ppManager && m_captureRT && m_gameRT)
			{
				uint32_t mask = (mainCam) ? mainCam->GetPostProcessMask() : 0;

				ppManager->Execute(
					m_captureRT.get(),      
					m_gameRT->GetRTV(),     
					mask,
					mainCam,
					m_gameRT->GetWidth(),   
					m_gameRT->GetHeight()   
				);
			}
		}
	}

	m_gameRT->Bind(); 
	scene->RenderUI(mainCam, m_gameRT.get());
	m_gameRT->Unbind();


	static const float black[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	DX11Renderer::Instance().BeginFrame(black);

	m_imgui->NewFrame();
	ImGuizmo::BeginFrame();


	m_editorUI->RenderToolbar(m_engineMode, [this](EngineMode mode) {
		if (mode == EngineMode::Play) {
			if (m_engineMode == EngineMode::Edit) SetPlayState(true);
			else SetPlayState(false);
		}
		else if (mode == EngineMode::Pause) {
			if (m_engineMode == EngineMode::Play) m_engineMode = EngineMode::Pause;
			else if (m_engineMode == EngineMode::Pause) m_engineMode = EngineMode::Play;
		}
		});

	m_editorUI->RenderSceneWindow(m_sceneRT.get(), scene, editorCam);
	m_editorUI->RenderGameWindow(m_gameRT.get(), scene);



	//ImGuizmo::SetRect(
	//	0, 0,
	//	(float)DX11Renderer::Instance().GetWidth(), 
	//	(float)DX11Renderer::Instance().GetHeight() 
	//);

	m_editorUI->Render(scene, m_engineMode);

	m_imgui->Render();
#else
	if (scene) scene->RenderShadows();

	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		if (!cam || !cam->IsActive()) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();
		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			RenderScene(scene, cam, targetRT);

			targetRT->Unbind();
		}
	}

	static const float defaultClearColor[4] = { 1.f, 1.f, 0.f, 1.f };

	Camera* mainCam = scene->GetMainCamera();

	const float* clearColor = (mainCam) ? (float*)&mainCam->GetClearColor() : defaultClearColor;


	if (mainCam && m_captureRT && m_gameRT)
	{
		m_captureRT->Bind();
		m_captureRT->Clear(clearColor[0], clearColor[1], clearColor[2], 1);

		float ratio = DX11Renderer::Instance().GetRefAspectRatio();
		mainCam->SetAspectRatio(ratio);

		RenderScene(scene, mainCam, m_captureRT.get());

		m_captureRT->Unbind();

		auto ppManager = DX11Renderer::Instance().GetPostProcessManager();

		uint32_t mask = (mainCam) ? mainCam->GetPostProcessMask() : 0;


		// uint32_t mask = 0xFFFFFFFF; // 모든 후처리 효과 적용
		if (ppManager)
		{
			ppManager->Execute(
				m_captureRT.get(),
				m_gameRT->GetRTV(),
				mask,
				mainCam,
				m_gameRT->GetWidth(),
				m_gameRT->GetHeight()
			);

			m_gameRT->Unbind();
		}
	}

	m_gameRT->Bind();
	scene->RenderUI(mainCam, m_gameRT.get());
	m_gameRT->Unbind();

	DX11Renderer::Instance().BeginFrame(clearColor);

	float windowWidth = static_cast<float>(DX11Renderer::Instance().GetWidth());
	float windowHeight = static_cast<float>(DX11Renderer::Instance().GetHeight());


	float targetAspect = DX11Renderer::Instance().GetRefAspectRatio();
	float windowAspect = windowWidth / windowHeight;

	float drawWidth, drawHeight;
	float offsetX, offsetY;

	if (windowAspect > targetAspect)
	{
		drawHeight = windowHeight;
		drawWidth = windowHeight * targetAspect;
		offsetX = (windowWidth - drawWidth) * 0.5f;
		offsetY = 0.0f;
	}
	else
	{
		drawWidth = windowWidth;
		drawHeight = drawWidth / targetAspect;
		offsetX = 0.0f;
		offsetY = (windowHeight - drawHeight) * 0.5f;
	}

	Texture* finalTexture = m_gameRT.get();

	Vector2 finalPos(offsetX, offsetY);
	Vector2 finalSize(drawWidth, drawHeight);




	DX11Renderer::Instance().BeginUIRender(DX11Renderer::Instance().GetRefWidth(), DX11Renderer::Instance().GetRefHeight());

	DX11Renderer::Instance().DrawUI(finalTexture, finalPos, finalSize, Vector4(1, 1, 1, 1));

	DX11Renderer::Instance().EndUIRender();



#endif







	DX11Renderer::Instance().EndFrame();
	DX11Renderer::Instance().Present();

	InputManager::Instance().EndFrame();

}

void Game::UpdateTimeScale() // 배속 테스트용
{
	//if (InputManager::Instance().GetKeyDown(KeyCode::Num0))
	//{
	//	SceneManager::Instance().GetActiveScene()->SetTimeScale(0.0f);
	//}
	//if (InputManager::Instance().GetKeyDown(KeyCode::Num1))
	//{
	//	SceneManager::Instance().GetActiveScene()->SetTimeScale(1.0f);
	//}
	//if (InputManager::Instance().GetKeyDown(KeyCode::Num2))
	//{
	//	SceneManager::Instance().GetActiveScene()->SetTimeScale(2.0f);
	//}
}

/*
void Game::LifeCycle(float deltaTime)
{
	if (SceneManager::Instance().ProcessSceneChange()) {
	

#ifdef _DEBUG

		Scene* curScene = SceneManager::Instance().GetActiveScene();
		SetEditorCamera(curScene);
	
#endif
	
	}; // Awake Start



	Scene* scene = SceneManager::Instance().GetActiveScene();


	if (!scene) {
		std::cout << "현재 활성화된 씬이 없음" << std::endl;
		return;
	}


	InputManager::Instance().Update();

#ifdef _DEBUG
	if (m_engineMode == EngineMode::Play)
	{
#endif  
		static float elapsedTime = 0.0f;
		static float fixedDeltaTime = 0.02f;

		elapsedTime += deltaTime;

		while (elapsedTime >= fixedDeltaTime)
		{
			scene->FixedUpdate(fixedDeltaTime);
			elapsedTime -= fixedDeltaTime;
		}

		scene->Update(deltaTime);





		scene->LateUpdate(deltaTime);
#ifdef _DEBUG
	}


	

	if (m_engineMode == EngineMode::Edit || m_engineMode == EngineMode::Pause) {

		if (m_editorCameraObject != nullptr) {
		
			m_editorCameraObject->Update(deltaTime);

			m_editorCameraObject->LateUpdate(deltaTime);

			DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights() , m_editorCameraObject->GetComponent<Camera>());


			Scene* activeScene = SceneManager::Instance().GetActiveScene();

			if (activeScene)
			{
				const auto& gameObjects = activeScene->GetGameObjects();

				for (const auto& go : gameObjects)
				{
					if (!go->IsActiveInHierarchy()) continue;

					Camera* cam = go->GetComponent<Camera>();

					if (cam)
					{
						if (go.get() != m_editorCameraObject)
						{
							cam->LateUpdate(deltaTime);
						}
					}
				}
			}

		}
	}

	// 렌더링


	static const float black[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	DX11Renderer::Instance().BeginFrame(black);


#endif

	if (scene)
	{
		const auto& gameObjects = scene->GetGameObjects();
		for (const auto& go : gameObjects)
		{
			if (!go || !go->IsActiveInHierarchy()) continue;

			if (auto probe = go->GetComponent<ReflectionProbe>())
			{
				if (probe->IsActive()) {
					probe->Render();
				}
			}
		}
	}


#ifdef _DEBUG

	if (scene) scene->RenderShadows();

	m_sceneRT->Bind();

	

	// 씬 뷰

	Camera* editorCam = m_editorCameraObject->GetComponent<Camera>();


	const auto& col = editorCam->GetClearColor();

	m_sceneRT->Clear(col.x , col.y , col.z , col.w);


	DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights(), editorCam);
	scene->Render(editorCam, m_sceneRT.get(), true);


	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		// 에디터 카메라는 위에서 이미 그렸으므로 패스
		if (!cam || !cam->IsActive() || cam == editorCam) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();

		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			scene->Render(cam, targetRT, false);

		}
		else
		{
			m_gameRT->Bind();
			const auto& col = cam->GetClearColor();
			m_gameRT->Clear(col.x, col.y, col.z, col.w);

			DX11Renderer::Instance().UpdateLights_CBUFFER(Light::GetAllLights(), cam->GetComponent<Camera>());

			scene->Render(cam, m_gameRT.get(), true);
		}
	}


	//static const float black[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
	DX11Renderer::Instance().BeginFrame(black);

	m_imgui->NewFrame();
	ImGuizmo::BeginFrame();


	m_editorUI->RenderToolbar(m_engineMode, [this](EngineMode mode) {
		if (mode == EngineMode::Play) {
			if (m_engineMode == EngineMode::Edit) SetPlayState(true);
			else SetPlayState(false);
		}
		else if (mode == EngineMode::Pause) {
			if (m_engineMode == EngineMode::Play) m_engineMode = EngineMode::Pause;
			else if (m_engineMode == EngineMode::Pause) m_engineMode = EngineMode::Play;
		}
		});

	m_editorUI->RenderSceneWindow(m_sceneRT.get(), scene, editorCam);
	m_editorUI->RenderGameWindow(m_gameRT.get(), scene);




	//ImGuizmo::SetRect(
	//	0, 0,
	//	(float)DX11Renderer::Instance().GetWidth(), 
	//	(float)DX11Renderer::Instance().GetHeight() 
	//);

	m_editorUI->Render(scene , m_engineMode);

	m_imgui->Render();


#else
	if (scene) scene->RenderShadows();

	const auto& gameObjects = scene->GetGameObjects();
	for (const auto& go : gameObjects)
	{
		if (!go || !go->IsActiveInHierarchy()) continue;

		Camera* cam = go->GetComponent<Camera>();
		if (!cam || !cam->IsActive()) continue;

		RenderTexture* targetRT = cam->GetTargetTexture();
		if (targetRT != nullptr)
		{
			targetRT->Bind();
			const auto& col = cam->GetClearColor();
			targetRT->Clear(col.x, col.y, col.z, col.w);

			RenderScene(scene, cam, targetRT, false);
		}
	}

	Camera* mainCam = scene->GetMainCamera();

	const float* clearColor = (mainCam) ? (float*)&mainCam->GetClearColor() : new float[4] {0.1f, 0.1f, 0.1f, 1.0f};

	DX11Renderer::Instance().BeginFrame(clearColor);

	if (mainCam)
	{
		float ratio = DX11Renderer::Instance().GetAspectRatio();
		mainCam->SetAspectRatio(ratio);

		RenderScene(scene, mainCam, nullptr, true);
	}



#endif







	DX11Renderer::Instance().EndFrame();
	DX11Renderer::Instance().Present();

	InputManager::Instance().EndFrame();

}
*/
#ifdef _DEBUG
void Game::SetPlayState(bool isPlay)
{

	uint64_t lastSelectedID = 0;
	if (m_editorUI && m_editorUI->GetSelectedGameObject())
	{
		lastSelectedID = m_editorUI->GetSelectedGameObject()->_GetID();
	}

	if (m_editorUI) m_editorUI->ClearSelection();

	if (isPlay)
	{
		if (SceneManager::Instance().BackupActiveScene())
		{
			m_engineMode = EngineMode::Play;

			Scene* scene = SceneManager::Instance().GetActiveScene();
			if (scene)
			{
				scene->Start();
			}
		}
	}
	else
	{
		m_engineMode = EngineMode::Edit;
		Vector3 lastPos = Vector3(0, 0, 0);
		Vector3 lastRot = Vector3(0, 0, 0);
		bool hasLastState = false;

		if (m_editorCameraObject && m_editorCameraObject->GetTransform())
		{
			lastPos = m_editorCameraObject->GetTransform()->GetPosition();
			lastRot = m_editorCameraObject->GetTransform()->GetEditorEuler(); 
			hasLastState = true;
		}

		SceneManager::Instance().RestoreActiveScene();

		Scene* scene = SceneManager::Instance().GetActiveScene();
		m_editorCameraObject = scene->FindGameObject("EditorCamera55");
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, false);
		if (hasLastState)
		{
			Transform* camTf = m_editorCameraObject->GetTransform();
			camTf->SetPosition(lastPos);
			camTf->SetRotationEuler(lastRot);

			if (scene)
			{
				//scene->Awake();
				//scene->Start();
			}
		}



		Camera* newMainCam = nullptr;

		for (const auto& go : scene->GetGameObjects())
		{
			if (go.get() == m_editorCameraObject) continue;

			if (Camera* cam = go->GetComponent<Camera>())
			{
				newMainCam = cam;
				break; 
			}
		}

		scene->SetMainCamera(newMainCam);


		

	}

	if (lastSelectedID != 0 && m_editorUI)
	{
		Scene* currentScene = SceneManager::Instance().GetActiveScene();
		if (currentScene)
		{
			GameObject* foundGO = currentScene->FindGameObjectByID(lastSelectedID);

			if (foundGO)
			{
				m_editorUI->SetSelectedGameObject(foundGO);
			}
		}
	}

}
void Game::SetEditorCamera(Scene* curScene)
{
	m_editorCameraObject = curScene->FindGameObject("EditorCamera55");

	if (m_editorCameraObject)
	{
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, false);
	}
	else
	{
		std::cout << "EditorCamera 게임 오브젝트가 씬에 없음. 새로 생성합니다." << std::endl;
		m_editorCameraObject = curScene->CreateGameObject("EditorCamera55");
		m_editorCameraObject->AddComponent<Camera>();
		m_editorCameraObject->AddComponent<FreeCamera>();
		m_editorCameraObject->SetFlag(GameObject::Flags::HideInHierarchy, false);
	}
}
#endif

bool Game::OnWndProc(HWND hWnd, uint32_t msg, uintptr_t wparam, intptr_t lparam)
{
#ifdef _DEBUG
	if (m_imgui && m_imgui->WndProcHandler(hWnd, msg, wparam, lparam))
		return true;
#endif

	InputManager::Instance().HandleMessage((UINT)msg, (WPARAM)wparam, (LPARAM)lparam);

	return false;
}

void Game::OnResize(int width, int height)
{
	if (width <= 0 || height <= 0) return;

	DX11Renderer::Instance().Resize(width, height);

	float windowWidth = static_cast<float>(width);
	float windowHeight = static_cast<float>(height);

	float targetAspect = DX11Renderer::Instance().GetRefAspectRatio(); 
	float windowAspect = windowWidth / windowHeight;

	float drawWidth, drawHeight;

	if (windowAspect > targetAspect)
	{
		drawHeight = windowHeight;
		drawWidth = windowHeight * targetAspect;
	}
	else
	{
		drawWidth = windowWidth;
		drawHeight = drawWidth / targetAspect;
	}

	if (m_gameRT)
		m_gameRT->Resize((int)drawWidth, (int)drawHeight);

	if (m_captureRT)
		m_captureRT->Resize((int)drawWidth, (int)drawHeight);

	Camera* mainCam = SceneManager::Instance().GetActiveScene()->GetMainCamera();
	if (mainCam) {
		mainCam->SetAspectRatio(targetAspect); 
		mainCam->SetViewDirty();
	}
}

void Game::OnClose()
{
}

void Game::RenderScene(Scene* scene, Camera* camera, RenderTexture* rt)
{

	scene->Render(camera, rt);

	
}
