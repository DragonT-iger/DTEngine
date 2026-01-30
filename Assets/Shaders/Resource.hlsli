
cbuffer CBuffer_Frame : register(b1)
{
    float4x4 View_TM;
    float4x4 Projection_TM;
};

cbuffer CBuffer_Object : register(b2)
{
    float4x4 World_TM;
    float4x4 WorldInverseTranspose_TM;
};


//b3 Light

cbuffer CBuffer_Material : register(b4)
{
    float4 MaterialColor;
    float4 UVTransform;
    
    int UseTexture;
    float Shadow_Bias; //일단 넣어 놓음. + 객체 단위의 정교한 Shadow_Bias 가 필요한 경우가 있을 경우. 
    
    float Shadow_Scale;
    float Roughness_Factor =1.0f ;
};


cbuffer CBuffer_BooleanFlag : register(b5)
{
    uint g_Flags;
    float pad[3]; //일단 비워 놓을게요  
};

cbuffer CBuffer_Matrix_Pallette : register(b6)
{
    float4x4 Matrix_Pallette_Array[128]; // 운용 할 bone 갯수 만큼 배열 크기를 정의할 예정. 
};


cbuffer CBuffer_SkyBox : register(b7)
{
    float4 SkyBox_Color; 
};

cbuffer CBuffer_Effect : register(b8)
{
    float progress; //진행도 
    float edgeWidth; 
    float glowIntensity; //빛의 강도 
    float totalTime; //
    
    float4 edgeColor;
    
    float noiseScalef;
    float timeMultiplier; // 애니메이션 배속 (추가)
    float effectType;
    float Skinned_Flag; // bone / rigid  object와의 통합 위해.
}



struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    
    int4 BoneID : BLENDINDICES;
    float4 Weights : BLENDWEIGHT;
};


// 전역적으로 Cubemap을 관리하지 않기 때문에, t0(cubemap)은 해당 Shader에서 정의
// 일반적인 경우는 해당 Text

//기본적인 Texture Index

Texture2D g_DiffuseMap : register(t0);
Texture2D g_NormalMap  : register(t1);

Texture2D g_SpecMap    : register(t2); //Editor에서도 이걸 기준으로 PBR - NONE PBR의 구분을 쉽게 할 수 있을 거 같음. 

Texture2D g_MetalMap   : register(t3);
Texture2D g_RoughMap   : register(t4);
Texture2D g_AoMap      : register(t5);

Texture2D g_NoiseMap : register(t6);
Texture2D g_SphereMap : register(t7);

TextureCube g_CubeMap : register(t9);
//Texture2D Empty : register(t6);`
//Texture2D Empty : register(t7);

//-------------------------------------------- 일단 Editor에서 작업할 수 있는 Texture들 , Object 단위의 Texture slot이라고 생각하면 됨. 
// P.S Masking or NoiseMap 같은 것들을 생각하고 있음 
// UI의 경우도 해당 공용 헤더를 상속받는 것에 대해서 문제가 없다고 판단하고 있음.  


// 여기 밑 [8] 부터는 사용자가 제어하는 것이 아닌 Renderer에서 혹은 게임 공용적인 자원으로 환경광이나, Global Texture Data; Shadowmap 같은 것들의 정보를 넣을 것.


//얘도 어찌보면 index 하나 더 올릴거임. 

// 01_10 지금 기준으로는 Light에 종속되어있지만 나중에 shadow를 Light 공용 헤더에서 빼서 여기에 넣을 생각임. 
// IBL도 따로 만들까 싶음. 
//Texture2D g_ShadowMap : register(t10);
//SamplerComparisonState g_ShadowSampler : register(s10);


//#pragma region Flag_Macros

#define HAS_ALBEDO   (1 << 0)
#define HAS_NORMAL   (1 << 1)
#define HAS_SPEC     (1 << 2)

#define HAS_METAL    (1 << 3)
#define HAS_ROUGH    (1 << 4)
#define HAS_AO       (1 << 5)

#define HAS_IBL      (1 << 6)
//#define ON_GAMMA     (1 << 7) 

#define USE_ALBEDO  (g_Flags & HAS_ALBEDO)
#define USE_NORMAL  (g_Flags & HAS_NORMAL)
#define USE_SPEC    (g_Flags & HAS_SPEC)
#define USE_METAL   (g_Flags & HAS_METAL)
#define USE_ROUGH   (g_Flags & HAS_ROUGH)
#define USE_AO      (g_Flags & HAS_AO)
#define USE_IBL     (g_Flags & HAS_IBL)
//#define NEED_ON_GAMMA    (g_Flags & ON_GAMMA)

//#pragma endregion
