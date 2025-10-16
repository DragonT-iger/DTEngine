#include "pch.h"
#include "Transform.h"
#include "SimpleMathHelper.h"

using namespace SimpleMathHelper;

void Transform::ResetValue()
{
	m_position = { 0, 0, 0 };
	m_rotation = { 0, 0, 0 };
	m_scale	   = { 1 ,1 ,1 };
	MarkDirtyRecursive();
}

void Transform::Serialize(JsonWriter& w) const
{
	w.WriteVec3("position", m_position.x, m_position.y, m_position.z);
	w.WriteVec3("rotationEuler", m_rotation.x, m_rotation.y, m_rotation.z);
	w.WriteVec3("scale", m_scale.x, m_scale.y, m_scale.z);
}

void Transform::Deserialize(JsonReader& r)
{
	auto p = r.ReadVec3("position", { 0,0,0 });
	auto e = r.ReadVec3("rotationEuler", { 0,0,0 });
	auto s = r.ReadVec3("scale", { 1,1,1 });
	m_position = Vector3(p[0], p[1], p[2]);
	m_rotation = EulerToQuaternion_ZXY(Vector3(e[0], e[1], e[2]));
	m_scale = Vector3(s[0], s[1], s[2]);
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
	MarkDirtyRecursive();
}

void Transform::SetRotationEuler(const Vector3& rotation)
{
	m_rotation = EulerToQuaternion_ZXY(rotation);
	MarkDirtyRecursive();
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	MarkDirtyRecursive();
}

void Transform::SetParent(Transform* newParent, bool worldPositionStays)
{
	if (newParent == m_parent) return;

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
		newLocal.Decompose(s, r, t);
		m_scale = s;
		m_rotation = r; 
		m_position = t;
	}

	UpdateMatrices();
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
