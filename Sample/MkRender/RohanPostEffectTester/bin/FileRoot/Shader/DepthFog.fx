
float g_FogMixer : UDP = 0.5f; // 0.f:mult ~ 1.f:add

texture baseTex : TEXTURE0;
texture depthTex : TEXTURE1;
texture fogMulTex : TEXTURE2;
texture fogAddTex : TEXTURE3;

sampler baseMap = sampler_state { Texture = (baseTex); };
sampler depthMap = sampler_state { Texture = (depthTex); };
sampler fogMulMap = sampler_state { Texture = (fogMulTex); AddressU = MIRROR; AddressV = MIRROR; };
sampler fogAddMap = sampler_state { Texture = (fogAddTex); AddressU = MIRROR; AddressV = MIRROR; };

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 baseColor = tex2D(baseMap, uv);
	float4 depthColor = tex2D(depthMap, uv);
	float4 mulColor = tex1D(fogMulMap, depthColor.r);
	float4 addColor = tex1D(fogAddMap, depthColor.r);
	
	float3 fogColor = baseColor.rgb * mulColor.rgb * (1.f - g_FogMixer) + (baseColor.rgb + addColor.rgb) * g_FogMixer;
	return float4(fogColor, baseColor.a);
}


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}