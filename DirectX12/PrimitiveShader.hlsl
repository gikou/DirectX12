SamplerState smp : register(s0);
Texture2D<float> tex : register(t0); //通常テクスチャ 

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
    float4 pos : POSITION0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCODE;
    
    float4 shadowpos : POSITION1;
};

PrimOutput PrimitiveVS(float4 pos : POSITION, float4 normal : NORMAL, float4 color : COLOR, float2 uv : TEXCOORD)
{
    PrimOutput o;

    o.svpos = mul(mul(viewproj, world), pos);
    //o.pos = mul(mul(viewproj, world), pos);
    o.normal = mul(world, normal);
    o.color = color;
    //o.uv = uv;
    o.shadowpos = mul(mul(ligth, world), pos);
    o.uv = o.shadowpos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);


    return o;
}
 
//ボーン表示用ピクセルシェーダ 
float4 PrimitivePS(PrimOutput inp) : SV_Target
{
    //return float4(inp.uv, 1.f, 1.f);
    //return float4(inp.color.rgb, 1.0f);
    float t = tex.Sample(smp, inp.uv);
    t = pow(t, 5000.f);
    float3 texcol = float3(t, t, t);
    texcol *= inp.color;
    float brightness = 1;
    if (inp.shadowpos.z > t)
    {
        brightness *= 0.7f; //暗くする 
    }
    //return float4(brightness * inp.color.rgb, 1);

    return float4(texcol.rgb, 1);
}