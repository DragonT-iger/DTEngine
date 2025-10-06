#pragma once
#include "Behaviour.h"

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


    //virtual void OnCollisionEnter(Collider* other) {};
    //virtual void OnCollisionStay(Collider* other) {};
    //virtual void OnCollisionExit(Collider* other) {};

    virtual void OnTriggerEnter(Collider* other) {} 
    virtual void OnTriggerStay(Collider* other) {}
    virtual void OnTriggerExit(Collider* other) {}
};