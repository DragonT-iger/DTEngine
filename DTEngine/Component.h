/// 날짜 / 이름 / 수정내용
/// 2025-07-02 권용범 최초버전 작성
/// 2025-07-09 권용범 MonoBehaviour 에서 바로 접근할수 있도록 GetOwner() 함수 추가 this로 접근하게 해보려고도 했는데 헤더 순환 참조 발생했음

#pragma once

class GameObject;

/// <summary>
/// 컴포넌트 클래스
/// </summary>
class Component {
public:

	Component() = default;
    virtual ~Component() = default;

	GameObject* GetOwner() const { return m_owner; }

    void Destroy(GameObject* gameObject);

protected:
    void SetOwner(GameObject* owner) { m_owner = owner; }

private:
    GameObject* m_owner = nullptr;
};