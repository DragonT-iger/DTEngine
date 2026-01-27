#include "pch.h"
#include "RuleSelectWindow.h"
#include "GameObject.h"
#include "UIButton.h"
#include "Image.h"
#include "../Client/AllyUnit.h"
#include "../Client/Unit.h"
#include "Texture.h"


BEGINPROPERTY(RuleSelectWindow)
DTPROPERTY(RuleSelectWindow, m_Window)
DTPROPERTY(RuleSelectWindow, m_titleBarImg)
DTPROPERTY(RuleSelectWindow, m_rookTex)
DTPROPERTY(RuleSelectWindow, m_knightTex)
DTPROPERTY(RuleSelectWindow, m_bishopTex)
ENDPROPERTY()

void RuleSelectWindow::CreateOnce()
{
}

void RuleSelectWindow::InitializeWindow()
{
}


void RuleSelectWindow::Close()
{
    if (m_Window) m_Window->SetActive(false);
}

void RuleSelectWindow::UpdateButtonVisuals()
{
}
