#pragma once


//------------------------------------------------------------------------------------------------//
// render to texture
//
// d3d device�� �����ϱ� ������ device�� reset�Ǵ� ��� �ı� �� ����� �ʿ�
// interface(UnloadResource/ReloadResource)�� �غ� �Ǿ� ������ å���� ����ϴ� ��ü���� �� �־�� ��
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseTexture.h"


class MkRenderToTexture : public MkBaseTexture
{
public:

	enum eFormat
	{
		eRGB = 0,
		eRGBA,
		eDepth // ���� ���� ����� ���� 32bit �� �ؽ���
	};

	// �ʱ�ȭ
	bool SetUp(const MkInt2& size, eFormat format, int targetIndex);

	void ChangeRenderTarget(void);
	void RestoreRenderTarget(void);

	void UnloadResource(void);
	void ReloadResource(LPDIRECT3DDEVICE9 device);
	
public:

	MkRenderToTexture();
	virtual ~MkRenderToTexture();

protected:

	void _ClearTarget(LPDIRECT3DDEVICE9 device);

protected:

	eFormat m_RenderToTextureFormat;
	LPDIRECT3DSURFACE9 m_DepthBuffer;
	int m_TargetIndex;

	LPDIRECT3DSURFACE9 m_OldRenderTarget;
	LPDIRECT3DSURFACE9 m_OldDepthBuffer;
	D3DVIEWPORT9 m_OldViewport;
};

typedef MkSharedPointer<MkRenderToTexture> MkRenderToTexturePtr;