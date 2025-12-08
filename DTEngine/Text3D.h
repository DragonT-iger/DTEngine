//#pragma once
//
//#include <string>
//
//#include "MonoBehaviour.h"
//#include "SimpleMathHelper.h"
//
//
//class Text3D : public MonoBehaviour
//{
//    DTGENERATED_BODY(Text3D);
//
//public:
//    Text3D();
//    virtual ~Text3D();
//
//    void Render();
//
//    void SetText(const std::string& text);
//    void SetText(const std::wstring& text);
//    const std::wstring& GetText() const { return m_text; }
//
//    void SetColor(const Vector4& color) { m_color = color; }
//
//    void SetLocalOffset(const Vector2& offset) { m_localOffset = offset; }
//
//private:
//    std::wstring m_text = L"New 3D Text";
//    Vector4 m_color = Vector4(0.f, 0.f, 0.f, 1.f);
//    Vector2 m_localOffset = Vector2(0.f, 0.f);
//};