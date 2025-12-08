#include "pch.h"
#include "Text.h"
#include "DX11Renderer.h"
#include "GameObject.h"
#include "Transform.h" 

BEGINPROPERTY(Text)

DTPROPERTY(Text, m_text)
DTPROPERTY(Text, m_color)
DTPROPERTY(Text, m_localOffset)
DTPROPERTY(Text, m_fontSize)

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

void Text::Render()
{
    if (m_text.empty()) return;
    DX11Renderer::Instance().DrawString(m_text, m_localOffset, m_fontSize, m_color);
}