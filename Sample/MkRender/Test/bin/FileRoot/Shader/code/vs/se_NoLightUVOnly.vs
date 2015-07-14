
float4x4 wvpMat : WORLDVIEWPROJECTION;

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	Out.pos = mul(float4(In.pos, 1), wvpMat);
	Out.uv = In.uv;

	return Out;
}

// approximately 6 instruction slots used
