#pragma once

#include <string>
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"

class Text : public MonoBehaviour
{
    DTGENERATED_BODY(Text);

public:
    Text();
    virtual ~Text();

    void Render();

    void SetText(const std::string& text);
    void SetText(const std::wstring& text);
    const std::wstring& GetText() const { return m_text; }

    void SetColor(const Vector4& color) { m_color = color; }

    void SetLocalOffset(const Vector2& offset) { m_localOffset = offset; }

	void SetFontSize(float size) { m_fontSize = size; }
	float GetFontSize() const { return m_fontSize; }

private:
    std::wstring m_text = L"New Text";
    Vector4 m_color = Vector4(1.f, 1.f, 1.f, 1.f);
    Vector2 m_localOffset = Vector2(0.f, 0.f);
	float m_fontSize = 1.0f;
};