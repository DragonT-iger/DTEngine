#include "pch.h"
#include "Effect.h"
#include "DX11Renderer.h"
#include "Animatior.h"

BEGINPROPERTY(Effect)
DTPROPERTY(Effect, m_isAnimating)
DTPROPERTY_SETTER(Effect, m_EM.progress, SetProgress)
DTPROPERTY(Effect, m_EM.edgeWidth)
DTPROPERTY(Effect, m_EM.glowIntensity)
DTPROPERTY_SETTER(Effect, m_EM.edgeColor , SetEdgeColor)
DTPROPERTY(Effect, m_EM.noiseScale)
DTPROPERTY_SETTER(Effect, m_EM.timeMultiplier, SetTimeMultiplier)
ENDPROPERTY()

Effect::Effect()
{
    m_EM = EffectParams();
}

static float g_damaged_interval = 1.0f;

void Effect::Update(float dt)
{
    if (m_isAnimating)
    {
        m_EM.progress += dt * m_EM.timeMultiplier;

        if (m_EM.effectType > 0.0f)
        {
            m_Timer += dt;

            if (m_Timer >= g_damaged_interval)
            {
                m_Timer = 0.f;
                SetEffectType(0.0f); // 효과 종료

            }
        }
    }

   

}

void Effect::BindEP()
{
    auto cmp = this->GetComponent<Animator>();
    if (cmp) m_EM.Skinned_Flag = true;

    DX11Renderer::Instance().UpdateEffect_CBUFFER(m_EM);
}