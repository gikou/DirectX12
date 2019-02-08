Texture2D<float4> tex : register(t0); //通常テクスチャ 
SamplerState smp : register(s0);

cbuffer Weight : register(b0)
{
    float4 wgts[2];
}; 


struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
    float2 uv2 : TEXCOORD2;
    float2 uv3 : TEXCOORD3;
    float2 uv4 : TEXCOORD4;
    float2 uv5 : TEXCOORD5;
    float2 uv6 : TEXCOORD6;
    float2 uv7 : TEXCOORD7;
    float2 size : SIZE;
};
//頂点シェーダ 
Output EffectVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Output output;
    float w, h, level;
    tex.GetDimensions(0, w, h, level);

    output.svpos = pos;
    output.uv = uv;
    output.uv1 = uv + float2(0, -1 / h);
    output.uv2 = uv + float2(0, -3 / h);
    output.uv3 = uv + float2(0, -5 / h);
    output.uv4 = uv + float2(0, -7 / h);
    output.uv5 = uv + float2(0, -9 / h);
    output.uv6 = uv + float2(0, -11 / h);
    output.uv7 = uv + float2(0, -13 / h);
    output.size = float2(w, h);

    return output;
}
//ピクセルシェーダ 
float4 EffectPS(Output input) : SV_Target
{
  
    float2 offsety = float2(0, 14 / input.size.y);
    float4 ret = tex.Sample(smp, input.uv);
    ret = ret * wgts[0].x;
    ret += wgts[0].y * (tex.Sample(smp, input.uv1) + tex.Sample(smp, input.uv7 + offsety));
    ret += wgts[0].z * (tex.Sample(smp, input.uv2) + tex.Sample(smp, input.uv6 + offsety));
    ret += wgts[0].w * (tex.Sample(smp, input.uv3) + tex.Sample(smp, input.uv5 + offsety));
    ret += wgts[1].x * (tex.Sample(smp, input.uv4) + tex.Sample(smp, input.uv4 + offsety));
    ret += wgts[1].y * (tex.Sample(smp, input.uv5) + tex.Sample(smp, input.uv3 + offsety));
    ret += wgts[1].z * (tex.Sample(smp, input.uv6) + tex.Sample(smp, input.uv2 + offsety));
    ret += wgts[1].w * (tex.Sample(smp, input.uv7) + tex.Sample(smp, input.uv1 + offsety));

    //return ret;

    return tex.Sample(smp, input.uv);

}