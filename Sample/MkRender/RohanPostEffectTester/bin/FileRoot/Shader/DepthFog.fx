
float2 g_DFogGraph : UDP = float2(0.25f, 0.75f);
float4 g_DFogBlendColor : UDP = float4(1.f, 0.f, 0.f, 0.3f);
float4 g_DFogMultColor : UDP = float4(0.f, 0.f, 1.f, 0.3f);
float4 g_DFogAddColor : UDP = float4(0.f, 1.f, 0.f, 0.3f);
float3 g_DFogMixer : UDP = float3(1.f, 0.f, 0.f); // blend, mult, add

texture baseTex : TEXTURE0;
texture depthTex : TEXTURE1;

sampler baseMap = sampler_state { Texture = (baseTex); };
sampler depthMap = sampler_state { Texture = (depthTex); };

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	// color
	float4 baseColor = tex2D(baseMap, uv);
	
	float3 blendColor = lerp(baseColor.rgb, g_DFogBlendColor.rgb, g_DFogBlendColor.a);
	float3 multColor = lerp(baseColor.rgb, baseColor.rgb * g_DFogMultColor.rgb, g_DFogMultColor.a);
	float3 addColor = lerp(baseColor.rgb, baseColor.rgb + g_DFogAddColor.rgb, g_DFogAddColor.a);
	
	float3 destColor = blendColor * g_DFogMixer.r + multColor * g_DFogMixer.g + addColor * g_DFogMixer.b;
	
	// power
	float depthValue = tex2D(depthMap, uv).r;
	float fogPower =
		(depthValue <= g_DFogGraph.x) ? 0.f :
		((depthValue >= g_DFogGraph.y) ? 1.f :
		(depthValue - g_DFogGraph.x) / (g_DFogGraph.y - g_DFogGraph.x));
		
	// final
	float3 fogColor = lerp(baseColor.rgb, destColor, fogPower);
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