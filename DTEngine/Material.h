#pragma once
#include "IResource.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h> // Matrix 용

using Matrix = DirectX::SimpleMath::Matrix;

class Shader; // 전방 선언

class Material : public IResource
{
public:
    Material();
    virtual ~Material();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    void Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM);

    Shader* GetShader() const { return m_shader; }


private:
    __declspec(align(16))
        struct CBuffer_Object_Data
    {
        Matrix WorldTM;
        Matrix WorldInverseTransposeTM;
    };

    Shader* m_shader = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer_object;
};