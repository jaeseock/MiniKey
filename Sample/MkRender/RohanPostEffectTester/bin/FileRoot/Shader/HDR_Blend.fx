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
#define BlendHardLight(base, blend) 	((blend < 0.5f) ? (2.f * base * blend) : (1.f - 2.f * (1.f - base) * (1.f - blend)))
#define BlendColorDodge(base, blend) (base / (1.f - blend))

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 baseColor = tex2D(baseMap, uv);
	float4 blendColor = tex2D(blendMap, uv);
	
	// hard light
	//float hr = (BlendHardLight(baseColor.r, blendColor.r) - baseColor.r) * 0.75f + baseColor.r;
	//float hg = (BlendHardLight(baseColor.g, blendColor.g) - baseColor.g) * 0.75f + baseColor.g;
	//float hb = (BlendHardLight(baseColor.b, blendColor.b) - baseColor.b) * 0.75f + baseColor.b;
	float hr = BlendHardLight(baseColor.r, blendColor.r);
	float hg = BlendHardLight(baseColor.g, blendColor.g);
	float hb = BlendHardLight(baseColor.b, blendColor.b);
	
	// invert
	float3 invertColor = float3(1.f, 1.f, 1.f) - blendColor.rgb;
	
	// overlay
	float or = (BlendOverlayf(hr, 1.f - blendColor.r) - hr) * 0.25f + hr;
	float og = (BlendOverlayf(hg, 1.f - blendColor.g) - hg) * 0.25f + hg;
	float ob = (BlendOverlayf(hb, 1.f - blendColor.b) - hb) * 0.25f + hb;
	
	// brightness
	//float3 brtColor = saturate(float3(or, og, ob) * 1.9f);
	float3 brtColor = float3(or, og, ob) * 1.9f;
	
	// contrast
	//float3 conColor = lerp(float3(0.5f, 0.5f, 0.5f), brtColor, 1.4f);
	
	// color dodge
	float3 dodgeColor = ((brtColor / (float3(1.f, 1.f, 1.f) - baseColor.rgb)) - brtColor) * 0.2f + brtColor;
	
	float3 finalColor = (dodgeColor - baseColor.rgb) * g_HDRPower.x + baseColor.rgb;
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