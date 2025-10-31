#pragma once

#include <SimpleMath.h>
#include <vector>

#include "Component.h"


using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;


class Transform : public Component
{
	DTGENERATED_BODY(Transform);

public:
	Transform();
	// Getter Setter
	const Vector3& GetPosition();
	const Vector3& GetRotationEuler();
	const Quaternion& GetRotationQuat() const;
	const Vector3& GetScale();

	void SetPosition(const Vector3& position);
	void SetRotationQuat(const Quaternion& rotation);
	void SetRotationEuler(const Vector3& rotation);
	void SetScale(const Vector3& scale);


	void SetParent(Transform* parent, bool worldPositionStays = true);
	Transform* GetParent() const { return m_parent; } // 에디터에서 이거 private 접근함 주의

	const Vector3 Forward();
	const Vector3 Right()  ;
	const Vector3 Up()     ;

	void ResetValue();

	//void Serialize() const override;
	//void Deserialize() override;

private:
	// 순환 참조 예외처리 안했음 public으로 끌고오지 말것
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);

	const Matrix& GetLocalMatrix();
	const Matrix& GetWorldMatrix();

	void UpdateMatrices();

	Matrix m_matrixLocal;
	Matrix m_matrixWorld;

	void MarkDirtyRecursive();
	bool IsDirty();
	bool m_dirty = false;


	// 직렬화 해야 하는 값들

	Vector3 m_position;
	Quaternion m_rotation;
	//DTPROPERTY(Quaternion, m_rotation);
	Vector3 m_scale;
	//DTPROPERTY(Vector3, m_scale);

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {};
};

