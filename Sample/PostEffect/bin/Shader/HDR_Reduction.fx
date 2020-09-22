// https://stackoverflow.com/questions/36303950/hlsl-gaussian-blur-effect

texture baseTex : TEXTURE0;
sampler baseMap = sampler_state
{
	Texture = (baseTex);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
};


float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	const float offset[] = { 0.f, 1.f, 2.f, 3.f };
	const float weight[] = { 0.22702f, 0.097295f, 0.06081f, 0.029027f };
	
	float4 baseColor = tex2D(baseMap, uv);
	float3 fragmentColor = baseColor.rgb * weight[0];
	
	float hstep = 1.f / 128.f;
	float vstep = 1.f / 96.f;

	for (int i=1; i<4; ++i)
	{
		float2 hmove = float2(hstep*offset[i], 0.f);
		fragmentColor += tex2D(baseMap, uv + hmove).rgb * weight[i];
		fragmentColor += tex2D(baseMap, uv - hmove).rgb * weight[i];
		
		float2 vmove = float2(0.f, vstep*offset[i]);
		fragmentColor += tex2D(baseMap, uv + vmove).rgb * weight[i];
		fragmentColor += tex2D(baseMap, uv - vmove).rgb * weight[i];
    }
	return float4(fragmentColor, baseColor.a);
}


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}