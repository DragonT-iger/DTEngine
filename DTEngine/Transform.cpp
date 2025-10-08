#include "pch.h"
#include "Transform.h"
#include "SimpleMathHelper.h"

using namespace SimpleMathHelper;

Matrix Transform::GetLocalMatrix()
{
	if (IsDirty()) {
		UpdateMatrices();
	}

	return m_matrixLocal;
}

Matrix Transform::GetWorldMatrix()
{
	if (IsDirty()) {
		UpdateMatrices();
	}

	return m_matrixWorld;
}

void Transform::UpdateMatrices()
{
	m_matrixLocal = Matrix::Identity;
	m_matrixLocal.Translation(m_scale);

	const auto R = D2D1::Matrix3x2F::Rotation(m_rotation);

	const auto T = D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);

	m_matrixLocal = S * R * T;

	if (m_parent)
		m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
	else
		m_matrixWorld = m_matrixLocal;

	m_dirty = false;
}

void Transform::SetDirty()
{
	m_dirty = true;

	for (auto* child : m_children) {
		child->SetDirty();
	}
}

bool Transform::IsDirty()
{
	if (m_dirty) {
		return true;
	}
	return false;
}

const Vector3& Transform::GetPosition()
{
	return m_position;
}

const Vector3& Transform::GetRotation()
{
	return m_rotation;
}

const Vector3& Transform::GetScale()
{
	return m_scale;
}

void Transform::SetPosition(const Vector3& position)
{
	m_position = position;
	SetDirty();
}

void Transform::SetRotation(const Vector3& rotation)
{
	m_rotation = rotation;
	SetDirty();
}

void Transform::SetScale(const Vector3& scale)
{
	m_scale = scale;
	SetDirty();
}

void Transform::SetParent(Transform* parent, bool worldPositionStays)
{
	if (m_parent) {
		if (worldPositionStays == true) {

			Vector3 position;
			Quaternion rotation;
			Vector3 scale;
			
			

			GetWorldMatrix().Decompose(position, rotation, scale);


			m_position = position;

			rotation.

			m_rotation = 
		}
	}
	else {

	}
}

void Transform::AddChild(Transform* child)
{
}

void Transform::RemoveChild(Transform* child)
{
}
