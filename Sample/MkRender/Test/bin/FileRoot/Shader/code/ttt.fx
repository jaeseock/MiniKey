#include "vs\se_NoLightUVOnly.vs"
#include "ps\ttt.ps"


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}