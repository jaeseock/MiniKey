// https://www.flickr.com/photos/kolnedra/2257529395

float2 g_HDRPower : UDP = float2(0.8f, 0.3f);

texture baseTex : TEXTURE0;
texture blendTex : TEXTURE1;

sampler baseMap = sampler_state { Texture = (baseTex); };
sampler blendMap = sampler_state
{
	Texture = (blendTex);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

#define BlendAddf(base, blend) min(base + blend, 1.f)
#define BlendSubstractf(base, blend) max(base + blend - 1.f, 0.f)
#define BlendLinearLightf(base, blend) ((blend < 0.5f) ? BlendSubstractf(base, (2.f * blend)) : BlendAddf(base, (2.f * (blend - 0.5f))))
#define BlendOverlayf(base, blend) 	((base < 0.5f) ? (2.f * base * blend) : (1.f - 2.f * (1.f - base) * (1.f - blend)))

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 baseColor = tex2D(baseMap, uv);
	float4 blendColor = tex2D(blendMap, uv);
	
	// invert
	float3 invertColor = float3(1.f, 1.f, 1.f) - blendColor.rgb;
	
	// overlay
	float or = BlendOverlayf(baseColor.r, invertColor.r);
	float og = BlendOverlayf(baseColor.g, invertColor.g);
	float ob = BlendOverlayf(baseColor.b, invertColor.b);
	
	// linear light
	float lr = BlendLinearLightf(or, blendColor.r);
	float lg = BlendLinearLightf(og, blendColor.g);
	float lb = BlendLinearLightf(ob, blendColor.b);
	
	float3 finalColor = float3(lr, lg, lb) * g_HDRPower.x + float3(or, og, ob) * g_HDRPower.y;
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