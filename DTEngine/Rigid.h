#pragma once
#include "MonoBehaviour.h"
#include "SimpleMathHelper.h"
#include <vector>

struct NodeResource;

class Rigid : public MonoBehaviour
{
    DTGENERATED_BODY(Rigid);

public:
    Rigid() = default;
    virtual ~Rigid() = default;

public:

    void LateUpdate(float dTime) override;
    uint64_t GetModelID() const { return m_modelID; }
    void SetModel(uint64_t id);
    void SetNodePose(int nodeIndex, const Matrix& mat);

    std::vector<Matrix>& GetFinalTransforms()  { return m_finalTransforms; }
    NodeResource* GetNodeResource() { return m_resource; }

private:
    uint64_t m_modelID = 0;
    NodeResource* m_resource = nullptr;
    std::vector<Matrix> m_baseLocalInv;
    bool m_baseInitialized = false;

    std::vector<Matrix> m_finalTransforms;       // GPU 전달용 (Global)
    std::vector<Matrix> m_globalTransforms;      // 계층 계산용
    std::vector<Matrix> m_animatedLocalMatrices; // Animator 주입용
};