#pragma once
#include "MonoBehaviour.h"
#include "RenderTexture.h"
#include <memory>

class Camera;
class GameObject;

class ReflectionProbe : public MonoBehaviour
{
    DTGENERATED_BODY(ReflectionProbe)
public:
    ReflectionProbe();
    virtual ~ReflectionProbe();

    void Awake() override;
    void Render();

  //  void SetTargetTextureSlot(int slot) { m_targetSlot = slot; }

private:
    std::unique_ptr<RenderTexture> m_cubeMap;
    std::unique_ptr<GameObject> m_cameraObject;

    Camera* m_captureCamera = nullptr;

     int m_targetSlot = 0; //보류 
 
};