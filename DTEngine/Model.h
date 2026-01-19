#pragma once
#include "IResource.h"
#include <vector>

class Mesh;
struct Vertex;
struct aiNode;
struct aiScene;
struct aiMesh;

struct BoneResource;

class Model : public IResource
{
public:
    Model();
    virtual ~Model();

    bool LoadFile(const std::string& fullPath) override;
    bool SaveFile(const std::string& fullPath) override { return false; }
    void Unload() override;

    size_t GetMeshCount() const { return m_meshes.size(); }
    Mesh* GetMesh(size_t index) const;
    BoneResource* GetBone() { return m_impl.get(); }
private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessBonesMap(const aiScene* scene);
    void CreateSkeleton(const aiNode* node, int parentIndex);


    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    std::vector<std::unique_ptr<Mesh>> m_meshes;

    std::unique_ptr<BoneResource> m_impl;
    int m_BoneCounter = 0;
};