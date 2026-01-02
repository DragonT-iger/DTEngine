#pragma once
#include <cstdint>


enum class CullMode;

union RenderKey
{
    uint64_t value;
    struct {
        uint64_t depth : 30;
        uint64_t CullMode : 2; // back front, node  
        uint64_t shaderID : 16; 
        uint64_t TextureID : 16;
    } bits;
};
//LAYER를 구분해도 되긴 함.이미 UI는 LAYER가 있긴 한데, 일단 보류 


RenderKey CreateKey(float viewSpaceZ, uint16_t sID, uint16_t tID, CullMode cMode);
