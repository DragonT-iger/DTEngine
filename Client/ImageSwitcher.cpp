#include "ImageSwitcher.h"
#include "GameObject.h"
#include "Image.h"
#include "GameManager.h"
#include "AssetDatabase.h"


BEGINPROPERTY(ImageSwitcher)
DTPROPERTY(ImageSwitcher, m_obj)
ENDPROPERTY()

void ImageSwitcher::Start()
{
		if (!m_obj)
				return;

		m_img = m_obj->GetComponent<Image>();

		int curStage = GameManager::Instance()->GetStageLevel();
		switch (curStage)
		{
		case 1:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_1.png"));
				break;
		case 2:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_2.png"));
				break;
		case 3:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_3.png"));
				break;
		case 4:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_4.png"));
				break;
		case 5:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_5.png"));
				break;
		case 6:
				m_img->SetTextureID(AssetDatabase::Instance().GetIDFromPath("Assets/Models/UI/Alice_UI/LastCheckImg/Stage_6.png"));
				break;
		default:
				break;
		}

}
