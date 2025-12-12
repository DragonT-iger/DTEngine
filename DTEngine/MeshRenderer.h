#pragma once
#include "MonoBehaviour.h"
#include "ReflectionMacros.h"
#include <string>
#include "JsonIO.h"

class Mesh;
class Material;
class Model;

class MeshRenderer : public MonoBehaviour
{
    DTGENERATED_BODY(MeshRenderer);

public:
    MeshRenderer();
    virtual ~MeshRenderer();

    void Awake() override;

    void SetModelID(uint64_t id);
    uint64_t GetModelID() const { return m_modelID; }

    void SetMeshIndex(int index);
    int GetMeshIndex() const { return m_meshIndex; }

    void SetMesh(Mesh* mesh) { m_mesh = mesh; }
    Mesh* GetMesh() const { return m_mesh; }

    // 이걸로 머터리얼 인스턴스 상태 초기화 가능함
    void SetMaterialID(uint64_t id);
    uint64_t GetMaterialID() const { return m_materialID; }

	// 이 함수는 주의해서 사용하셈 이게 호출되면 머터리얼 인스턴스화 시킴
    Material* GetMaterial();
    void SetMaterial(Material* material);


    Material* GetSharedMaterial() const { return m_material; }

    bool IsMaterialInstanced() const { return m_isMaterialInstanced; }

    void SaveInstanceData(JsonWriter& writer);
    void LoadInstanceData(JsonReader& reader);

private:
    Mesh* m_mesh = nullptr;
    Material* m_material = nullptr;

    uint64_t m_modelID = 0;
    uint64_t m_materialID = 0;
    int m_meshIndex = 0;

    bool m_isMaterialInstanced = false;
};