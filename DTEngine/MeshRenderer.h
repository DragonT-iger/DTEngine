#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include <string>

class Mesh;
class Material;
class Model;

class MeshRenderer : public MonoBehaviour
{
    DTGENERATED_BODY(MeshRenderer);

public:
    MeshRenderer() = default;

    void Awake() override;

    void SetModelID(uint64_t id);
    uint64_t GetModelID() const { return m_modelID; }

    void SetMeshIndex(int index);
    int GetMeshIndex() const { return m_meshIndex; }

    void SetMesh(Mesh* mesh) { m_mesh = mesh; }
    Mesh* GetMesh() const { return m_mesh; }

    void SetMaterial(Material* material) { m_material = material; }
    Material* GetMaterial() const { return m_material; }

    void SetMaterialID(uint64_t id);
    uint64_t GetMaterialID() const { return m_materialID; }

private:
    Mesh* m_mesh = nullptr;
    Material* m_material = nullptr;

    uint64_t m_modelID = 0;
    uint64_t m_materialID = 0;
    int m_meshIndex = 0;
};