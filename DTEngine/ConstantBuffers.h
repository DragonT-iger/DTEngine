#pragma once

//기존에는 material의 상수 버퍼 settting을 통해 drawcall 증가

//Renderer가 상수 버퍼 포인터를 독점; material은 해당 포인터에 값을 쓰는 형식으로 변경 (기존 포인터 변경 및 쓰기 -> 현재 쓰기만)

//외부에서 상수버퍼를 생성; 즉 renderer와 그 형식을 공유해야 하는 경우의 상수버퍼 타입만 header로 공개.

// Light의 경우 Renderer에서 공개 안 해도 되니, 해당 헤더에 넣지 않겠음. 



__declspec(align(16))
struct CBuffer_Frame_Data //b0
{
    Matrix ViewTM {};
    Matrix ProjectionTM {};
};

__declspec(align(16))
struct CBuffer_Object_Data //b1
{
    Matrix WorldTM;
    Matrix WorldInverseTransposeTM;
};


//Use Texture 빼고, Metal 이랑 Roughness값 넣을 생각.....
__declspec(align(16))
struct MaterialData //b3
{
    Vector4 Color = { 1,1,1,1 };
    Vector4 UVTransform = { 1, 1, 0, 0 };

    int   UseTexture =1;
	float Shadow_Bias = 0.005f; //일단 넣어놓고 쓰지는 않음.  // 26/1/4 추가.
    float metallicFactor =1.0f;
    float roughnessFactor = 1.0f;
};

enum class MaterialTextureFlag : uint32_t {
    None = 0,
    Albedo = 1 << 0,  // 1
    Normal = 1 << 1,  // 2
    Specular = 1 << 2,  // 4
    Metallic = 1 << 3,  // 8
    Roughness = 1 << 4,  // 16
    AO = 1 << 5,  // 32
    IBL = 1 << 6,  // 64
    Gamma = 1 << 7   // 128
};

__declspec(align(16))
struct TextureFlag //b4
{
    uint32_t Use_Texture_flags = 0;      
    float pad[3];
};

__declspec(align(16))
struct Matrix_Pallette //b5
{
    Matrix Matrix_Pallette[128]; // 운용할 bone 갯수 만큼 배열 크기를 정의할 예정. 
};



