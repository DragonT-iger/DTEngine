#pragma once
#include "IResource.h"
#include <wrl/client.h>
#include <map>
#include <string>

namespace DirectX {
    namespace SimpleMath {
        struct Matrix;
    }
}
using Matrix = DirectX::SimpleMath::Matrix;

struct ID3D11Buffer;

class Shader;
class Texture;

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

private:
    Shader* m_shader = nullptr;

    std::map<int, Texture*> m_textures;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_object;
};