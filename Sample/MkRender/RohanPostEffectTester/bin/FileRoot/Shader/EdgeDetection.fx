
float2 g_EdgeFactor : UDP = float2(8.f, 1.f); // amplifier, base blend
float3 g_EdgeColor : UDP = float3(0.f, 0.f, 0.f);

texture baseTex : TEXTURE0;
texture depthTex : TEXTURE1;

sampler baseMap = sampler_state { Texture = (baseTex); };
sampler depthMap = sampler_state
{
	Texture = (depthTex);
	MagFilter = LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	const float2 MagicOffset = float2(1.f / 1024.f, 1.f / 768.f);
	
	float4 depthColor = tex2D(depthMap, uv);
	float offsetLT = tex2D(depthMap, uv - MagicOffset).r - depthColor.r;
	float offsetRT = tex2D(depthMap, uv + float2(MagicOffset.x, -MagicOffset.y)).r - depthColor.r;
	float offsetRB = tex2D(depthMap, uv + MagicOffset).r - depthColor.r;
	float offsetLB = tex2D(depthMap, uv + float2(-MagicOffset.x, MagicOffset.y)).r - depthColor.r;
	
	// 차이 벡터 합산
	float totalDiff = offsetLT + offsetRT + offsetRB + offsetLB;
	
	// 증폭
	float diffPower = saturate(totalDiff * g_EdgeFactor.x);
	
	// 선형성 제거
	diffPower = (diffPower < 0.25f) ? 0.f : 1.f;
	
	// 최종
	float4 baseColor = tex2D(baseMap, uv);
	float3 diffuseColor = baseColor.rgb * g_EdgeFactor.y + float3(1.f, 1.f, 1.f) * (1.f - g_EdgeFactor.y);
	float3 finalColor = diffuseColor.rgb * (1.f - diffPower) + g_EdgeColor * diffPower;
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