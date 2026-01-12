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

// 매 프레임마다 ★
// Camera 기준 Z값이 작은 애들로 정렬 ; depth 가 MSB임. 
// 그래서 지금은 OPQUE 기준이고, Texture랑 Shader랑 저울질좀 해봐야 할듯? 정렬의 최우선 조건 
void Sorter::CreateKey(std::vector<GameObject*>& Objects)
{
    m_Sorting_Vec.clear();

    if (m_Sorting_Vec.capacity() < Objects.size()) m_Sorting_Vec.reserve(Objects.size());

    for (auto& Obj : Objects)
    {
        RenderKey rk{};

        Vector3 CamToObj_Vec = Obj->GetComponent<Transform>()->GetWorldPosition() - m_CamPara.Cam_Pos;
        float ViewZ = CamToObj_Vec.Dot(m_CamPara.Cam_For);
        float Normalized_Z = std::clamp(ViewZ / m_CamPara.Cam_z_Far, 0.f, 1.f);
        rk.bits.depth = static_cast<uint64_t>(Normalized_Z * ((1ULL << 30) - 1));

       // std::cout << "Z_depth" << Normalized_Z << "Name" << Obj->GetName() << std::endl;

         Material* mt = Obj->GetComponent<MeshRenderer>()->GetSharedMaterial();

        if (mt) //그러게 없으면 어쩐담. 
        {
            rk.bits.CullMode = static_cast<uint64_t>(mt->GetCullMode());
            rk.bits.shaderID = static_cast<uint64_t>(mt->GetShaderID());
            rk.bits.TextureID = static_cast<uint64_t>(mt->GetTextureID());
        }

        m_Sorting_Vec.push_back({ rk.value, Obj });
    }

    std::sort(m_Sorting_Vec.begin(), m_Sorting_Vec.end(),
        [](const SortingValue& a, const SortingValue& b) {
            return a.key < b.key;
            
        }
    );

  /*  std::cout << "AfterSorting" << std::endl;
    for (const auto& val : m_Sorting_Vec)
    {
        std::cout << "Name: " << val.obj->GetName() << std::endl;
    }*/

}