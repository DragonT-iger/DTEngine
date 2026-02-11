#include "BgFadeController.h"
#include "GameObject.h"
#include "Image.h"
#include "Text.h"
#include "UIButton.h"
#include "RayCastHitEvent.h"
#include "GameManager.h"

BEGINPROPERTY(BgFadeController)
DTPROPERTY(BgFadeController, m_settingButton)
DTPROPERTY(BgFadeController, m_rayObj)
DTPROPERTY(BgFadeController, m_bgObj)
// 아래 두개 gameover 전용임.
DTPROPERTY(BgFadeController, m_gameOverObj)
DTPROPERTY(BgFadeController, m_gameOverMainBtnImg)
DTPROPERTY(BgFadeController, m_lifeImg)
DTPROPERTY(BgFadeController, m_nextRetryImg)
DTPROPERTY(BgFadeController, m_text)
// 아래 두개는 victory 전용.
DTPROPERTY(BgFadeController, m_mushroomSkillimg)
DTPROPERTY(BgFadeController, m_bottleSkillimg)
// 아래 두개만 defeat 전용임. victory 경우 없음.
DTPROPERTY(BgFadeController, m_mainbuttonImg)
DTPROPERTY(BgFadeController, m_exitbuttonImg)
DTPROPERTY(BgFadeController, fadeSpeed)
DTPROPERTY(BgFadeController, startalpha)
DTPROPERTY(BgFadeController, endAlpha)
DTPROPERTY(BgFadeController, delayTime)
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
    if (!isComplete || !m_img)
    {
        // 함수호출하자..
        SettingToggleFinish();

        startalpha += deltaTime * fadeSpeed;

        // 목표치 도달 여부 체크
        if (startalpha >= endAlpha)
        {
            startalpha = endAlpha;

            // 완전히 진해진 "직후"에 수행할 로직
            m_img->SetColor(Vector4(1, 1, 1, startalpha));

            // button들 꺼줘서 처리 안되게 할려고.
            m_rayObj->GetComponent<RayCastHitEvent>()->SetButtons(false);

            time += deltaTime;

            if (time >= delayTime)
            {
                // 여기서 0.5초 추가로 더 계산해주고 이후에 호출.
                
                if (m_fadeType == FadeType::GameOver) 
                {
                    m_gameOverObj->SetActive(true);
                    auto windowImg = m_gameOverObj->GetComponent<Image>();
                    if (windowImg) windowImg->SetActive(true);
                }
                else 
                {
                    m_bgObj->SetActive(true);
                    auto windowImg = m_bgObj->GetComponent<Image>();
                    if (windowImg) windowImg->SetActive(true);
                }

                switch (m_fadeType)
                {
                    case FadeType::Victory:
                    {
                        SoundManager::Instance().PlayOneShot("SFX/Result_Victory_sfx");
                        SetVictoryObj(true); break;
                    }
                    case FadeType::Defeat:
                    {
                        SoundManager::Instance().PlayOneShot("SFX/Result_Defeat_sfx");
                        SetDefeatObj(true);  break;
                    }
                    case FadeType::GameOver: 
                    {
                        SoundManager::Instance().PlayOneShot("SFX/Result_Defeat_sfx");
                        SetGameOverObj(true); break;
                    }
                }
                    isComplete = true;
            }
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

    time = 0;

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
    SetGameOverObj(false);
}

void BgFadeController::SetVictoryObj(bool value)
{
    // 공통부분은 다 꺼주기. 
    if (m_mushroomSkillimg && m_nextRetryImg && m_text)
    {
        isVictoryObj = true;                 // 여기서 체크해줘도 어차피 defeat면 false임. 
        // 둘 중 한개만.
        if (IsMushroomSkillSet())
        {
            m_mushroomSkillimg->SetActive(value);
            m_mushroomSkillimg->GetComponent<UIButton>()->SetActive(value);
            m_bottleSkillimg->SetActive(false);
            m_bottleSkillimg->GetComponent<UIButton>()->SetActive(value);
        }
        else
        {
            m_mushroomSkillimg->SetActive(false);
            m_mushroomSkillimg->GetComponent<UIButton>()->SetActive(value);
            m_bottleSkillimg->SetActive(value);
            m_bottleSkillimg->GetComponent<UIButton>()->SetActive(value);
        }
        
        m_bottleSkillimg->SetActive(value);
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
        m_lifeImg->SetActive(value);
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

void BgFadeController::SetGameOverObj(bool value)
{
    if (m_gameOverObj && m_gameOverMainBtnImg)
    {
        m_gameOverObj->GetComponent<Image>()->SetActive(value);
        m_gameOverMainBtnImg->SetActive(value);
        m_gameOverMainBtnImg->_GetOwner()->GetComponent<UIButton>()->SetActive(value);
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

bool BgFadeController::IsMushroomSkillSet()
{
    int currentLevel = GameManager::Instance()->GetStageLevel();
    // 이거에 따라서 스킬 횟수 증가.
    switch (currentLevel)
    {
    case 1:
        return false;
        break;
    case 2:
        return true;
        break;
    case 3:
        return false;
        break;
    case 4:
        return true;
        break;
    case 5:
        return false;
        break;
    case 6:
        return true;
        break;
    default:
        return false;
        break;
    }
}
