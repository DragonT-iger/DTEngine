#pragma once

#include <string>
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include "UIBase.h"

class Font;

class Text : public UIBase
{
    DTGENERATED_BODY(Text);

public:
    Text();
    virtual ~Text();

    void Render();
    void RenderUI() override;

    //void SetText(const std::string& text);
    void SetText(const std::wstring& text);

    void SetFont(uint64_t Font_ID);

    const std::wstring& GetText() const { return m_text; }

    void SetColor(const Vector4& color) { m_color = color; }

    //void SetLocalOffset(const Vector2& offset) { m_localOffset = offset; }

	void SetFontSize(float size) { m_fontSize = size; }
	float GetFontSize() const { return m_fontSize; }

private:
    std::wstring m_text = L"New Text";
    Vector4 m_color = Vector4(1.f, 1.f, 1.f, 1.f);
    //Vector2 m_localOffset = Vector2(0.f, 0.f);
	float m_fontSize = 1.0f;


    Font* m_Font = nullptr;
    uint64_t m_FontID= 0 ;
};

//Editor에서는 IResource인 Font를 받아서 처리. 