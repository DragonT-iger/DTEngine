#include "pch.h"
#include "MonitorScreen.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "MonitorController.h"
#include "RenderTexture.h"

BEGINPROPERTY(MonitorScreen)
DTPROPERTY(MonitorScreen, m_materialSlot)
DTPROPERTY(MonitorScreen, m_noise01)
DTPROPERTY(MonitorScreen, m_noise02)
DTPROPERTY(MonitorScreen, m_noise03)
DTPROPERTY(MonitorScreen, m_noise04)
DTPROPERTY(MonitorScreen, m_noise05)
DTPROPERTY(MonitorScreen, m_noise06)
DTPROPERTY(MonitorScreen, m_noise07)
DTPROPERTY(MonitorScreen, m_noise08)
DTPROPERTY(MonitorScreen, m_noise09)
DTPROPERTY(MonitorScreen, m_noise10)
DTPROPERTY(MonitorScreen, m_noise11)
ENDPROPERTY()

MonitorScreen::MonitorScreen() = default;
MonitorScreen::~MonitorScreen() = default;

void MonitorScreen::Awake()
{
    m_monitorController = GetComponent<MonitorController>();
    m_renderer = GetComponent<MeshRenderer>();

    if (m_noise01) m_noiseList.push_back(m_noise01);
    if (m_noise02) m_noiseList.push_back(m_noise02);
    if (m_noise03) m_noiseList.push_back(m_noise03);
    if (m_noise04) m_noiseList.push_back(m_noise04);
    if (m_noise05) m_noiseList.push_back(m_noise05);
    if (m_noise06) m_noiseList.push_back(m_noise06);
    if (m_noise07) m_noiseList.push_back(m_noise07);
    if (m_noise08) m_noiseList.push_back(m_noise08);
    if (m_noise09) m_noiseList.push_back(m_noise09);
    if (m_noise10) m_noiseList.push_back(m_noise10);
    if (m_noise11) m_noiseList.push_back(m_noise11);
}

void MonitorScreen::Update(float deltaTime)
{
    if (allPlayed) {

        m_RTrenderTimer += deltaTime;

        m_monitorController->SetSourceCamera(m_monitorController->GetSourceCamera()); // 이거 최적화 안되긴 했음

        if (m_RTrenderTimer >= 2.0f) {
            

            allPlayed = false;
            m_RTrenderTimer -= 2.0f;
        }
        
    }
    else {
        UpdateNoiseAnimation(deltaTime);
    }
}

void MonitorScreen::UpdateNoiseAnimation(float deltaTime)
{
    if (m_noiseList.empty()) return;

    m_noiseTimer += deltaTime;

    if (m_noiseTimer >= m_noiseSpeed)
    {
        m_noiseTimer = 0.0f;

        if (m_currentNoiseIndex == m_noiseList.size() - 1) {
            allPlayed = true;
        }

        m_currentNoiseIndex = (m_currentNoiseIndex + 1) % m_noiseList.size();
        Texture* nextNoise = m_noiseList[m_currentNoiseIndex];

        
        if (m_renderer && m_renderer->GetSharedMaterial())
        {
            m_renderer->GetMaterial()->SetTexture(m_materialSlot, nextNoise);
        }
    }
}