
cbuffer CBuffer_Frame : register(b0)
{
    matrix View_TM;
    matrix Projection_TM;
};

cbuffer CBuffer_Object : register(b1)
{
    matrix World_TM;
    matrix WorldInverseTranspose_TM;
};

//Use Texture 대신 Metal + Roughness값 넣기 
cbuffer CBuffer_Material : register(b3)
{
    float4 MaterialColor;
    float4 UVTransform;
    int UseTexture;
    float2 Padding2;
};


struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
};


// 전역적으로 Cubemap을 관리하지 않기 때문에, t0(cubemap)은 해당 Shader에서 정의
// 일반적인 경우는 해당 Text

//기본적인 Texture Index

Texture2D g_DiffuseMap : register(t0);
Texture2D g_SpecMap : register(t1);
Texture2D g_NormalMap : register(t3);

//Specular도 뒤로 미루고 -> PBR 쓰니깐 얘는 안쓰지 않을까 싶어서 

//0  Diffuse / Albedo
//1  Normal / Normal 
//-------------------- PBR NON PBR 이든 여기는 쓸 거 같음 
//2 Metal
//3 AO
//4 ROUGHNESS 

//5번 부터 7번 ; Editor에서 제공하는 Index까지는 객체가 관리하는 거로; masking 같은 것들 포함 
Texture2D g_SphereMap : register(t7);

//그 이후는 이제 외부에서 Shadow Map처럼 관리하자
//그래

//선배들은 이렇게 하긴 함 

//Texture2D txDiffuse : register(t0);
//Texture2D txNormal : register(t1);
//Texture2D txSpecular : register(t2);
//Texture2D txEmissive : register(t3);
//Texture2D txOpacity : register(t4);
//Texture2D txMetalness : register(t5);
//Texture2D txRoughness : register(t6);

//TextureCube txEnvironment : register(t7);
//TextureCube txIBL_Diffuse : register(t8);
//TextureCube txIBL_Specular : register(t9);
//Texture2D txIBL_SpecularBRDF_LUT : register(t10);
//Texture2D txShadowMap : register(t11);

//TextureCube txLightIBL_Diffuse : register(t12);
//TextureCube txLightIBL_Specular : register(t13);
//Texture2D txLightIBL_SpecularBRDF_LUT : register(t14);
