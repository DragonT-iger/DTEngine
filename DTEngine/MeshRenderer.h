#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"

class Mesh;     
class Material; 

class MeshRenderer : public MonoBehaviour
{
    DTGENERATED_BODY(MeshRenderer);

public:
    MeshRenderer() = default;

    void SetMesh(Mesh* mesh) { m_mesh = mesh; }
    Mesh* GetMesh() const { return m_mesh; }

    void SetMaterial(Material* material) { m_material = material; }
    Material* GetMaterial() const { return m_material; }

private:
    Mesh* m_mesh = nullptr;
    Material* m_material = nullptr;
};