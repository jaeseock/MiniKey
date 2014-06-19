
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkS2D_MkBitmapTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkBitmapTexture::SetUp(const MkByteArray& srcData, const MkDataNode* subsetNode)
{
	// 이미지 정보
	D3DXIMAGE_INFO imgInfo;
	MK_CHECK(D3DXGetImageInfoFromFileInMemory(srcData.GetPtr(), srcData.GetSize(), &imgInfo) == D3D_OK, L"이미지 데이터로부터 정보 읽기 실패")
		return false;

	// 텍스쳐 생성
	if (!MkBaseTexture::SetUp(MkUInt2(imgInfo.Width, imgInfo.Height), imgInfo.MipLevels, 0, imgInfo.Format, ePoint, D3DTADDRESS_WRAP, MkColor::Black, subsetNode))
		return false;

	// 서페이스에 이미지 출력
	MK_CHECK(D3DXLoadSurfaceFromFileInMemory(m_Surface, NULL, NULL, srcData.GetPtr(), srcData.GetSize(), NULL, D3DX_FILTER_NONE, 0, NULL) == D3D_OK, L"이미지 데이터로부터 서페이스에 이미지 생성 실패")
	{
		_Clear();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------//
