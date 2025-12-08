#include "pch.h"
#include "Text.h"
#include "DX11Renderer.h"
#include "GameObject.h"


BEGINPROPERTY(Text)

DTPROPERTY(Text, m_text)
DTPROPERTY(Text, m_color)
DTPROPERTY(Text, m_localOffset)

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

    GameObject* owner = _GetOwner();
    if (!owner) return;

    const Matrix& worldMat = owner->GetTransform()->GetWorldMatrix();

    DX11Renderer::Instance().DrawString3D(
        m_text,
        Vector3(m_localOffset.x, m_localOffset.y, 0.f),
        m_color,
        worldMat
    );
}