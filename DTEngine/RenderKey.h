#pragma once
#include "Singleton.h"
#include <cstdint>
#include <deque>
#include <vector>

class GameObject;
class Camera;

struct CamParameters
{
    Vector3 Cam_For = { 0,0,0 };
    Vector3 Cam_Pos = { 0,0,0 };
    float Cam_z_Far = 0;
};

struct SortingValue
{
    uint32_t key;
    GameObject* obj;
};


class Sorter : public Singleton<Sorter>
{
    friend class Singleton<Sorter>;
public:

    void SetCamParameters(Camera* Cam); // CamPos ; Forward ; for Depth z 
    void CreateKeyOpaque(std::vector<GameObject*>& Objects);
    void CreateKeyTransparent(std::vector<GameObject*>& Objects);

     std::vector<GameObject*>& GetOpaqueVec() { return m_returnOpaqueVec; }
     std::vector<GameObject*>& GetTransVec()  { return m_returnTransVec; }
private:
    CamParameters m_CamPara;
    std::vector<SortingValue> m_sortingVec;
    std::vector<GameObject*>  m_returnOpaqueVec;
    std::vector<GameObject*> m_returnTransVec;
};


