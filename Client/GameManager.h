#pragma once

#include "MonoSingleton.h"
#include "../DTEngine/SoundManager.h"
class Text;

//우리 엔진의 MonoSingleton은 잘 생각해봐야 하는게 c++ 코드 상에서의 싱글톤이지
//엔진 컴포넌트 상에서는 싱글톤 패턴이 아님. 각자 씬마다의 고유한 인스턴스를 가진다는 뜻임.
//그러므로 static 변수를 사용하지 않는다면 여러 씬에서 공유되는 변수를 만들수 없음.
//즉 컴포넌트 각각의 변수들이 존재할 뿐이지 전역적인 변수가 아니라는 뜻
//DontDestroyOnLoad 개념이 존재하지 않음. 주의해서 사용할 것
//GameManager는 전역적인 여러 상태들을 저장함 ex) 남은 목숨, 점수 등등
class GameManager : public MonoSingleton<GameManager>
{
	DTGENERATED_BODY(GameManager)

public:

	void Start() override;
	void SetLife(int life);
	int GetLife() const;
	
	void SetMoney(int money);
	int GetMoney() const;

	void Update(float deltaTime) override;
	
	void SetUnitCount(int count) { curUnitCount = count; }
	int GetUnitCount() const { return curUnitCount; }

	void SetBgmValue(float value) { curbgmValue = value; SoundManager::Instance().SetBGMVolume(value); }
	float GetBgmValue() const { return curbgmValue; }

	void SetSfxValue(float value) { cursfxValue = value; SoundManager::Instance().SetSFXVolume(value);}
	float GetSfxValue() const { return cursfxValue; }



private:
	
	Text* m_money;
	Text* m_life;

	static inline int maxLife = 9;
	static inline int curLife = 9;

	static inline int curMoney = 9; //cost는 좀 짜친데
	static inline int curUnitCount = 0;		// 3 초과부터는 유닛 소환 불가능으로. -> 변할 수도 있음. 

	static inline float curbgmValue = 0.5f;		
	static inline float cursfxValue = 0.5f;
	
};

