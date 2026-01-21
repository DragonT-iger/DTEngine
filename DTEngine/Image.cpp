#include "pch.h"
#include "Image.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "AssetDatabase.h"
#include "Material.h"
#include "Transform.h"

// 리플렉션: 텍스처와 컬러를 에디터에 노출
BEGINPROPERTY(Image)
DTPROPERTY_ACCESSOR(Image, m_textureID, GetTextureID, SetTextureID)
DTPROPERTY_ACCESSOR(Image, m_color, GetColor, SetColor)
DTPROPERTY_ACCESSOR(Image, m_orderInLayer, GetOrderInLayer, SetOrderInLayer)
ENDPROPERTY()

void Image::Awake()
{
    SetupRenderer();

    if (m_textureID != 0)
    {
        std::string path = AssetDatabase::Instance().GetPathFromID(m_textureID);
        Texture* tex = ResourceManager::Instance().Load<Texture>(path);
        SetTexture(tex);
    }
    //SetColor(m_color);
}

void Image::SetupRenderer()
{
    MeshRenderer* mr = GetComponent<MeshRenderer>();
    if (!mr) mr = AddComponent<MeshRenderer>();

    if (mr->GetMesh() == nullptr)
    {
        uint64_t planeID = AssetDatabase::Instance().GetIDFromPath("Assets/Models/Primitives/Plane.fbx");
        if (planeID != 0)
        {
            mr->SetModelID(planeID);
            mr->SetMeshIndex(0);
        }
    }

    if (mr->GetMaterialID() == 0 && mr->GetSharedMaterial() == nullptr)
    {
        uint64_t defaultUIID = AssetDatabase::Instance().GetIDFromPath("Assets/Materials/UI_Default.mat");
        if (defaultUIID != 0)
        {
            mr->SetMaterialID(defaultUIID);
        }
    }

    // Transform* tf = GetTransform();
    // if (tf->GetRotationEuler() == Vector3::Zero) tf->SetRotationEuler({ 90.f, 0.f, 0.f });
}

void Image::SetTexture(Texture* texture)
{
    MeshRenderer* mr = GetComponent<MeshRenderer>();
    if (!mr) return;

    if (texture) m_textureID = texture->GetMeta().guid;
    else m_textureID = 0;

    Material* mat = mr->GetMaterial();
    if (mat)
    {
        mat->SetTexture(0, texture);
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
    MeshRenderer* mr = GetComponent<MeshRenderer>();
    if (mr && mr->GetMaterial()) 
    {
        mr->GetMaterial()->SetColor(color);
    }
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