#include "BgFadeController.h"
#include "GameObject.h"
#include "Image.h"
#include "Text.h"
#include "UIButton.h"
#include "RayCastHitEvent.h"


BEGINPROPERTY(BgFadeController)
DTPROPERTY(BgFadeController, m_settingButton)
DTPROPERTY(BgFadeController, m_rayObj)
DTPROPERTY(BgFadeController, m_bgObj)
DTPROPERTY(BgFadeController, m_skillimg)
DTPROPERTY(BgFadeController, m_nextRetryImg)
DTPROPERTY(BgFadeController, m_text)

// 아래 두개만 defeat 전용임. victory 경우 없음.
DTPROPERTY(BgFadeController, m_mainbuttonImg)
DTPROPERTY(BgFadeController, m_exitbuttonImg)
DTPROPERTY(BgFadeController, fadeSpeed)
DTPROPERTY(BgFadeController, startalpha)
DTPROPERTY(BgFadeController, endAlpha)
ENDPROPERTY()

void BgFadeController::Start()
{
    Init();
}

void BgFadeController::Update(float deltaTime)
{
    BGFadeUpdate(deltaTime);
}

void BgFadeController::BGFadeUpdate(float deltaTime)
{
    if (!isComplete)
    {
        // 함수호출하자..
        SettingToggleFinish();

        startalpha += deltaTime * fadeSpeed;

        // 목표치 도달 여부 체크
        if (startalpha >= endAlpha)
        {
            startalpha = endAlpha;
            isComplete = true;

            // 완전히 진해진 "직후"에 수행할 로직
            m_img->SetColor(Vector4(1, 1, 1, startalpha));
            m_bgObj->SetActive(true);
            auto windowImg = m_bgObj->GetComponent<Image>();
            windowImg->SetActive(true);

            if (isVictoryObj)
                SetVictoryObj(true);
            else
                SetDefeatObj(true);

            // button들 꺼줘서 처리 안되게 할려고.
            m_rayObj->GetComponent<RayCastHitEvent>()->SetButtons(false);
            return;
        }

        // 색상만 적용.
        m_img->SetColor(Vector4(1, 1, 1, startalpha));
    }
}

void BgFadeController::Init()
{
    // 배경 오브젝트는 키는데 img만 꺼주기.
    if (!m_bgObj)
        return;
    m_bgObj->SetActive(true);

    auto windowImg = m_bgObj->GetComponent<Image>();

    if (windowImg)
    {
        windowImg->SetActive(false);
    }

    auto img = _GetOwner()->GetComponent<Image>();

    if (img)
    {
        // startalpha값으로 set 먼저.
        m_img = img;
        m_img->SetColor(Vector4(1, 1, 1, startalpha));
    }

    SetVictoryObj(false);
    SetDefeatObj(false);
}

void BgFadeController::SetVictoryObj(bool value)
{
    // 공통부분은 다 꺼주기. 
    if (m_skillimg && m_nextRetryImg && m_text)
    {
        isVictoryObj = true;                 // 여기서 체크해줘도 어차피 defeat면 false임. 
        m_skillimg->SetActive(value);
        m_nextRetryImg->SetActive(value);
        m_text->SetActive(value);

        m_nextRetryImg->_GetOwner()->GetComponent<UIButton>()->SetActive(value);
    }
}

void BgFadeController::SetDefeatObj(bool value)
{
    // defeat 전용들 있는 경우 꺼주기.
    if (m_mainbuttonImg && m_exitbuttonImg)
    {
        isVictoryObj = false;
        m_skillimg->SetActive(value);
        m_nextRetryImg->SetActive(value);
        m_text->SetActive(value);
        m_mainbuttonImg->SetActive(value);
        m_exitbuttonImg->SetActive(value);

        // button도 꺼줘야함. 안꺼주면 다 입력받음.
        m_nextRetryImg->_GetOwner()->GetComponent<UIButton>()->SetActive(value);
        m_mainbuttonImg->_GetOwner()->GetComponent<UIButton>()->SetActive(value);
        m_exitbuttonImg->_GetOwner()->GetComponent<UIButton>()->SetActive(value);
    }
}

void BgFadeController::SettingToggleFinish()
{
    if (!m_settingButton)
        return;
    
    if (!setFinish)
    {
        // button 꺼버리기. toggle 로직 자체를 막아버리기.
        m_settingButton->GetComponent<UIButton>()->SetActive(false);
        m_rayObj->GetComponent<RayCastHitEvent>()->SetGameFinish(true);
        setFinish = true;
    }
}
