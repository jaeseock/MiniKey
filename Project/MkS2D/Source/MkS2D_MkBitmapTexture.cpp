
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkS2D_MkBitmapTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkBitmapTexture::SetUp(const MkByteArray& srcData, const MkDataNode* subsetNode)
{
	// �̹��� ����
	D3DXIMAGE_INFO imgInfo;
	MK_CHECK(D3DXGetImageInfoFromFileInMemory(srcData.GetPtr(), srcData.GetSize(), &imgInfo) == D3D_OK, L"�̹��� �����ͷκ��� ���� �б� ����")
		return false;

	// �ؽ��� ����
	if (!MkBaseTexture::SetUp(MkUInt2(imgInfo.Width, imgInfo.Height), imgInfo.MipLevels, 0, imgInfo.Format, ePoint, D3DTADDRESS_WRAP, MkColor::Black, subsetNode))
		return false;

	// �����̽��� �̹��� ���
	MK_CHECK(D3DXLoadSurfaceFromFileInMemory(m_Surface, NULL, NULL, srcData.GetPtr(), srcData.GetSize(), NULL, D3DX_FILTER_NONE, 0, NULL) == D3D_OK, L"�̹��� �����ͷκ��� �����̽��� �̹��� ���� ����")
	{
		_Clear();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------//
