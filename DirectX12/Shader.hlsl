//テクスチャの0番レジスタとサンプラの0番レジスタを設定
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

cbuffer mat:register(b0) {
	float4x4 world;
	float4x4 viewproj;
}

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

Out BasicVS(float3 pos : POSITION/*, float2 uv : TEXCOORD*/, float3 normal : NORMAL)
{
	Out o;
	float4 position = float4(pos, 1);
	position = mul(mul(viewproj, world), position);
	o.svpos = position;
	o.pos = position;
	//o.uv = uv;
	o.normal = mul(world,normal);
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
	//return float4(tex.Sample(smp, o.uv).abg, 1);
	float3 light = normalize(float3(-1,1,-1));
	float brightness = dot(o.normal, light); 
	return float4(brightness, brightness, brightness,1);
	//return float4(1,1,1,1);
}
