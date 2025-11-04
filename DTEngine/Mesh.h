#pragma once
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>
#include "IResource.h"
#include "Vertex.h"

class Mesh : public IResource
{
public:
    Mesh();
    virtual ~Mesh() = default;

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override;
    void Unload() override;

    bool CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    void Bind() const;

    void Draw() const;

    UINT GetIndexCount() const { return m_indexCount; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    UINT m_indexCount = 0;
    UINT m_stride = 0;
    UINT m_offset = 0;
};