#pragma once
#include "IResource.h"
#include <wrl/client.h>
#include <vector>
#include <string>
#include "SimpleMathHelper.h"

#include "ConstantBuffers.h"


class Shader;
class Texture;


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

    void BindPipeLine();
    void BindPerObject(const Matrix& worldTM, const Matrix& WorldINVTM);
    
    bool SetTexture(int slot, Texture* texture);

    Shader* GetShader() const { return m_shader; }
	void SetShader(Shader* shader) { m_shader = shader; }

    Material* Clone();

    Texture* GetTexture(int slot) const;

    bool SetColor(const Vector4& color);
    Vector4 GetColor() const;

    Vector2 GetTiling() const { return Vector2(m_data.UVTransform.x, m_data.UVTransform.y); }
    Vector2 GetOffset() const { return Vector2(m_data.UVTransform.z, m_data.UVTransform.w); }

    void SetTiling(float x, float y) { m_data.UVTransform.x = x; m_data.UVTransform.y = y; }
    void SetOffset(float x, float y) { m_data.UVTransform.z = x; m_data.UVTransform.w = y; }

    void SetRenderMode(RenderMode mode) { m_renderMode = mode; }
    RenderMode GetRenderMode() const { return m_renderMode; }

    void SetCullMode(CullMode mode) { m_cullMode = mode; }
    CullMode GetCullMode() const { return m_cullMode; }

    void SetShadowBias(float bias);
    float GetShadowBias() const;

	static constexpr int MAX_TEXTURE_SLOTS = 8;
   

    uint16_t GetShaderID();
    uint64_t GetTextureID();
private:
    void UpdateTextureBatchID();
    void SetDefaultShader();

    Shader* m_shader = nullptr;

    std::vector<Texture*> m_textures;

	MaterialData m_data;

    RenderMode m_renderMode = RenderMode::Opaque;

    CullMode m_cullMode = CullMode::Back;

    uint64_t m_textureBatchID = 0;


};