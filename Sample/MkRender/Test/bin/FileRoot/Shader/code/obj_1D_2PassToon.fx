#include "vs\obj_1D_2PassToon.vs"
#include "ps\obj_1D_2PassToon.ps"


technique DefaultTechnique
{
	pass p0
	{
	  CULLMODE = CW;
	  
		VertexShader = compile vs_2_0 OutlinePassVS();
		PixelShader = compile ps_2_0 OuterColorPassPS();
	}

	pass p1
	{
	  CULLMODE = CCW;
	  
		VertexShader = compile vs_2_0 DiffSpecVS();
		PixelShader = compile ps_2_0 ToonRampPS();
	}
}