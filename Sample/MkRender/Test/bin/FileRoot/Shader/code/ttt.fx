//#include "vs\se_NoLightUVOnly.vs"
#include "ps\ttt.ps"


technique DefaultTechnique
{
	pass p0
	{
		VertexShader = NULL;
		PixelShader = compile ps_2_0 PS();
	}
}