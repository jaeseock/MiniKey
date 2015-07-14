
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
  
  // max���� ������ normal map�� x(binormal), y(tangent), z(normal)�� ������
  // ���� tangent space�� ��ȯ�� light�� ��������ǥ�踦 ����
  float3 NL = -dLightDir;
  Out.lightDir = float3(dot(In.binormal, NL), dot(In.tangent, NL), dot(In.normal, NL));
  
  // vertex to camera�� tangent space�� �ű�
	float3 posToCamera = normalize(cameraPos - In.pos);
	Out.cameraDir = float3(dot(In.binormal, posToCamera), dot(In.tangent, posToCamera), dot(In.normal, posToCamera));
	
	Out.color0 = dLightColor;
  Out.uv = In.uv;

	return Out;
}

// approximately 17 instruction slots used
