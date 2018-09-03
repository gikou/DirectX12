//テクスチャの0番レジスタとサンプラの0番レジスタを設定
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

cbuffer mat:register(b0) {
	float4x4 wvp;
}

struct Out
{
	float4 svpos : SV_POSITION;
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

Out BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Out o;
	pos = mul(wvp, pos);
	o.svpos = pos;
	o.pos = pos;
	o.uv = uv;
	return o;
}

float4 BasicPS(Out o) : SV_TARGET
{
	return float4(tex.Sample(smp, o.uv).abg, 1);
	//return float4(o.pos);
}
