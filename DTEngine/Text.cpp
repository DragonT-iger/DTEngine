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
    Vector3 W_pos = this->GetTransform()->GetWorldPosition();
    Matrix view = DX11Renderer::Instance().GetViewMatrix();
    Matrix Proj = DX11Renderer::Instance().GetProjectionMatrix();
    float Width = DX11Renderer::Instance().GetWidth();
    float Height = DX11Renderer::Instance().GetHeight();

    //World -> ndc 변환. 

    Vector3 screenPos = XMVector3Project(
        W_pos,
        0, 0, Width, Height,
        0.0f, 1.0f,
        Proj,
        view,
        Matrix()
    );

    if (screenPos.z < 0.0f || screenPos.z > 1.0f)
        return;

    Vector2 finalPos(screenPos.x, screenPos.y);
    finalPos += m_localOffset;

    if (m_Font)
    {
        DX11Renderer::Instance().DrawString(
            m_Font,     
            m_text,     
            finalPos,   
            m_fontSize, 
            m_color     
        );
    }
    else
    {
        DX11Renderer::Instance().DrawString(
            m_text,
            finalPos,
            m_fontSize,
            m_color
        );
    }

}