struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VS_OUTPUT VS(uint id : SV_VertexID)
{
    VS_OUTPUT output;
    
    float2 tex = float2((id << 1) & 2, id & 2);
    output.UV = tex;
    output.Pos = float4(tex * float2(2, -2) + float2(-1, 1), 0, 1);
    
    return output;
}



// 포스트 프로세싱의 _VS PostProcess_VS 를 제외하고 굳이 있을 필요는 없지만
// 엔진의 구조상 있는게 나아서 그냥 냅뒀음 무시해도 되는 코드