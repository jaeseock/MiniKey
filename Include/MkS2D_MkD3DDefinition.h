#pragma once


//------------------------------------------------------------------------------------------------//
// d3d definition
//------------------------------------------------------------------------------------------------//

#include <d3dx9.h>
#include "MkCore_MkValueDecision.h"

// 크기 고정
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DRECT)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DCOLORVALUE)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DVECTOR)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DXVECTOR2)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DXVECTOR3)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DMATRIX)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DXMATRIX)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DMATERIAL9)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DLIGHT9)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DVIEWPORT9)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DVERTEXELEMENT9)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DDISPLAYMODE)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DVERTEXBUFFER_DESC)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DINDEXBUFFER_DESC)
MKDEF_DECLARE_FIXED_SIZE_TYPE(D3DSURFACE_DESC)

MKDEF_VALUE_DECISION_SIZE_ONLY_COMPARED_COMMIT(D3DMATERIAL9)

// 자료형 변환(D3D type <-> minikey type)
class MkVec2;
class MkVec3;
class MkMat3;

class MkD3DType
{
public:

	// MkVec2 -> D3DXVECTOR2
	static void Convert(const MkVec2& src, D3DXVECTOR2& dest);
	static D3DXVECTOR2 Convert(const MkVec2& src);
	
	// D3DXVECTOR2 -> MkVec2
	static void Convert(const D3DXVECTOR2& src, MkVec2& dest);
	static MkVec2 Convert(const D3DXVECTOR2& src);

	// MkVec3 -> D3DXVECTOR3
	static void Convert(const MkVec3& src, D3DXVECTOR3& dest);
	static D3DXVECTOR3 Convert(const MkVec3& src);

	// D3DXVECTOR3 -> MkVec3
	static void Convert(const D3DXVECTOR3& src, MkVec3& dest);
	static MkVec3 Convert(const D3DXVECTOR3& src);

	// MkMat3 -> D3DXMATRIX
	static void Convert(const MkMat3& src, D3DXMATRIX& dest);
	static D3DXMATRIX Convert(const MkMat3& src);

	// D3DXMATRIX -> MkMat3
	static void Convert(const D3DXMATRIX& src, MkMat3& dest);
	static MkMat3 Convert(const D3DXMATRIX& src);

	// identity D3DXMATRIX 반환
	static const D3DXMATRIX& GetIdentityMatrix(void);
};