#include "pch.h"
#include "Image.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "AssetDatabase.h"
#include "Transform.h"
#include "Effect.h"
#include "DX11Renderer.h"


BEGINPROPERTY(Image)
DTPROPERTY_ACCESSOR(Image, m_textureID, GetTextureID, SetTextureID)
DTPROPERTY_ACCESSOR(Image, m_color, GetColor, SetColor)
DTPROPERTY(Image, m_orderInLayer)
ENDPROPERTY()

void Image::Awake()
{
    if (m_textureID != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(m_textureID);
        Texture* tex = ResourceManager::Instance().Load<Texture>(path);

        SetTexture(tex);

    }
}


void Image::SetTexture(Texture* texture)
{
    if (texture)
    {
        m_textureID = texture->GetMeta().guid;
    }
    else
    {
        m_textureID = 0;
    }
}

Texture* Image::GetTexture() const
{
    if (m_textureID == 0) return nullptr;
    std::string path = AssetDatabase::Instance().GetPathFromID(m_textureID);
    return ResourceManager::Instance().Load<Texture>(path);
}

void Image::SetColor(const Vector4& color)
{
    m_color = color;
}

const Vector4& Image::GetColor() const
{
    return m_color;
}

void Image::SetTextureID(uint64_t id)
{
    m_textureID = id;

    if (id != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(id);
        Texture* tex = ResourceManager::Instance().Load<Texture>(path);
        SetTexture(tex);

        
        //SetNativeSize(); 
    }
    else
    {
        SetTexture(nullptr);
    }
}

void Image::SetNativeSize()
{
    Texture* tex = GetTexture();
    if (!tex) return;

    GetTransform()->SetScale(Vector3((float)tex->GetWidth(), (float)tex->GetHeight(), 1.0f));
}

void Image::RenderUI()
{
    Texture* tex = GetTexture();
    if (tex)
    {
        Effect* eff = GetComponent<Effect>();
        if (eff) eff->BindEP();

        Transform* tf = GetTransform();

        Vector3 pos = tf->GetWorldPosition();
        Vector3 scale = tf->GetWorldScale();

        float rotationRad = DirectX::XMConvertToRadians(tf->GetWorldRotationEuler().z);

        DX11Renderer::Instance().DrawUI(
            tex,
            Vector2(pos.x, pos.y),
            Vector2(scale.x, scale.y),
            m_color,
            rotationRad 
        );
    }
}
