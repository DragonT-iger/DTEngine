#pragma once
#include "MonoBehaviour.h"

class Camera;
class RenderTexture;

class MonitorController : public MonoBehaviour
{
    DTGENERATED_BODY(MonitorController);

public:
    MonitorController();
    virtual ~MonitorController();

    void Awake() override;

    void SetSourceCamera(Camera* cam);
    Camera* GetSourceCamera() const { return m_sourceCamera; }

private:

    std::unique_ptr<RenderTexture> m_renderTexture = nullptr;
    Camera* m_sourceCamera = nullptr;
    int materialSlot = 0;


};