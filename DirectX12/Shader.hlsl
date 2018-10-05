//テクスチャの0番レジスタとサンプラの0番レジスタを設定
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);
cbuffer mat:register(b0) {
	float4x4 world;
	float4x4 viewproj;

	float3 diffuse;
}

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
	float4 normal:NORMAL;
};

Out BasicVS(float4 pos : POSITION/*, float2 uv : TEXCOORD*/, float4 normal : NORMAL)
{
	Out o;
	pos = mul(mul(viewproj, world), pos);
	o.svpos = pos;
	o.pos = pos;
	//o.uv = uv;
	o.normal = mul(world, normal);
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
	//return float4(tex.Sample(smp, o.uv).rgb,1);
	float3 eye = (0,20,-20);
	float3 ray = o.pos.rgb - eye;

	float3 light = normalize(float3(-1,1,1));//光源へのベクトル(平行光源) 
	
	float3 ref = reflect(-light, o.normal.rgb);

	float sec = pow(dot(ref, normalize(ray)), 20);

	float brightness = dot(o.normal, light)+sec;//内積とります 
	/*float3 mat = diffuse.xyz + brightness;*/
	return float4(brightness, brightness, brightness,1);
	//return float4(0.1,1,0.9,1);
	//return float4(o.pos);
}
