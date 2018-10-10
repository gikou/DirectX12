//テクスチャの0番レジスタとサンプラの0番レジスタを設定
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);
cbuffer mat : register(b0) {
	float4x4 world;
	float4x4 viewproj;
}

cbuffer material : register(b1) {
	float3 diffuse;//基本色(拡散反射色) 
	float alpha;//;アルファ色 
	float specularity;//スペキュラ強さ 
	float3 specular;//スペキュラ(反射色) 
	float3 mirror;//アンビエント 
	int toonIdx;//トゥーんのインデックス 
	int edgeFlg;//輪郭線フラグ 
	int exitTex;//テクスチャあるか 
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
	//return float4(1.0f,1.0f,1.0f,1.0f);
	//
	float3 eye = (0,20,-20);
	float3 ray = o.pos.rgb - eye;

	float3 light = normalize(float3(-1,1,1));//光源へのベクトル(平行光源) 
	
	float3 ref = reflect(-light, o.normal.rgb);

	float sec = pow(dot(ref, normalize(ray)), 5);

	float brightness = dot(o.normal, light)+sec;//内積とります 

	float3 color = (exitTex==1) ? tex.Sample(smp, o.uv).rgb : diffuse;


	return float4(color*brightness, 1);
	//return float4(brightness*diffuse,1);
	//return float4(tex.Sample(smp, o.uv).rgb*diffuse,1);

}
