#pragma once
#include "IResource.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Shader : public IResource
{
public:
    Shader();
    virtual ~Shader();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;
    void Bind() const;
    uint16_t GetID() const { return m_Shader_ID; }


    static uint16_t g_ShaderID;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>   m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>    m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>    m_inputLayout;


    uint16_t m_Shader_ID =0;
};