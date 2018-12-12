Texture2D<float4> tex : register(t0); //通常テクスチャ 
Texture2D<float4> normal : register(t1); //法線テクスチャ 
Texture2D<float> depth : register(t2); //法線テクスチャ 
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
Output EffectVS( float4 pos : POSITION, float2 uv : TEXCOORD) {
    Output output;
    float w, h, level;
    tex.GetDimensions(0, w, h, level);

    output.svpos = pos; 
    output.uv  = uv;
    output.uv1 = uv + float2(-1 / w, 0);
    output.uv2 = uv + float2(-3 / w, 0);
    output.uv3 = uv + float2(-5 / w, 0);
    output.uv4 = uv + float2(-7 / w, 0);
    output.uv5 = uv + float2(-9 / w, 0);
    output.uv6 = uv + float2(-11 / w, 0);
    output.uv7 = uv + float2(-13 / w, 0);
    output.size = float2(w, h);

    return output;
} 
//ピクセルシェーダ 
float4 EffectPS(Output input) : SV_Target
{
    float d = pow(depth.Sample(smp, input.uv/* * float2(4, 4)*/), 200);
    pow(d, 1000.0f);
    //d -= 0.8f;
    //return float4(d, d, d, 1.0f);
    //float3 texC = tex.Sample(smp, input.uv).rgb;
    //float v = dot(float3(0.2126, 0.7152, 0.0722), texC);//モノクロ
    //return float4(tex.Sample(smp, input.uv).rgb - fmod(tex.Sample(smp, input.uv).rgb, 0.25f), 1);//ポスタリゼーション
    //return float4(v,v,v, 1.0f);//グレースケール

    /*ぼかし周囲9pixelの平均値の色を出している*/
    //float w, h, level;
    //float ave = 9.0f
    //tex.GetDimensions(0, w, h, level);
    //float dx = 1.0f / w;
    //float dy = 1.0f / h;
    //float4 ret = float4(0, 0, 0, 0);
  
 
    //ret += tex.Sample(smp, input.uv + float2(-2 * dx, 2 * dy)) / ave;
    //ret += tex.Sample(smp, input.uv + float2(2 * dx, 2 * dy)) / ave;
    //ret += tex.Sample(smp, input.uv + float2(0, 2 * dy)) / ave;
 
    //ret += tex.Sample(smp, input.uv + float2(-2 * dx, 0)) / ave;
    //ret += tex.Sample(smp, input.uv) / ave;
    //ret += tex.Sample(smp, input.uv + float2(2 * dx, 0)) / ave;
 
    //ret += tex.Sample(smp, input.uv + float2(-2 * dx, -2 * dy)) / ave;
    //ret += tex.Sample(smp, input.uv + float2(2 * dx, -2 * dy)) / ave;
    //ret += tex.Sample(smp, input.uv + float2(0, -2 * dy)) / ave;
    //return ret;

    /*輪郭抽出*/
    //float w, h, level;
    //tex.GetDimensions(0, w, h, level);
    //float dx = 1.0f / w;
    //float dy = 1.0f / h;
    //float4 ret = tex.Sample(smp, input.uv);
    //ret = ret * 4 - tex.Sample(smp, input.uv + float2(-dx, 0)) -
    //tex.Sample(smp, input.uv + float2(dx, 0)) -
    //tex.Sample(smp, input.uv + float2(0, dy)) -
    //tex.Sample(smp, input.uv + float2(0, -dy));
    //float b = dot(float3(0.298912f, 0.586611f, 0.114478f), 1 - ret.rgb);
    //b = pow(b, 4);
    //return float4(b, b, b, 1);
    
    //float2 offsetx = float2(14 / input.size.x, 0);
    //float4 ret = tex.Sample(smp, input.uv);
    //ret = ret * wgts[0].x;
    //ret += wgts[0].y * (tex.Sample(smp, input.uv1) + tex.Sample(smp, input.uv7 + offsetx));
    //ret += wgts[0].z * (tex.Sample(smp, input.uv2) + tex.Sample(smp, input.uv6 + offsetx));
    //ret += wgts[0].w * (tex.Sample(smp, input.uv3) + tex.Sample(smp, input.uv5 + offsetx));
    //ret += wgts[1].x * (tex.Sample(smp, input.uv4) + tex.Sample(smp, input.uv4 + offsetx));
    //ret += wgts[1].y * (tex.Sample(smp, input.uv5) + tex.Sample(smp, input.uv3 + offsetx));
    //ret += wgts[1].z * (tex.Sample(smp, input.uv6) + tex.Sample(smp, input.uv2 + offsetx));
    //ret += wgts[1].w * (tex.Sample(smp, input.uv7) + tex.Sample(smp, input.uv1 + offsetx));

    //return ret;
    return tex.Sample(smp, input.uv);
    float rx = (input.uv.x - 0.5f);
    float ry = (input.uv.y - 0.5f) * 9 / 16;
    if (rx * rx + ry * ry > 0.04f)
    {
        return tex.Sample(smp, input.uv);
    }
    
    float4 nm = normal.Sample(smp, input.uv);
    float2 offset = (nm.rg - float2(0.5f, 0.5)) * float2(2, -2) * 0.06f;
    return tex.Sample(smp, input.uv + offset);
   

}