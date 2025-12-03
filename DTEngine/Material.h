#pragma once
#include "IResource.h"
#include <wrl/client.h>
#include <map>
#include <string>
#include <SimpleMath.h>

namespace DirectX {
    namespace SimpleMath {
        struct Matrix;
        struct Vector4;
    }
}
using Matrix = DirectX::SimpleMath::Matrix;
using Vector4 = DirectX::SimpleMath::Vector4;

struct ID3D11Buffer;

class Shader;
class Texture;

struct MaterialData
{
    float Color[4] = { 1,1,1,1 };
    int UseTexture;    
    int Padding[3];    
};

class Material : public IResource
{
public:
    Material();
    virtual ~Material();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    void Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM);

    void SetTexture(int slot, Texture* texture);

    Shader* GetShader() const { return m_shader; }
	void SetShader(Shader* shader) { m_shader = shader; }

    Texture* GetTexture(int slot) const;

    void SetColor(const Vector4& color);
    Vector4 GetColor() const;

private:
    void UpdateMaterialBuffer();

    Shader* m_shader = nullptr;

    std::map<int, Texture*> m_textures;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_object;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_material;

	MaterialData m_data;
};