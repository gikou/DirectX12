
cbuffer mat : register(b0)
{
    float4x4 world;
    float4x4 viewproj;
    float4x4 ligth;
    float4 peye;
}

cbuffer bones : register(b1)
{
    matrix boneMats[512];
}

struct Out
{
    float4 svpos : SV_POSITION;
    float4 pos : POSITION0;
    float2 uv : TEXCOORD;
    float4 normal : NORMAL;
    float3 weight : WEIGHT;
};

Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, min16uint2 boneno : BONENO, min16uint weight : WEIGHT)
{
    Out o;

    float w = weight / 100.0f;
    matrix m = boneMats[boneno.x] * w + boneMats[boneno.y] * (1 - w);
    pos = mul(m, pos);

	
    o.svpos = mul(mul(viewproj, world), pos);
    pos = mul(mul(viewproj, world), pos);
	//o.svpos = pos;
    o.pos = pos;
    o.uv = uv;
    o.normal = mul(world, normal);
    return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
    return float4(1, 1, 1, 1);

}
