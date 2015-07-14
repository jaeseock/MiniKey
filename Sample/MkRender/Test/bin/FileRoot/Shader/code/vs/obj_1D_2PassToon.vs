
float4x4 worldMat : WORLD;
float4x4 viewMat : VIEW;
float4x4 projMat : PROJECTION;
float4x4 wvpMat : WORLDVIEWPROJECTION;
float3 cameraPos : CAMERAPOSITION;
float3 dLightDir : DLIGHTDIRECTION0;

float outlineThickness : UDP <string def = "1";>;

struct OutlinePassVS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct OutlinePassVS_OUT
{
	float4 pos : POSITION;
};

struct DiffSpecVS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct DiffSpecVS_OUT
{
	float4 pos : POSITION;
	float3 color0 : COLOR0; // main light power(r), specular power(g)
	float2 uv : TEXCOORD0;
};


OutlinePassVS_OUT OutlinePassVS(OutlinePassVS_IN In)
{
  OutlinePassVS_OUT Out = (OutlinePassVS_OUT)0;
	float4 wp = mul(float4(In.pos + In.normal * outlineThickness, 1), worldMat);
	float4 vp = mul(float4(wp.xyz, 1), viewMat);
	Out.pos = mul(float4(vp.xyz, 1), projMat);
	
	return Out;
}
// approximately 15 instruction slots used

DiffSpecVS_OUT DiffSpecVS(DiffSpecVS_IN In)
{
  DiffSpecVS_OUT Out = (DiffSpecVS_OUT)0;
	Out.pos = mul(float4(In.pos, 1), wvpMat);
	
	float NDotL = dot(In.normal, -dLightDir);
	Out.color0.x = (1.f + NDotL) * 0.5f; // (-1 ~ 1) -> (0 ~ 1)
	
	float3 posToCamera = normalize(cameraPos - In.pos);
	float3 reflectionVec = 2.f * NDotL * In.normal + dLightDir;
	Out.color0.y = pow(max(0.f, dot(reflectionVec, posToCamera)), 2.f); // phong. sharpness = 2.f
	
	Out.uv = In.uv;
	
	return Out;
}
// approximately 19 instruction slots used
