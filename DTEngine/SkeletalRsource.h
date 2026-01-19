#pragma once

#include <map>
#include <string>
#include <vector>
#include "SimpleMathHelper.h"


struct BoneNode
{
    Matrix OffsetMatrix{}; //모델 -> 뼈
    Matrix DefaultLocalMatrix{};  // 뼈 -> 부모

    Matrix GlobalMatrix{}; // 누적 행렬 

    int ParentIndex = -1;
    std::string name;
    int id; //Matrix Pallette 의 index 
};

struct BoneResource
{
    std::vector<BoneNode> m_Bones;
    std::map<std::string, int> m_BoneMapping; // 이름으로 찾을 수도

    int GetIndex(const std::string& name) { if (m_BoneMapping.find(name) != m_BoneMapping.end()) return m_BoneMapping[name];  else return -1; }

};
