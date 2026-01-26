#pragma once
#include "Singleton.h" 
#include <string>
#include <unordered_map>
#include <functional>

class ClientSceneManager : public Singleton<ClientSceneManager>
{
public:
    void LoadScene(const std::string& sceneName);

private:
    void InitTitleScene();
    void InitInGameScene();
    void InitEndingScene();

};



// SceneManager가 있는데 굳이 왜 랩핑을 하냐?
// 특정 씬을 로드할때 고유한 초기값 세팅을 해야 할 일이 꼭 생김 그걸 적을 곳이 없음.
// 그렇기 때문에 모든 씬 전환은 ClientSceneManager를 통해서 하세요. 일괄 처리하기 좋음이거