#include "pch.h"
#include "Mesh.h"
#include "DX11Renderer.h"
#include "DXHelper.h"
#include <cassert>

Mesh::Mesh()
    : m_indexCount(0), m_stride(sizeof(Vertex)), m_offset(0)
{
}

bool Mesh::LoadFile(const std::string& fullPath)
{
    // TODO: .obj, .fbx 같은 파일 로더 구현
    // 지금은 CreateBuffers를 수동으로 호출해서 테스트합니다.
    return false;
}

bool Mesh::SaveFile(const std::string& fullPath)
{
    return false;
}

bool Mesh::CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    assert(vertices.size() > 0 && indices.size() > 0);

    m_indexCount = static_cast<UINT>(indices.size());
    m_stride = sizeof(Vertex);
    m_offset = 0;

    auto* device = DX11Renderer::Instance().GetDevice();
    if (!device) return false;

    HRESULT hr;

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vbDesc.Usage = D3D11_USAGE_DEFAULT;  
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();

    hr = device->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf());
    DXHelper::ThrowIfFailed(hr); 

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(uint32_t) * static_cast<UINT>(indices.size());
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibDesc, &ibData, m_indexBuffer.GetAddressOf());
    DXHelper::ThrowIfFailed(hr);

    return true;
}

void Mesh::Bind() const
{
    auto* context = DX11Renderer::Instance().GetContext();
    if (!context || !m_vertexBuffer || !m_indexBuffer) return;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Draw() const
{
    auto* context = DX11Renderer::Instance().GetContext();
    if (!context) return;

    context->DrawIndexed(m_indexCount, 0, 0);
}