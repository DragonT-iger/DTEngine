#include "pch.h"
#include "Text.h"
#include "DX11Renderer.h"
#include "Transform.h" 

#include "Font.h"

BEGINPROPERTY(Text)

DTPROPERTY(Text, m_text)
DTPROPERTY(Text, m_color)
DTPROPERTY(Text, m_localOffset)
DTPROPERTY(Text, m_fontSize)
DTPROPERTY_SETTER(Text, m_FontID , SetFont)

ENDPROPERTY()

Text::Text() = default;
Text::~Text() = default;

void Text::SetText(const std::string& text)
{
    m_text.assign(text.begin(), text.end());
}

void Text::SetText(const std::wstring& text)
{
    m_text = text;
}

void Text::SetFont(uint64_t Font_ID)
{

    if (Font_ID == m_FontID) return;

    m_FontID = Font_ID;

    std::string path = AssetDatabase::Instance().GetPathFromID(Font_ID);

    Font* font = ResourceManager::Instance().Load<Font>(path);

    if (font) m_Font = font;

}

void Text::Render()
{
    if (m_text.empty()) return;

    if(!m_Font) //없다면 기본으로 
    DX11Renderer::Instance().DrawString(m_text, m_localOffset, m_fontSize, m_color);

    else
    {
        DX11Renderer::Instance().DrawString(m_Font, m_text, m_localOffset, m_fontSize, m_color);
    }

}