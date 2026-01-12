#pragma once
#include "Singleton.h"
#include <cstdint>
#include <deque>

class GameObject;
class Camera;

union RenderKey
{
    uint64_t value;
    struct {
        
        uint64_t CullMode : 2; // back front, node  
        uint64_t shaderID : 16;
        uint64_t TextureID : 16;
        uint64_t depth : 30;
       
    } bits;
};

// MSVC 는 선언 시 little_endian 부터 채워짐 
//[ depth(30) ] [ TextureID(16) ][ shaderID(16) ][ CullMode(2) ] 
//MSB                                                      LSB ; Sorting은 MSB 기준으로 

//Texture & Shader 가 우선으로 정렬시킴. 

struct CamParameters
{
    Vector3 Cam_For = { 0,0,0 };
    Vector3 Cam_Pos = { 0,0,0 };
    float Cam_z_Far = 0;
};

struct SortingValue
{
    uint64_t key;
    GameObject* obj;
};


class Sorter : public Singleton<Sorter>
{
    friend class Singleton<Sorter>;
public:

    void SetCamParameters(Camera* Cam); // CamPos ; Forward ; for Depth z 
    void CreateKey(std::vector<GameObject*>& Objects);
    const std::vector<SortingValue>& GetRenderVec() const { return m_Sorting_Vec; }

private:
    CamParameters m_CamPara;
    std::vector<SortingValue> m_Sorting_Vec;
};


