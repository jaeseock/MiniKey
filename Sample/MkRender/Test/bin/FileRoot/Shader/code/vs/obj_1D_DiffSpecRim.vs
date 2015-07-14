
float4x4 wvpMat : WORLDVIEWPROJECTION;
float3 cameraPos : CAMERAPOSITION;

float3 dLightDir : DLIGHTDIRECTION0;
float3 dLightColor : DLIGHTCOLOR0;

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float3 color0 : COLOR0; // main light color
	float3 color1 : COLOR1; // specular power(r), rim power(g)
	float2 uv : TEXCOORD0;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
  Out.pos = mul(float4(In.pos, 1), wvpMat);
  
  float NDotL = dot(In.normal, -dLightDir);
	float dLightPower = saturate((0.5f + NDotL) * 0.7f); // Lambert. -0.35 ~ 1.05
	Out.color0 = dLightPower * dLightColor;
	
	float3 posToCamera = normalize(cameraPos - In.pos);
	float3 reflectionVec = 2.f * NDotL * In.normal + dLightDir;
	Out.color1.x = pow(max(0.f, dot(reflectionVec, posToCamera)), 2.f); // phong. sharpness = 2.f
	
	float rimPower = 1.f - saturate(dot(In.normal, posToCamera));
	Out.color1.y = rimPower * rimPower; // pow 2
	
  Out.uv = In.uv;

	return Out;
}

// approximately 26 instruction slots used
