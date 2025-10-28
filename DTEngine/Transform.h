#pragma once

#include <SimpleMath.h>
#include "MonoBehaviour.h"
#include <vector>


using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;


class Transform : public MonoBehaviour
{
	DTCLASS(Transform);

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
	Transform* GetParent() const { return m_parent; }

	const Vector3 Forward();
	const Vector3 Right()  ;
	const Vector3 Up()     ;

	void ResetValue();

	void Serialize(JsonWriter& w) const override;
	void Deserialize(JsonReader& r) override;

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
	inline static bool s_propRegistered_m_position = []() { \
		ReflectionDatabase::Instance().RegisterDTPROPERTY(\
			ThisType::GetStaticTypeName(),  /* "Transform" */ \
			"m_position",                          /* "m_position" */ \
			typeid(m_position),                   /* typeid(Vector3) */ \
			offsetof(ThisType, m_position)        /* offsetof(Transform, m_position) */ \
		); \
		return true; \
		}();
	Quaternion m_rotation;
	//DTPROPERTY(Quaternion, m_rotation);
	Vector3 m_scale;
	//DTPROPERTY(Vector3, m_scale);

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {};
};

