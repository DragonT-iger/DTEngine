#pragma once
#include <vector>
#include <string>
#include <wrl/client.h>
#include "Vertex.h"

struct ID3D11Buffer;
struct aiScene;
struct aiNode;
struct aiMesh;

class Mesh
{
public:
    Mesh();
    virtual ~Mesh() = default;

    bool CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    void Bind() const;
    void Draw() const;

    UINT GetIndexCount() const { return m_indexCount; }

    const DirectX::BoundingBox& GetLocalBoundingBox() const { return m_localBounds; }

    const std::vector<Vector3>& GetPositions() const { return m_positions; }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }

private:
    void BuildLocalBounds();

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    std::vector<Vector3> m_positions;
    std::vector<uint32_t> m_indices;

    UINT m_indexCount = 0;
    UINT m_stride = 0;
    UINT m_offset = 0;

    DirectX::BoundingBox m_localBounds;
};