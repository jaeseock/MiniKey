#include "vs\obj_1D_DiffSpecRimNorm.vs"
#include "ps\obj_DiffSpecRimNorm.ps"

technique DefaultTechnique
{
	pass p0
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}