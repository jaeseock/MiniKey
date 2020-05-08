#pragma once


//------------------------------------------------------------------------------------------------//
// render to texture
//
// d3d device에 의지하기 때문에 device가 reset되는 경우 파괴 후 재생성 필요
// interface(UnloadResource/ReloadResource)는 준비 되어 있지만 책임은 사용하는 객체에서 해 주어야 함
//------------------------------------------------------------------------------------------------//

#include "MkPE_MkBaseTexture.h"


class MkRenderToTexture : public MkBaseTexture
{
public:

	enum eTargetFormat
	{
		eTF_RGB = 0,
		eTF_RGBA,
		eTF_Depth // 깊이 버퍼 기록을 위한 32bit 논리 텍스쳐
	};

	// 초기화
	bool SetUp(const MkInt2& size, eTargetFormat format, int targetIndex);

	// 크기 재설정
	bool Resize(const MkInt2& size);

	// draw
	void ChangeRenderTarget(void);
	void RestoreRenderTarget(void);

	// 리소스 재초기화
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