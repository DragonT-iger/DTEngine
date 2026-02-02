#pragma once
#include "pch.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "RenderKey.h"


#include "Camera.h"



void Sorter::SetCamParameters(Camera* Cam) // RenderPass 초입에
{
    m_CamPara.Cam_Pos = Cam->GetCamPos();
    m_CamPara .Cam_For = Cam->GetCamFor();
    m_CamPara.Cam_z_Far = Cam->GetFarZ();
}


void Sorter::CreateKeyOpaque(std::vector<GameObject*>& Objects)
{
    m_sortingVec.clear();
    m_returnOpaqueVec.clear();
    for (auto& Obj : Objects)
    {
        Vector3 CamToObj = Obj->GetTransform()->GetWorldPosition() - m_CamPara.Cam_Pos;
        float ViewZ = CamToObj.Dot(m_CamPara.Cam_For);
        float Normalized_Z = std::clamp(ViewZ / m_CamPara.Cam_z_Far, 0.f, 1.f);

        uint32_t key = static_cast<uint32_t>(Normalized_Z * 0xFFFFFFFF);
        m_sortingVec.push_back({ key, Obj });
    }

    std::sort(m_sortingVec.begin(), m_sortingVec.end(),
        [](const SortingValue& a, const SortingValue& b) { return a.key < b.key; });


    m_returnOpaqueVec.reserve(m_sortingVec.size());
    for (auto& val : m_sortingVec)
    {
        m_returnOpaqueVec.push_back(val.obj);
    }


}

void Sorter::CreateKeyTransparent(std::vector<GameObject*>& Objects)
{
    m_sortingVec.clear();
    m_returnTransVec.clear();
    for (auto& Obj : Objects)
    {
        Vector3 CamToObj = Obj->GetTransform()->GetWorldPosition() - m_CamPara.Cam_Pos;
        float ViewZ = CamToObj.Dot(m_CamPara.Cam_For);
        float Normalized_Z = std::clamp(ViewZ / m_CamPara.Cam_z_Far, 0.f, 1.f);

        uint32_t key = static_cast<uint32_t>((1.0f - Normalized_Z) * 0xFFFFFFFF);
        m_sortingVec.push_back({ key, Obj });
    }

    std::sort(m_sortingVec.begin(), m_sortingVec.end(),
        [](const SortingValue& a, const SortingValue& b) { return a.key < b.key; });

    m_returnTransVec.reserve(m_sortingVec.size());
    for (auto& val : m_sortingVec)
    {
        m_returnTransVec.push_back(val.obj);
    }

}