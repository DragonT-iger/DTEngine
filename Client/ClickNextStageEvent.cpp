#include "ClickNextStageEvent.h"  
#include "UIButton.h"
#include "GameObject.h"
#include "GameManager.h"
#include "ClientSceneManager.h"

BEGINPROPERTY(ClickNextStageEvent)
DTPROPERTY(ClickNextStageEvent, m_button)
DTPROPERTY(ClickNextStageEvent, m_window)
ENDPROPERTY()


void ClickNextStageEvent::Start()
{
		if (!m_button || !m_window)
				return;

        m_button->SetOnClick([this]()
            {
                m_window->SetActive(false);

                int currentLevel = GameManager::Instance()->GetStageLevel();
                // 이거에 따라서 스킬 횟수 증가.
                switch (currentLevel)
                {
                case 1:
                    GameManager::Instance()->GrantHealSkillReward(1);
                    break;
                case 2:
                    GameManager::Instance()->GrantAttackSkillReward(1);
                    break;
                case 3:
                    GameManager::Instance()->GrantHealSkillReward(1);
                    break;
                case 4:
                    GameManager::Instance()->GrantAttackSkillReward(1);
                    break;
                case 5:
                    GameManager::Instance()->GrantHealSkillReward(1);
                    break;
                case 6:
                    GameManager::Instance()->GrantAttackSkillReward(1);
                    break;
                default:
                    break;
                }

                GameManager::Instance()->NextStage();
                if (GameManager::Instance()->GetStageLevel() < 5) {
					ClientSceneManager::Instance().LoadScene("MainGameScene");
                }
                else if (GameManager::Instance()->GetStageLevel() == 6 || GameManager::Instance()->GetStageLevel() == 5)  {
                    ClientSceneManager::Instance().LoadScene("BossDialogue");
                }
                else {
                    ClientSceneManager::Instance().LoadScene("EndingScene");
                }
						
						// 이건 GameManager의 curStage 값을 올리고 그냥 그 씬을 재로드 해야겠지
						//GameManager::Instance()->LoadNextStageFromCurrent();
				});
}
