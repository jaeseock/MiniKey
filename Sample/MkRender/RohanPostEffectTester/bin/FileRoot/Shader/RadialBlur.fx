// 샘플링 위치 산출. 퀄리티와 인스트럭션 수를 고려하면 8회 샘플링이 적당
// e(0) = 1.0, f = 1.05, e(i+1) = e(i) * f
// { 1.000, 1.050, 1.102, 1.158, 1.216, 1.276, 1.340, 1.407 }
// 0부터 누적. a(-1) = 0.0, a(i) = a(i-1) + e(i)
// { 1.000, 2.050, 3.152, 4.310, 5.526, 6.802, 8.142, 9.549 }
// 마지막 위치에서 원본을 샘플링하기 때문에 끝 값을 0에 맞추기 위해 -9.549 쉬프팅. x(i) = a(i) - 9.549
// { -8.549, -7.499, -6.397, -5.239, -4.023, -2.747, -1.407, 0.000 }
// 여기서 원본인 마지막 값을 제외하면 최종적으로,
static const float SamplingPos[7] = {-8.549f, -7.499f, -6.397f, -5.239f, -4.023f, -2.747f, -1.407f};

// uv좌표상 factor
// xy : 초점 위치. (0.f ~ 1.f, 0.f ~ 1.f)
// z : 블러가 시작되는 반지름. (0.f ~ 1.f)
// w : 샘플링 단위 구간(0 ~). 단위 구간을 기준으로 8회 증가되며 샘플링 되므로 커질수록 블러 강도가 세짐
float4 g_RadialBlurFactor : UDP = float4(0.5f, 0.5f, 0.1f, 0.01f);

texture baseTex : TEXTURE0;
sampler baseMap = sampler_state { Texture = (baseTex); MagFilter = LINEAR; };

float4 PS(float2 uv : TEXCOORD0) : COLOR
{
	float4 diffuseColor = tex2D(baseMap, uv);
	float2 focusToUV = uv - g_RadialBlurFactor.xy;
	float distFocusToUV = length(focusToUV);
	
	if (distFocusToUV > 0.f)
	{
		float distStartToUV = distFocusToUV - g_RadialBlurFactor.z;
		
		if (distStartToUV > 0.f)
		{
			float2 direction = focusToUV / distFocusToUV; // normalize
			float movement = distStartToUV * g_RadialBlurFactor.w;

			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[0] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[1] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[2] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[3] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[4] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[5] * direction);
			diffuseColor += tex2D(baseMap, uv + movement * SamplingPos[6] * direction);
			
			diffuseColor *= 0.125f; // 원본 + 추가 7 샘플링이므로 8로 나누어 평균을 냄
		}
	}
	return diffuseColor;
}

technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}