#include "pch.h"

#include <cassert>
#include <d3d11.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "DX11Renderer.h"
#include "DXHelper.h"
#include "ResourceManager.h"

Mesh::Mesh()
    : m_indexCount(0), m_stride(sizeof(Vertex)), m_offset(0)
{
}

bool Mesh::LoadFile(const std::string& fullPath)
{
    Assimp::Importer importer;

    const UINT importFlags =
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices;

    const aiScene* scene = importer.ReadFile(fullPath, importFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << (importer.GetErrorString()) << std::endl;
        return false;
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    ProcessNode(scene->mRootNode, scene, vertices, indices);

    return CreateBuffers(vertices, indices);
}

bool Mesh::SaveFile(const std::string& fullPath)
{
    return false;
}

void Mesh::Unload()
{
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_indexCount = 0;
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

void Mesh::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, vertices, indices);
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, vertices, indices);
    }
}

void Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    UINT vertexOffset = static_cast<UINT>(vertices.size());

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v;

        if (mesh->HasPositions())
        {
            v.Pos.x = mesh->mVertices[i].x;
            v.Pos.y = mesh->mVertices[i].y;
            v.Pos.z = mesh->mVertices[i].z;
        }

        if (mesh->HasNormals())
        {
            v.Normal.x = mesh->mNormals[i].x;
            v.Normal.y = mesh->mNormals[i].y;
            v.Normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasTextureCoords(0))
        {
            v.Texcoord.x = mesh->mTextureCoords[0][i].x;
            v.Texcoord.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            v.Texcoord = Vector2(0.0f, 0.0f);
        }

        if (mesh->HasVertexColors(0))
        {
            v.Color.x = mesh->mColors[0][i].r;
            v.Color.y = mesh->mColors[0][i].g;
            v.Color.z = mesh->mColors[0][i].b;
            v.Color.w = mesh->mColors[0][i].a;
        }
        else
        {
            v.Color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        vertices.push_back(v);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j] + vertexOffset);
        }
    }
}
