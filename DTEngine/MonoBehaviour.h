#pragma once
#include "Behaviour.h"
/// 날짜 / 이름 / 수정내용
/// 2025-07-02 권용범 최초버전 작성
/// 2025-07-22 물리 Trigger 이벤트 추가

class Collider;

/// <summary>
/// 라이프 사이클을 관리하는 MonoBehaviour 클래스
/// </summary>
class MonoBehaviour : public Behaviour {
public:
    virtual ~MonoBehaviour() = default;

    virtual void Awake() {}
    virtual void Start() {}
	virtual void Update(float deltaTime) {};
	virtual void FixedUpdate(float fixedDelta) {};
	virtual void LateUpdate(float deltaTime) {};

    //virtual void OnTriggerEnter(Collider* other) {} 
    //virtual void OnTriggerStay(Collider* other) {}
    //virtual void OnTriggerExit(Collider* other) {}
};