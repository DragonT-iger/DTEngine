#pragma once
#include "SimpleMathHelper.h"

struct Vertex
{
    Vector3 Pos;     
    Vector4 Color;   
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 Bitangent;
    int BoneIDs[4];
    float Weights[4];
};