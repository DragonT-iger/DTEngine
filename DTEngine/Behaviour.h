#pragma once
#include "Component.h"
/// 날짜 / 이름 / 수정내용
/// 2025-07-02 권용범 최초버전 작성
/// 하다보니 Enable 이런건 안써서 삭제

/// <summary>
/// 컴포넌트를 껐다 켤수 있음
/// </summary>
class Behaviour : public Component {
public:
    Behaviour() : m_active(true) {}
    virtual ~Behaviour() = default;

    void SetActive(bool state) { m_active = state; }
    bool IsActive() const { return m_active; }

private:
    bool m_active;
};
