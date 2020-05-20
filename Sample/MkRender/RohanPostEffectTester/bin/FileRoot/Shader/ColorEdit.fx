// color balance : https://gist.github.com/liovch/3168961

float3 g_ColorFactor : UDP = float3(1.f, 1.f, 1.f); // brightness, saturation, contrast
float3 g_ColorShift : UDP = float3(0.f, 0.f, 0.f); // r, g, b shift

texture baseTex : TEXTURE0;
sampler baseMap = sampler_state { Texture = (baseTex); };


float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 baseColor = tex2D(baseMap, uv);
	
	float3 brtColor = baseColor.rgb * g_ColorFactor.x;
	float intensity = dot(brtColor, float3(0.299f, 0.587f, 0.114f));
	float3 satColor = lerp(float3(intensity, intensity, intensity), brtColor, g_ColorFactor.y);
	float3 conColor = lerp(float3(0.5f, 0.5f, 0.5f), satColor, g_ColorFactor.z);
	
	// color blance
	float origMin = min(min(conColor.r, conColor.g), conColor.b);
    float origMax = max(max(conColor.r, conColor.g), conColor.b);
	float origLightness = (origMax + origMin) * 0.5;
	
	const float A = 0.25f;
	const float B = 0.333f;
	const float SCALE = 0.7f;
	
	float3 midtones = (saturate((origLightness - B) / A + 0.5f) * saturate((origLightness + B - 1.f) / -A + 0.5f) * SCALE) * g_ColorShift;
    float3 newColor = saturate(conColor.rgb + midtones);
	
	return float4(newColor, baseColor.a);
}


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}