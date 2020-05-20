
float g_BlendingWeight : UDP = 1.f;

texture baseTex : TEXTURE0;
sampler baseMap = sampler_state
{
	Texture = (baseTex);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	const float2 Magic3X = float2(0.0014648f, 0.0019531f); // float2(1.5f, 1.5f) / float2(1024.f, 768.f);
	const float2 Magic59X = float2(0.0024414f, 0.0058593f); // float2(2.5f, 4.5f) / float2(1024.f, 768.f);
	const float2 Magic95X = float2(0.0043945f, 0.0032552f); // float2(4.5f, 2.5f) / float2(1024.f, 768.f);

	float4 baseColor = tex2D(baseMap, uv);

	float4 sampleColor = tex2D(baseMap, uv - Magic3X);
	sampleColor += tex2D(baseMap, uv + float2(Magic3X.x, -Magic3X.y));
	sampleColor += tex2D(baseMap, uv + Magic3X);
	sampleColor += tex2D(baseMap, uv + float2(-Magic3X.x, Magic3X.y));

	sampleColor += tex2D(baseMap, uv - Magic59X);
	sampleColor += tex2D(baseMap, uv + float2(Magic59X.x, -Magic59X.y));
	sampleColor += tex2D(baseMap, uv + Magic59X);
	sampleColor += tex2D(baseMap, uv + float2(-Magic59X.x, Magic59X.y));

	sampleColor += tex2D(baseMap, uv - Magic95X);
	sampleColor += tex2D(baseMap, uv + float2(Magic95X.x, -Magic95X.y));
	sampleColor += tex2D(baseMap, uv + Magic95X);
	sampleColor += tex2D(baseMap, uv + float2(-Magic95X.x, Magic95X.y));

	sampleColor *= 0.0833f;
	float3 offset = (sampleColor.rgb - baseColor.rgb) * g_BlendingWeight;

	return float4(baseColor.rgb + offset, baseColor.a);
}

technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}