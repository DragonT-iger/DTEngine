#include "pch.h"

#include <iostream>

#include "Material.h"
#include "Shader.h"
#include "ResourceManager.h" 
#include "DX11Renderer.h"    
#include "DXHelper.h"
#include "SimpleMathHelper.h"

Material::Material() = default;
Material::~Material() { Unload(); }

bool Material::LoadFile(const std::string& fullPath)
{
    m_shader = ResourceManager::Instance().Load<Shader>("../" +fullPath);
    if (!m_shader)
    {
        std::cerr << "Error: Material failed to load shader: " << fullPath << std::endl;
        return false;
    }

    ID3D11Device* device = DX11Renderer::Instance().GetDevice();
    if (!device) return false;

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(CBuffer_Object_Data);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, m_cbuffer_object.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    return true;
}

bool Material::SaveFile(const std::string& fullPath)
{
    return false;
}

void Material::Unload()
{
    m_cbuffer_object.Reset();
    m_shader = nullptr; // 참조만 하므로 delete하지 않음
}

void Material::Bind(const Matrix& worldTM, const Matrix& worldInverseTransposeTM)
{
    if (!m_shader || !m_cbuffer_object) return;

    ID3D11DeviceContext* context = DX11Renderer::Instance().GetContext();
    if (!context) return;

    m_shader->Bind();

    D3D11_MAPPED_SUBRESOURCE mappedData = {};
    HRESULT hr = context->Map(m_cbuffer_object.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
    DXHelper::ThrowIfFailed(hr);

    CBuffer_Object_Data* dataPtr = static_cast<CBuffer_Object_Data*>(mappedData.pData);

    dataPtr->WorldTM = worldTM.Transpose();

    dataPtr->WorldInverseTransposeTM = worldInverseTransposeTM.Transpose();

    context->Unmap(m_cbuffer_object.Get(), 0);

    context->VSSetConstantBuffers(1, 1, m_cbuffer_object.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_cbuffer_object.GetAddressOf());
}