
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkMat3.h"
#include "MkPE_MkD3DDefinition.h"


//------------------------------------------------------------------------------------------------//

void MkD3DType::Convert(const MkVec2& src, D3DXVECTOR2& dest)
{
	dest.x = src.x;
	dest.y = src.y;
}

D3DXVECTOR2 MkD3DType::Convert(const MkVec2& src)
{
	return D3DXVECTOR2(src.x, src.y);
}

void MkD3DType::Convert(const D3DXVECTOR2& src, MkVec2& dest)
{
	dest.x = src.x;
	dest.y = src.y;
}

MkVec2 MkD3DType::Convert(const D3DXVECTOR2& src)
{
	return MkVec2(src.x, src.y);
}

void MkD3DType::Convert(const MkVec3& src, D3DXVECTOR3& dest)
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

D3DXVECTOR3 MkD3DType::Convert(const MkVec3& src)
{
	return D3DXVECTOR3(src.x, src.y, src.z);
}

void MkD3DType::Convert(const D3DXVECTOR3& src, MkVec3& dest)
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

MkVec3 MkD3DType::Convert(const D3DXVECTOR3& src)
{
	return MkVec3(src.x, src.y, src.z);
}
/*
void MkD3DType::Convert(const MkMat3& src, D3DXMATRIX& dest)
{
	dest.m[0][0] = src.m[0][0];
	dest.m[0][1] = src.m[0][1];
	dest.m[0][2] = src.m[0][2];
	dest.m[0][3] = 0.f;
	dest.m[1][0] = src.m[1][0];
	dest.m[1][1] = src.m[1][1];
	dest.m[1][2] = src.m[1][2];
	dest.m[1][3] = 0.f;
	dest.m[2][0] = src.m[2][0];
	dest.m[2][1] = src.m[2][1];
	dest.m[2][2] = src.m[2][2];
	dest.m[2][3] = 0.f;
	dest.m[3][0] = 0.f;
	dest.m[3][1] = 0.f;
	dest.m[3][2] = 0.f;
	dest.m[3][3] = 1.f;
}

D3DXMATRIX MkD3DType::Convert(const MkMat3& src)
{
	D3DXMATRIX dest;
	Convert(src, dest);
	return dest;
}

void MkD3DType::Convert(const D3DXMATRIX& src, MkMat3& dest)
{
	dest.m[0][0] = src.m[0][0];
	dest.m[0][1] = src.m[0][1];
	dest.m[0][2] = src.m[0][2];
	dest.m[1][0] = src.m[1][0];
	dest.m[1][1] = src.m[1][1];
	dest.m[1][2] = src.m[1][2];
	dest.m[2][0] = src.m[2][0];
	dest.m[2][1] = src.m[2][1];
	dest.m[2][2] = src.m[2][2];
}

MkMat3 MkD3DType::Convert(const D3DXMATRIX& src)
{
	MkMat3 dest;
	Convert(src, dest);
	return dest;
}
*/
const D3DXMATRIX& MkD3DType::GetIdentityMatrix(void)
{
	static D3DXMATRIX mat(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);
	return mat;
}

//------------------------------------------------------------------------------------------------//
