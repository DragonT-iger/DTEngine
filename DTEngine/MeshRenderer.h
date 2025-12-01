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

    void SetModelPath(const std::string& path);
    std::string GetModelPath() const { return m_modelPath; }

    void SetMeshIndex(int index);
    int GetMeshIndex() const { return m_meshIndex; }

    void SetMesh(Mesh* mesh) { m_mesh = mesh; } // 런타임 수동 할당용
    Mesh* GetMesh() const { return m_mesh; }

    void SetMaterial(Material* material) { m_material = material; }
    Material* GetMaterial() const { return m_material; }

    void SetMaterialPath(const std::string& path);
    std::string GetMaterialPath() const { return m_materialPath; }

private:
    Mesh* m_mesh = nullptr;
    Material* m_material = nullptr;

    std::string m_modelPath = "";
    std::string m_materialPath = "";
    int m_meshIndex = 0;
};