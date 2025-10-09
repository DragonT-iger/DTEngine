#pragma once

#include <SimpleMath.h>
#include "MonoBehaviour.h"
#include <vector>

using Vector3 = DirectX::SimpleMath::Vector3;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

class Transform : public MonoBehaviour
{
	// Getter Setter
	const Vector3& GetPosition();
	const Vector3& GetRotation();
	const Vector3& GetScale();

	void SetPosition(const Vector3& position);
	void SetRotation(const Vector3& rotation);
	void SetScale(const Vector3& scale);


	void SetParent(Transform* parent, bool worldPositionStays = true);

	const Vector3& Forward();
	const Vector3& Right()  ;
	const Vector3& Up()     ;


	void ResetValue();
	


private:
	// 순환 참조 예외처리 안했음 public으로 끌고오지 말것
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);



	const Matrix& GetLocalMatrix();
	const Matrix& GetWorldMatrix();
	


	void UpdateMatrices();

	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;


	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {};

	Matrix m_matrixLocal = Matrix::Identity;
	Matrix m_matrixWorld = Matrix::Identity;

	void MarkDirtyRecursive();
	bool IsDirty();
	bool m_dirty = false;
};

