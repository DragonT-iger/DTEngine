//#pragma once
//#include "MonoBehaviour.h"
//#include "UIBase.h"
//#include "SimpleMathHelper.h"
//
//class GameObject;
//class Camera;
//class Image;
//class Unit;
//
//class HPBarFollowEvent : public UIBase
//{
//		DTGENERATED_BODY(HPBarFollowEvent);
//public:
//
//		HPBarFollowEvent() = default;
//		~HPBarFollowEvent() = default;
//		void Start() override;
//		void Update(float deltaTime) override;
//
//		void UpdateFollowPosition(float deltaTime);
//		void UpdateFillScale(float deltaTime);
//
//private:
//		// target
//		GameObject* m_targetObject = nullptr;
//		Unit* m_targetUnit = nullptr;
//
//		// 체력바들
//		Image* m_trackImage = nullptr;
//		Image* m_fillImage = nullptr;
//
//		// 메인카메라 캐싱용.
//		Camera* m_camera = nullptr;
//
//		// 체력바 연출 설정.
//		float m_lerpSpeed = 8.0f;       // 값이 클수록 빠르게 따라감.
//		float m_currentRatio = 1.0f;    // 현재 화면에 표시된 비율.
//
//		// 팔로우 위치 오프셋.
//		Vector3 m_worldOffset = Vector3(0.0f, 1.5f, 0.0f);
//		Vector2 m_screenOffset = Vector2(0.0f, 0.0f);
//
//		// fill의 원래 크기.
//		Vector3 m_fillBaseScale = Vector3(1.0f, 1.0f, 1.0f);
//};
//
