#include "vs\obj_1D_DiffSpecRim.vs"
#include "ps\obj_DiffSpecRim.ps"

technique DefaultTechnique
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}