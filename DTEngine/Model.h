#pragma once
#include "IResource.h"
#include <vector>

class Mesh;
struct aiNode;
struct aiScene;
struct aiMesh;

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

private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Mesh*> m_meshes;
};