#pragma once

//기존에는 material의 상수 버퍼 settting을 통해 drawcall 증가

//Renderer가 상수 버퍼 포인터를 독점; material은 해당 포인터에 값을 쓰는 형식으로 변경 (기존 포인터 변경 및 쓰기 -> 현재 쓰기만)

//외부에서 상수버퍼를 생성; 즉 renderer와 그 형식을 공유해야 하는 경우의 상수버퍼 타입만 header로 공개.

// Light의 경우 Renderer에서 공개 안 해도 되니, 해당 헤더에 넣지 않겠음. 

__declspec(align(16))
struct CBuffer_Object_Data
{
    Matrix WorldTM;
    Matrix WorldInverseTransposeTM;
};

__declspec(align(16))
struct CBuffer_Frame_Data
{
    Matrix ViewTM;
    Matrix ProjectionTM;
};

__declspec(align(16))
struct MaterialData
{
    Vector4 Color = { 1,1,1,1 };
    Vector4 UVTransform = { 1, 1, 0, 0 };
    int UseTexture;
    int Padding[3];
};



