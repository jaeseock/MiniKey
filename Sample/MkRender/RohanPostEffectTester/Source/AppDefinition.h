#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

enum eDrawStepType
{
	eDST_MainScene = 0,
	eDST_DepthFog,
	eDST_EdgeDetection,
	eDST_HDR_Reduction,
	eDST_HDR_Blend,
	eDST_DepthOfField,
	eDST_RadialBlur,

	eDST_Final, // UI
	eDST_Max
};
