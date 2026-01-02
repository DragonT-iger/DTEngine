#pragma once
#include "pch.h"
#include "Material.h"
#include "RenderKey.h"

//가져올 수만 있다면...
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"




RenderKey CreateKey(Vector3 WorldPos ,  uint16_t sID, uint16_t tID, CullMode cMode)
{
    
    Camera* mainCam = SceneManager::Instance().GetActiveScene()->GetMainCamera();
    Vector3 CamPos = mainCam->

    float z_Far = mainCam->GetFarZ();

        RenderKey key;
        key.value = 0; // 초기화

        //Texture & Shader ID 는 생성시, 할당.
        key.bits.shaderID = sID;
        key.bits.TextureID = tID;

        key.bits.CullMode = (uint64_t)cMode; 







        float normalizedDepth = std::clamp(viewSpaceZ / z_Far, 0.0f, 1.0f); // FarPlane=10000 가정

        //30 bit 최댓값으로 
        uint64_t maxDepthInt = (1ULL << 30) - 1;
        uint64_t depthInt = (uint64_t)(normalizedDepth * maxDepthInt);


        // 불투명: 가까운 것(작은 Z)이 먼저 그려져야 함 -> Front-to-Back
        // 작은 Z가 작은 숫자가 되도록 그대로 사용.
        key.bits.depth = depthInt;


        return key;
    
}
