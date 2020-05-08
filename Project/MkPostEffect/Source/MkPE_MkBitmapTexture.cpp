
#include "MkCore_MkCheck.h"
#include "MkPE_MkBitmapTexture.h"


//------------------------------------------------------------------------------------------------//

bool MkBitmapTexture::SetUp(const MkByteArray& srcData)
{
	// �̹��� ����
	D3DXIMAGE_INFO imgInfo;
	MK_CHECK(D3DXGetImageInfoFromFileInMemory(srcData.GetPtr(), srcData.GetSize(), &imgInfo) == D3D_OK, L"�̹��� �����ͷκ��� ���� �б� ����")
		return false;

	// �ؽ��� ����
	if (!MkBaseTexture::SetUp(MkInt2(static_cast<int>(imgInfo.Width), static_cast<int>(imgInfo.Height)), 0, imgInfo.Format, eMMFT_Point))
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
