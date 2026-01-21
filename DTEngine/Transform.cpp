#include "pch.h"
#include "Transform.h"
#include "SimpleMathHelper.h"

using namespace SimpleMathHelper;

BEGINPROPERTY(Transform)

DTPROPERTY_ACCESSOR(Transform, m_position, GetPosition, SetPosition)
DTPROPERTY_ACCESSOR(Transform, m_rotation, GetRotationQuat, SetRotationQuat)
DTPROPERTY_ACCESSOR(Transform, m_scale, GetScale, SetScale)
//DTPROPERTY(Transform, kkkl)
DTPROPERTY_ACCESSOR(Transform, m_editorEulerAngles, GetEditorEuler, SetRotationEuler)
DTPROPERTY_SETTER(Transform, m_parent, SetParent)

// DTPROPERTY로 해결이 안되는 경우들.
ENDPROPERTY()




void Transform::ResetValue()
{
	m_position = { 0, 0, 0 };
	m_editorEulerAngles = { 0, 0, 0 };
	m_rotation = { 0, 0, 0 ,1 };
	m_scale	   = { 1 ,1 ,1 };
	MarkDirtyRecursive();
}

const Matrix& Transform::GetLocalMatrix()
{
	if (IsDirty()) {
		UpdateMatrices();
	}

	return m_matrixLocal;
}

const Matrix& Transform::GetWorldMatrix()
{
	if (IsDirty()) {
		UpdateMatrices();
	}

	return m_matrixWorld;
}

const Matrix& Transform::GetWorldInverseTransposeMatrix()
{
	if (IsDirty()) {
		UpdateMatrices();
	}

	return m_matrixWorld.Invert().Transpose();
}

void Transform::UpdateMatrices()
{
	if (!m_dirty) return;

	const Matrix S = Matrix::CreateScale(m_scale);

	const Matrix R = Matrix::CreateFromQuaternion(m_rotation);

	const Matrix T = Matrix::CreateTranslation(m_position);

	m_matrixLocal = S * R * T;

	if (m_parent) {
		m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
	}
	else {
		m_matrixWorld = m_matrixLocal;
	}
	m_dirty = false;
}


void Transform::MarkDirtyRecursive() {
	m_dirty = true;
	for (auto* c : m_children) {
		if (c) c->MarkDirtyRecursive();
	}
}

bool Transform::IsDirty()
{
	if (m_dirty) {
		return true;
	}
	return false;
}

Transform::Transform()
	: m_matrixLocal(IdentityMatrix())
	, m_matrixWorld(IdentityMatrix())
{
	ResetValue();
}

Transform::~Transform()
{
	if (m_parent)
	{
		m_parent->RemoveChild(this);
	}
	for (Transform* child : m_children)
	{
		if (child)
		{
			child->m_parent = nullptr;
		}
	}
}

const Vector3& Transform::GetEditorEuler() const
{
	return m_editorEulerAngles;
}

const Vector3& Transform::GetPosition()
{
	return m_position;
}

const Vector3& Transform::GetRotationEuler()
{
	return QuaternionToEulerDeg_ZXY(m_rotation);
}

const Quaternion& Transform::GetRotationQuat() const
{
	return m_rotation;
}

const Vector3& Transform::GetScale()
{
	return m_scale;
}
Vector3 Transform::GetWorldPosition()
{
	return GetWorldMatrix().Translation();
}
Vector3 Transform::GetWorldScale()
{
	Vector3 scale;
	Quaternion rotation;
	Vector3 translation;

	m_matrixWorld.Decompose(scale, rotation, translation);

	return scale;
}
Vector3 Transform::GetLossyScale()
{
	Vector3 scale = m_scale;
	Transform* p = m_parent;

	while (p != nullptr)
	{
		scale *= p->GetScale();
		p = p->GetParent();
	}

	return scale;
}
const Vector3 Transform::Forward() {
	const Matrix& w = GetWorldMatrix();
	Vector3 f{ w._31, w._32, w._33 };
	f.Normalize(); 
	return f;
}
const Vector3 Transform::Right() {
	const Matrix& w = GetWorldMatrix();
	Vector3 r{ w._11, w._12, w._13 };
	r.Normalize(); 
	return r;
}
const Vector3 Transform::Up() {
	const Matrix& w = GetWorldMatrix();
	Vector3 u{ w._21, w._22, w._23 };
	u.Normalize(); 
	return u;
}
void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
	MarkDirtyRecursive();
}

void Transform::SetRotationQuat(const Quaternion& rotation)
{
	m_rotation = rotation;

	m_editorEulerAngles = QuaternionToEulerDeg_ZXY(m_rotation);

	MarkDirtyRecursive();
}

void Transform::SetRotationEuler(const Vector3& rotation)
{
	m_editorEulerAngles = rotation;

	m_rotation = EulerToQuaternion_ZXY(m_editorEulerAngles);

	MarkDirtyRecursive();
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	MarkDirtyRecursive();
}

void Transform::LookAt(const Vector3& target, const Vector3& worldUp)
{
	Vector3 direction = target - GetPosition();

	Vector3 forward = direction;
	forward.Normalize();

	Matrix targetWorldMatrix = Matrix::CreateWorld(Vector3(0, 0, 0), -forward, worldUp); // RH이니까 그냥 -넣어줬음

	Quaternion targetRotation = Quaternion::CreateFromRotationMatrix(targetWorldMatrix);

	SetRotationQuat(targetRotation);
}

bool Transform::SetParent(Transform* newParent, bool worldPositionStays)
{
	if (newParent == m_parent) return true;

	bool flag = true;

	Matrix oldWorld = GetWorldMatrix();

	if (m_parent) {
		m_parent->RemoveChild(this);
	}

	m_parent = newParent;
	if (m_parent) {
		m_parent->AddChild(this);
	}

	if (worldPositionStays) {
		Matrix parentWorld = (m_parent) ? m_parent->GetWorldMatrix() : IdentityMatrix();
		Matrix invParent = parentWorld.Invert();

		Matrix newLocal = oldWorld * invParent;

		Vector3 s, t;
		Quaternion r;
		
		if (newLocal.Decompose(s, r, t)) {
			m_scale = s;
			m_rotation = r;
			m_position = t;
		}
		else {
			std::cout << "[Warning] Can't decompose matrix in Transform::SetParent\n"; 
			
			// 부모의 스케일이 1 2 1(비등방) 그리고 회전값이 있으면 분해가 안됨. 유니티도 안되더라?  전단인 경우.
			// 언리얼은 됨 스케일 처리 로직이 달라서 보면 스케일 맥이는 방식이 좀 다름
			// 우리는 유니티 방식을 따르기 때문에 처리하지 않을것
			// 만약 모델러가 버텍스 데이터에 스케일이나 Rotation 값을 넣어서 줘서 그걸 파싱해도 이거 뜰꺼임
			// 우리 엔진에서는 처리하지 않음 그냥 모델링 툴에서 Apply Transform 할것 
			// (모든 정점 위치를 옮겨서 스케일과 rot을 적용하는것)
			// 그리고 애초에 충돌처리도 제대로 지원할 수가 없다.
			// PhysX 도 Shear인 경우 지원하지 않는다 카더라.

			MarkDirtyRecursive();
			flag = false;
		}
		
	}

	if(!worldPositionStays) MarkDirtyRecursive();


	UpdateMatrices();

	return flag;
}



void Transform::AddChild(Transform* child)
{
	m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
	auto it = std::remove(m_children.begin(), m_children.end(), child);

	m_children.erase(it, m_children.end());
}
