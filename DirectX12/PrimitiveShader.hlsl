cbuffer mat : register(b0)
{
    float4x4 world;
    float4x4 viewproj;
    float4x4 ligth;
    float4 peye;
}

struct PrimOutput
{
    float4 svpos : SV_POSITION;
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
    float2 uv : TEXCODE;
};

PrimOutput PrimitiveVS(float4 pos : POSITION, float3 normal : NORMAL, float3 color :COLOR, float2 uv : TEXCOORD)
{
    PrimOutput o;
    o.svpos = mul(mul(viewproj, world), pos);
    //o.pos = mul(mul(viewproj, world), pos);

    return o;
}
 
//ボーン表示用ピクセルシェーダ 
float4 PrimitivePS(PrimOutput inp) : SV_Target
{
    return float4(1, 1, 1, 1);
}