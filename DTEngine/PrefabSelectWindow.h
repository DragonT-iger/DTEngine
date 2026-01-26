#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Prefab;
class Image;
class GameObject;

class PrefabSelectWindow : public MonoBehaviour
{
		DTGENERATED_BODY(PrefabSelectWindow);
public:
		using SelectCallback = std::function<void(Prefab*)>;

		PrefabSelectWindow() = default;
		~PrefabSelectWindow() override = default;

		void CreateOnce();   // InitializeWindow + LoadPrefab 1회
		void Open(SelectCallback callback);         // 창 켜고 선택 초기화
		void Close();        // 창 끄기


		void Awake() override;
		// 업데이트는 필요 없을거같음. 필요 시 추가 예정.
		//void Update(float deltaTime) override;
		void UpdateButtonVisuals();

		// 처음 window 전체적으로 생성.
		void InitializeWindow();

		// prefab Load
		void LoadPrefab();

		// 결과 생긴 후 out으로 넘기기.
		bool TryConsumeResult(Prefab*& out);


		Prefab*& GetNightPrefab() { return m_nightPrefab; }
		void SetNightPrefab(Prefab* value) { m_nightPrefab = value; }

		Prefab*& GetRookPrefab() { return m_rookPrefab; }
		void SetRookPrefab(Prefab* value) { m_rookPrefab = value; }

		Prefab*& GetBishopPrefab() { return m_bishopPrefab; }
		void SetBishopPrefab(Prefab* value) { m_bishopPrefab = value; }

		bool IsWindowOpen() { return m_Window->IsActive(); }

private:
		// 본인 가지고있기.
		GameObject* m_Window = nullptr;

		// 콜백 저장용 변수
		SelectCallback m_onResult = nullptr;

		// prefab load해두기 위해서.
		Prefab* m_nightPrefab = nullptr;
		Prefab* m_rookPrefab = nullptr;
		Prefab* m_bishopPrefab = nullptr;

		// image 컴포넌트. visual update 편하게 해주기 위해.
		Image* m_nightImg = nullptr;
		Image* m_rookImg = nullptr;
		Image* m_bishopImg = nullptr;

		// index에 해당하는 prefab 외부로 넘기기 위해.
		Prefab* m_selectPrefab;

		// 계산할 index
		int m_selectIndex = 0;

		// 생성 및 결과 반환을 위해서.
		bool m_created = false;
		bool m_hasResult = false;
};

