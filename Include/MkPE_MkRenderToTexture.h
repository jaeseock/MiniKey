#pragma once


//------------------------------------------------------------------------------------------------//
// render to texture
//
// d3d device�� �����ϱ� ������ device�� reset�Ǵ� ��� �ı� �� ����� �ʿ�
// interface(UnloadResource/ReloadResource)�� �غ� �Ǿ� ������ å���� ����ϴ� ��ü���� �� �־�� ��
//------------------------------------------------------------------------------------------------//

#include "MkPE_MkBaseTexture.h"


class MkRenderToTexture : public MkBaseTexture
{
public:

	enum eTargetFormat
	{
		eTF_RGB = 0,
		eTF_RGBA,
		eTF_Depth // ���� ���� ����� ���� 32bit �� �ؽ���
	};

	// �ʱ�ȭ
	bool SetUp(const MkInt2& size, eTargetFormat format, int targetIndex);

	// ũ�� �缳��
	bool Resize(const MkInt2& size);

	// draw
	void ChangeRenderTarget(void);
	void RestoreRenderTarget(void);

	// ���ҽ� ���ʱ�ȭ
	void UnloadResource(void);
	void ReloadResource(void);
	
public:

	MkRenderToTexture();
	virtual ~MkRenderToTexture();

protected:

	bool _CreateTexture(void);
	void _ClearTarget(LPDIRECT3DDEVICE9 device);

protected:

	eTargetFormat m_TargetFormat;
	LPDIRECT3DSURFACE9 m_DepthBuffer;
	int m_TargetIndex;

	LPDIRECT3DSURFACE9 m_OldRenderTarget;
	LPDIRECT3DSURFACE9 m_OldDepthBuffer;
	D3DVIEWPORT9 m_OldViewport;
};