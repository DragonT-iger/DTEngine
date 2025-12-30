#pragma once
#include "IResource.h"
#include <wrl/client.h>
#include <vector>
#include <string>
#include "SimpleMathHelper.h"


struct ID3D11Buffer;

class Shader;
class Texture;

__declspec(align(16))
struct MaterialData 
{
    Vector4 Color = { 1,1,1,1 };
    Vector4 UVTransform = { 1, 1, 0, 0 }; 
    int UseTexture;
    int Padding[3];    
};

enum class RenderMode { Opaque, Transparent };
enum class CullMode { Back, Front, None };

class Material : public IResource
{
public:
    Material();
    virtual ~Material();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    void Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM);

    bool SetTexture(int slot, Texture* texture);

    Shader* GetShader() const { return m_shader; }
	void SetShader(Shader* shader) { m_shader = shader; }

    Material* Clone();

    Texture* GetTexture(int slot) const;

    bool SetColor(const Vector4& color);
    Vector4 GetColor() const;

    Vector2 GetTiling() const { return Vector2(m_data.UVTransform.x, m_data.UVTransform.y); }
    Vector2 GetOffset() const { return Vector2(m_data.UVTransform.z, m_data.UVTransform.w); }

    void SetTiling(float x, float y) { m_data.UVTransform.x = x; m_data.UVTransform.y = y; UpdateMaterialBuffer(); }
    void SetOffset(float x, float y) { m_data.UVTransform.z = x; m_data.UVTransform.w = y; UpdateMaterialBuffer(); }

    void SetRenderMode(RenderMode mode) { m_renderMode = mode; }
    RenderMode GetRenderMode() const { return m_renderMode; }

    void SetCullMode(CullMode mode) { m_cullMode = mode; }
    CullMode GetCullMode() const { return m_cullMode; }

	static constexpr int MAX_TEXTURE_SLOTS = 8;

private:
    void UpdateMaterialBuffer();
    void CreateBuffers();
    void SetDefaultShader();

    Shader* m_shader = nullptr;

    std::vector<Texture*> m_textures;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_object;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_material;

	MaterialData m_data;

    RenderMode m_renderMode = RenderMode::Opaque;

    CullMode m_cullMode = CullMode::Back;
};