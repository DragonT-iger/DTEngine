#pragma once
#include "MonoBehaviour.h"
#include <vector>

class Texture;
class MonitorController;
class MeshRenderer;

class MonitorScreen : public MonoBehaviour
{
    DTGENERATED_BODY(MonitorScreen);

public:
    MonitorScreen();
    virtual ~MonitorScreen();

    void Awake() override;
    void Update(float deltaTime) override;

private:
    void UpdateNoiseAnimation(float deltaTime);

private:
    MonitorController* m_monitorController = nullptr;
    MeshRenderer* m_renderer;

    int m_materialSlot = 0;

    float m_noiseTimer = 0.0f;
    int m_currentNoiseIndex = 0;


    bool allPlayed = false;

    float m_noiseSpeed = 0.05f; 
    Texture* m_noise01 = nullptr;
    Texture* m_noise02 = nullptr;
    Texture* m_noise03 = nullptr;
    Texture* m_noise04 = nullptr;
    Texture* m_noise05 = nullptr;
    Texture* m_noise06 = nullptr;
    Texture* m_noise07 = nullptr;
    Texture* m_noise08 = nullptr;
    Texture* m_noise09 = nullptr;
    Texture* m_noise10 = nullptr;
    Texture* m_noise11 = nullptr;

    std::vector<Texture*> m_noiseList;
};