

texture baseTex : TEXTURE0;
sampler baseMap = sampler_state { Texture = (baseTex); };

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 depthColor = tex2D(baseMap, uv);
	return float4(depthColor.rrr, depthColor.a);
}


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}