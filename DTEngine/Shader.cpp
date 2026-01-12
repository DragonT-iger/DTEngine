#include "pch.h"


#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

#include "Shader.h"
#include "DX11Renderer.h"
#include "DXHelper.h"
#include "Vertex.h" 
#include "MeshRenderer.h"

namespace fs = std::filesystem;

uint16_t Shader::g_ShaderID = 0;

// .cso 파일을 바이너리로 읽어오는 헬퍼 함수
static std::vector<char> ReadFileBlob(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Error: Failed to open shader file: " << filePath << std::endl;
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
    {
        std::cerr << "Error: Failed to read shader file: " << filePath << std::endl;
        return {};
    }
    return buffer;
}


Shader::Shader() = default;
Shader::~Shader() { Unload(); }

bool Shader::LoadFile(const std::string& fullPath)
{
    ID3D11Device* device = DX11Renderer::Instance().GetDevice();
    if (!device) return false;

    

    wchar_t buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    fs::path exePath = fs::path(buffer).parent_path();

    fs::path sourcePath(fullPath);
    std::string filename = sourcePath.stem().string();

    if (filename.size() >= 3)
    {
        std::string suffix = filename.substr(filename.size() - 3);
        if (suffix == "_VS" || suffix == "_PS")
        {
            filename = filename.substr(0, filename.size() - 3);
        }
    }

    fs::path shaderDir = exePath / "Shaders";
    std::string vsPath = (shaderDir / (filename + "_VS.cso")).string();
    std::string psPath = (shaderDir / (filename + "_PS.cso")).string();

    HRESULT hr;

    std::vector<char> vsBlob = ReadFileBlob(vsPath);
    if (vsBlob.empty())
    {
        // 파일이 없을 경우 경로 문제일 수 있으므로 콘솔에 경로를 출력해 확인해보세요.
        // std::cout << "Failed path: " << vsPath << std::endl;
        return false;
    }

    hr = device->CreateVertexShader(vsBlob.data(), vsBlob.size(), nullptr, m_vertexShader.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = device->CreateInputLayout(
        layoutDesc,
        ARRAYSIZE(layoutDesc),
        vsBlob.data(),
        vsBlob.size(),
        m_inputLayout.GetAddressOf()
    );
    DXHelper::ThrowIfFailed(hr);


    // Pixel Shader 로드
    std::vector<char> psBlob = ReadFileBlob(psPath);
    if (psBlob.empty()) return false;

    hr = device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_pixelShader.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);


    m_Shader_ID = ++g_ShaderID;

    return true;
}

bool Shader::SaveFile(const std::string& fullPath)
{
    return false;
}

void Shader::Unload()
{
    m_vertexShader.Reset();
    m_pixelShader.Reset();
    m_inputLayout.Reset();
}

void Shader::Bind() const
{
    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();
    if (!context || !m_vertexShader || !m_pixelShader || !m_inputLayout) return;


    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}