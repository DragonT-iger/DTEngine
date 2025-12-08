#include "pch.h"
//#include "Text3D.h"
//#include "DX11Renderer.h"
//#include "GameObject.h"
//
//
//BEGINPROPERTY(Text3D)
//
//DTPROPERTY(Text3D, m_text)
//DTPROPERTY(Text3D, m_color)
//DTPROPERTY(Text3D, m_localOffset)
//
//ENDPROPERTY()
//
//
//Text3D::Text3D() = default;
//Text3D::~Text3D() = default;
//
//void Text3D::SetText(const std::string& text)
//{
//    m_text.assign(text.begin(), text.end());
//}
//
//void Text3D::SetText(const std::wstring& text)
//{
//    m_text = text;
//}
//
//void Text3D::Render()
//{
//    if (m_text.empty()) return;
//
//    GameObject* owner = _GetOwner();
//    if (!owner) return;
//
//    const Matrix& worldMat = owner->GetTransform()->GetWorldMatrix();
//
//    DX11Renderer::Instance().DrawString3D(
//        m_text,
//        Vector3(m_localOffset.x, m_localOffset.y, 0.f),
//        m_color,
//        worldMat
//    );
//}