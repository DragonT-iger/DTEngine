#pragma once
#include <SimpleMath.h>

using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector4 = DirectX::SimpleMath::Vector4;

struct Vertex
{
    Vector3 Pos;     
    Vector4 Color;   
    Vector2 Texcoord;
    Vector3 Normal;  
};