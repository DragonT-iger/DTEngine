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


    std::vector<Vertex> vertices(24);

    // 꼭짓점 8개 위치
    Vector3 p0(-0.5f, 0.5f, 0.5f); // 0 (앞-좌-위)
    Vector3 p1(0.5f, 0.5f, 0.5f); // 1 (앞-우-위)
    Vector3 p2(0.5f, -0.5f, 0.5f); // 2 (앞-우-아래)
    Vector3 p3(-0.5f, -0.5f, 0.5f); // 3 (앞-좌-아래)
    Vector3 p4(-0.5f, 0.5f, -0.5f); // 4 (뒤-좌-위)
    Vector3 p5(0.5f, 0.5f, -0.5f); // 5 (뒤-우-위)
    Vector3 p6(0.5f, -0.5f, -0.5f); // 6 (뒤-우-아래)
    Vector3 p7(-0.5f, -0.5f, -0.5f); // 7 (뒤-좌-아래)

    // UV 좌표 4개
    Vector2 uv0(0.0f, 0.0f);
    Vector2 uv1(1.0f, 0.0f);
    Vector2 uv2(1.0f, 1.0f);
    Vector2 uv3(0.0f, 1.0f);

    // 법선 6개
    Vector3 nFront(0.0f, 0.0f, 1.0f);  // +Z
    Vector3 nBack(0.0f, 0.0f, -1.0f); // -Z
    Vector3 nTop(0.0f, 1.0f, 0.0f);   // +Y
    Vector3 nBottom(0.0f, -1.0f, 0.0f);// -Y
    Vector3 nLeft(-1.0f, 0.0f, 0.0f); // -X
    Vector3 nRight(1.0f, 0.0f, 0.0f);  // +X

    // 색상 (단순화)
    Vector4 c(1.0f, 1.0f, 1.0f, 1.0f); // 흰색

    // 1. 앞면 (+Z) (0, 1, 2, 3)
    vertices[0] = { p0, c, uv0, nFront };
    vertices[1] = { p1, c, uv1, nFront };
    vertices[2] = { p2, c, uv2, nFront };
    vertices[3] = { p3, c, uv3, nFront };

    // 2. 뒷면 (-Z) (4, 5, 6, 7) - UV 순서 주의
    vertices[4] = { p5, c, uv0, nBack }; // 5
    vertices[5] = { p4, c, uv1, nBack }; // 4
    vertices[6] = { p7, c, uv2, nBack }; // 7
    vertices[7] = { p6, c, uv3, nBack }; // 6

    // 3. 윗면 (+Y) (4, 5, 1, 0)
    vertices[8] = { p4, c, uv0, nTop };
    vertices[9] = { p5, c, uv1, nTop };
    vertices[10] = { p1, c, uv2, nTop };
    vertices[11] = { p0, c, uv3, nTop };

    // 4. 아랫면 (-Y) (3, 2, 6, 7)
    vertices[12] = { p3, c, uv0, nBottom };
    vertices[13] = { p2, c, uv1, nBottom };
    vertices[14] = { p6, c, uv2, nBottom };
    vertices[15] = { p7, c, uv3, nBottom };

    // 5. 왼쪽 면 (-X) (4, 0, 3, 7)
    vertices[16] = { p4, c, uv0, nLeft };
    vertices[17] = { p0, c, uv1, nLeft };
    vertices[18] = { p3, c, uv2, nLeft };
    vertices[19] = { p7, c, uv3, nLeft };

    // 6. 오른쪽 면 (+X) (1, 5, 6, 2)
    vertices[20] = { p1, c, uv0, nRight };
    vertices[21] = { p5, c, uv1, nRight };
    vertices[22] = { p6, c, uv2, nRight };
    vertices[23] = { p2, c, uv3, nRight };

    // 인덱스 데이터 (6면 * 2삼각형 * 3점 = 36개)
    std::vector<uint32_t> indices = {
         0,  1,  2,    0,  2,  3, // Front
         4,  5,  6,    4,  6,  7, // Back
         8,  9, 10,    8, 10, 11, // Top
        12, 13, 14,   12, 14, 15, // Bottom
        16, 17, 18,   16, 18, 19, // Left
        20, 21, 22,   20, 22, 23  // Right
    };

    // 2. 구현해두신 버퍼 생성 함수 호출
    return CreateBuffers(vertices, indices);

    // 테스트용


    return false;
}

bool Mesh::SaveFile(const std::string& fullPath)
{
    return false;
}

void Mesh::Unload()
{
    return;
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