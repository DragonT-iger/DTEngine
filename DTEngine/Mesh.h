#pragma once
#include <vector>
#include <string>
#include <wrl/client.h>
#include "IResource.h"
#include "Vertex.h"

struct ID3D11Buffer;
struct aiScene;
struct aiNode;
struct aiMesh;

class Mesh : public IResource
{
public:
    Mesh();
    virtual ~Mesh() = default;

    bool LoadFile(const std::string& fullPath) override { return false; }
    bool SaveFile(const std::string& fullPath) override { return false; }
	void Unload() override {};

    bool CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    void Bind() const;
    void Draw() const;

    UINT GetIndexCount() const { return m_indexCount; }

private:

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    UINT m_indexCount = 0;
    UINT m_stride = 0;
    UINT m_offset = 0;
};