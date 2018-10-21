//テクスチャの0番レジスタとサンプラの0番レジスタを設定
//Texture2D<float4> tex:register(t0);
Texture2D<float4> modelTex : register(t0);
Texture2D<float4> spa : register(t1);
Texture2D<float4> sph : register(t2);
Texture2D<float4> clut : register(t3);

SamplerState smp:register(s0);
cbuffer mat : register(b0) {
	float4x4 world;
	float4x4 viewproj;
    float4 peye;
}

cbuffer material : register(b1) {
	float4 diffuse;//基本色(拡散反射色) 
	float4 specular;//スペキュラ(反射色) 
	float4 ambient;//アンビエント 
}

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
	float4 normal:NORMAL;
};

Out BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	Out o;
	pos = mul(mul(viewproj, world), pos);
	o.svpos = pos;
	o.pos = pos;
    o.uv = uv;
    o.normal = mul(world, normal);
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
    //return clut.Sample(smp, o.uv);
    //return float4(1, 1, 1, 1);
	//
    
    float3 lightcol = (0.6f, 0.6f, 0.6f);

    float3 eye = peye.xyz;
    float3 ray = normalize(o.pos.xyz - eye);
	float3 light = normalize(float3(-1,1,-1));//光源へのベクトル(平行光源) 
	float3 ref = reflect(-light, o.normal.rgb);
    float spec = saturate(dot(pow(ref, ray), specular.a));
    float brightness = dot(light, o.normal.xyz);
    //brightness = saturate(acos(brightness) / 3.14);
   // return float4(brightness, brightness, brightness, 1);
    float3 vray = normalize(eye - o.pos.xyz );

    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, vray));
    up = normalize(cross(vray, right));
  
    float2 uv = float2(dot(o.normal.xyz, right), dot(o.normal.xyz, up));
    uv = float2(0.5f, -0.5f) * (uv + float2(1.0f, -1.0f));

   // return sph.Sample(smp, uv);

    float3 tooncol = clut.Sample(smp, float2(0, 1.0-brightness));
    float3 spacol = spa.Sample(smp, uv);
    float3 sphcol = sph.Sample(smp, uv);
    float3 texcol = modelTex.Sample(smp, o.uv);
    float3 matcol = tooncol.rgb * diffuse.rgb + specular.rgb * spec + ambient.rgb * lightcol;

    float3 col = spacol.rgb + sphcol.rgb * texcol.rgb * matcol.rgb;
    return float4(col,diffuse.a);
    //return float4(modelTex.Sample(smp, o.uv).rgb * diffuse, 1);
	//float3 color = (exitTex==1) ? tex.Sample(smp, o.uv).rgb : diffuse;
    //return float4(color * brightness, 1);
	//return float4(brightness*diffuse,1);
	//return float4(tex.Sample(smp, o.uv).rgb*diffuse,1);

}
