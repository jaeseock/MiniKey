
float3 g_DOF_Factor : UDP = float3(0.000625f, 0.15f, 0.f); // 위치, 범위, 디버깅용 컬러비율

texture baseTex : TEXTURE0;
texture depthTex : TEXTURE1;

sampler baseMap = sampler_state
{
	Texture = (baseTex);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

sampler depthMap = sampler_state { Texture = (depthTex); };

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	const float2 MagicOffset = float2(1.5f / 1024.f, 1.5f / 768.f);
	
	float4 baseColor = tex2D(baseMap, uv);
	float4 c0 = tex2D(baseMap, uv - MagicOffset);
	float4 c1 = tex2D(baseMap, uv + float2(MagicOffset.x, -MagicOffset.y));
	float4 c2 = tex2D(baseMap, uv + MagicOffset);
	float4 c3 = tex2D(baseMap, uv + float2(-MagicOffset.x, MagicOffset.y));
	float3 bluredColor = (baseColor.rgb + c0.rgb + c1.rgb + c2.rgb + c3.rgb) * 0.2f;
	
	float4 depthColor = tex2D(depthMap, uv);
	
	float focusing = depthColor.r - g_DOF_Factor.x;
	float blurPower = saturate(focusing * focusing * g_DOF_Factor.y);
	float3 worldColor = (bluredColor * blurPower  + baseColor.rgb * (1.f - blurPower));
	float3 finalColor = (float3(blurPower, blurPower, blurPower) * g_DOF_Factor.z  + worldColor * (1.f - g_DOF_Factor.z));
	return float4(finalColor, baseColor.a);
}


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}