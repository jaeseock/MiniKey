
float4x4 wvpMat : WORLDVIEWPROJECTION;
float3 cameraPos : CAMERAPOSITION;

float3 dLightDir : DLIGHTDIRECTION0;
float3 dLightColor : DLIGHTCOLOR0;

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float3 color0 : COLOR0;
	float2 uv : TEXCOORD0;
	float3 lightDir : TEXCOORD1; // light direction in tangent space
	float3 cameraDir : TEXCOORD2; // vertex to camera direction in tangent space
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
  Out.pos = mul(float4(In.pos, 1), wvpMat);
  
  // max에서 구워진 normal map은 x(binormal), y(tangent), z(normal)로 설정됨
  // 따라서 tangent space로 변환된 light도 오른손좌표계를 따름
  float3 NL = -dLightDir;
  Out.lightDir = float3(dot(In.binormal, NL), dot(In.tangent, NL), dot(In.normal, NL));
  
  // vertex to camera도 tangent space로 옮김
	float3 posToCamera = normalize(cameraPos - In.pos);
	Out.cameraDir = float3(dot(In.binormal, posToCamera), dot(In.tangent, posToCamera), dot(In.normal, posToCamera));
	
	Out.color0 = dLightColor;
  Out.uv = In.uv;

	return Out;
}

// approximately 17 instruction slots used
