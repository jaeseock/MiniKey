#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - device manager
//
// display device 관련 처리
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkMap.h"

#include "MkPE_MkScreen.h"


#define MK_POST_EFFECT MkPostEffectRenderer::GetInstance()


class MkStr;

class MkPostEffectRenderer
{
public:

	void SetUp(LPDIRECT3DDEVICE9 device, const MkInt2& size, const MkArray<MkStr>& effectFilePathList, bool autoRefresh = false);

	// 크기 재설정
	void Resize(const MkInt2& size);

	// source
	bool CreateSourceRenderTarget(MkRenderToTexture::eTargetFormat format); // single render to texture
	bool CreateSourceRenderTarget(const MkArray<MkRenderToTexture::eTargetFormat>& formats); // MRT
	inline MkRenderTarget& GetSourceRenderTarget(void) { return m_RenderTarget; }

	// screen
	// 이미 존재하면 해제 후 반환
	MkScreen* CreateScreen(int index); // backbuffer
	MkScreen* CreateScreen(int index, MkRenderToTexture::eTargetFormat format); // single render to texture
	MkScreen* CreateScreen(int index, const MkInt2& size, MkRenderToTexture::eTargetFormat format); // single render to texture
	MkScreen* CreateScreen(int index, const MkArray<MkRenderToTexture::eTargetFormat>& formats); // MRT
	MkScreen* CreateScreen(int index, const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats); // MRT
	MkScreen* GetScreen(int index);
	void ClearAllScreens(void);

	// 그리기
	void Draw(void);

	// 해제
	void Clear(void);

	// 리소스 재초기화
	void UnloadResource(void);
	void ReloadResource(void);

	// D3D device 반환
	inline LPDIRECT3DDEVICE9 GetDevice(void) const { return m_Device; }

	// screen size 반환
	inline const MkInt2& GetScreenSize(void) const { return m_ScreenSize; }

	static MkPostEffectRenderer& GetInstance(void);

	MkPostEffectRenderer();
	virtual ~MkPostEffectRenderer() {}

protected:

	MkScreen* _CreateScreen(int index);

protected:

	LPDIRECT3DDEVICE9 m_Device;
	MkInt2 m_ScreenSize;

	MkRenderTarget m_RenderTarget;

	// screen queue
	MkMap<int, MkScreen> m_ScreenQueue;
};
