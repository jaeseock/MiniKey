#include "vs\se_NoLightUVOnly.vs"
#include "ps\se_CircularZoomingBlur.ps"

technique DefaultTechnique
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}