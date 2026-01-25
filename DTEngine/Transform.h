#pragma once

#include "SimpleMathHelper.h"
#include <vector>

#include "Component.h"


using Vector3 = DirectX::SimpleMath::Vector3;
//using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;


class Transform : public Component
{
	DTGENERATED_BODY(Transform);

public:
	Transform();
	virtual ~Transform();

	// Getter Setter
	const Vector3& GetPosition();
	const Quaternion& GetRotationQuat() const;
	const Vector3& GetEditorEuler() const; // 이걸로 가져와야함
	const Vector3& GetScale();

	Vector3 GetWorldPosition();
	Vector3 GetWorldScale();
	Vector3 GetWorldRotationEuler();

	Vector3 GetLossyScale(); // Sheer인 경우

	void SetPosition(const Vector3& position);
	void SetRotationQuat(const Quaternion& rotation);
	void SetRotationEuler(const Vector3& rotation);
	void SetScale(const Vector3& scale);

	void LookAt(const Vector3& target, const Vector3& worldUp = Vector3::Up);


	bool SetParent(Transform* parent, bool worldPositionStays = false);
	Transform* GetParent() const { return m_parent; } // 에디터에서 이거 private 접근함 수정주의


	const Vector3 Forward();
	const Vector3 Right()  ;
	const Vector3 Up()     ;

	void ResetValue(); // not found는 인텔리센스 오류

	//void Serialize() const override;
	//void Deserialize() override;

	const std::vector<Transform*>& GetChildren() const { return m_children; }





	// 이것도 엔진에서 전용으로 하는걸 생각하고 푼거임

	const Matrix& GetLocalMatrix();
	const Matrix& GetWorldMatrix();
	const Matrix& GetWorldInverseTransposeMatrix();

private:

	const Vector3& GetRotationEuler(); // 웬만하면 EditorEuler 사용하셈 좌표계가 틀린 문제를 해결하긴 했는데
									   // 이건 변환해서 주는거라 정확한 값이 아니기도 해서


	// 순환 참조 예외처리 안했음 public으로 끌고오지 말것
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);

	

	void UpdateMatrices();

	Matrix m_matrixLocal;
	Matrix m_matrixWorld;

	void MarkDirtyRecursive();
	bool IsDirty();
	bool m_dirty = false;


	// 직렬화 해야 하는 값들

	Vector3 m_editorEulerAngles; // 에디터용 저장 변수

	Vector3 m_position;
	
	Quaternion m_rotation;

	Vector3 m_scale;

	Transform* m_parent = nullptr;
	std::vector<Transform*> m_children = {}; // 생각해보니까 이건 안해도 됨
};

