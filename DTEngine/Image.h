#pragma once
#include "MonoBehaviour.h"
#include "Texture.h"
#include "SimpleMathHelper.h"

class Image : public MonoBehaviour
{
    DTGENERATED_BODY(Image);

public:
    Image();
    virtual ~Image() = default;

    //void Awake() override;
    //void Start() override;

    //void LateUpdate(float deltaTime) override;

    void Render();

    void SetTexture(Texture* texture) { m_texture = texture; }
    void SetColor(const Vector4& color) { m_color = color; }

private:
    Texture* m_texture = nullptr;   
    Vector4  m_color = { 1.f, 1.f, 1.f, 1.f };
};