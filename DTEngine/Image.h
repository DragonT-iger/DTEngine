#pragma once
#include "MonoBehaviour.h"
#include "Texture.h"

class Image : public MonoBehaviour
{
    DTGENERATED_BODY(Image);

public:
    Image() = default;
    virtual ~Image() = default;

    void Awake() override;

    void SetTexture(Texture* texture);
    Texture* GetTexture() const;

    void SetColor(const Vector4& color);
    Vector4 GetColor() const;

    void SetNativeSize();

private:
    void SetupRenderer();

    // 저장 데이터
    uint64_t m_textureID = 0;
    Vector4 m_color = { 1.f, 1.f, 1.f, 1.f };
};