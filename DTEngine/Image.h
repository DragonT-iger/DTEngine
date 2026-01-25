#pragma once
#include "MonoBehaviour.h"
#include "UIBase.h"
#include "Texture.h"
#include "SimpleMathHelper.h"

class Image : public UIBase
{
    DTGENERATED_BODY(Image);

public:
    Image() = default;
    virtual ~Image() = default;

    void Awake() override;

    void SetTexture(Texture* texture);
    Texture* GetTexture() const;

    void SetColor(const Vector4& color);
    const Vector4& GetColor() const;

    void SetTextureID(uint64_t id);
    const uint64_t& GetTextureID() const { return m_textureID; }
    
    //void SetOrderInLayer(int order) { m_orderInLayer = order; }
    //const int& GetOrderInLayer() const { return m_orderInLayer; }

    void SetNativeSize();

	void RenderUI() override;

private:

    uint64_t m_textureID = 0;
    Vector4 m_color = { 1.f, 1.f, 1.f, 1.f };
    //int     m_orderInLayer = 0;
};