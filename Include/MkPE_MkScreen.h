#pragma once


//------------------------------------------------------------------------------------------------//
// 주어진 scene node(및 모든 하위)를 그리기 위한 draw step
// backbuffer, 혹은 render to texture에 그릴 수 있음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "MkPE_MkDrawCall.h"
#include "MkPE_MkRenderTarget.h"


class MkScreen
{
public:

	// render target -> backbuffer로 초기화
	void SetUp(void);

	// render target -> render to texture로 초기화
	// size : texture 크기
	bool SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format, bool fixedSize = false);
	bool SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats, bool fixedSize = false);

	// 크기 변경
	// render to texture인 경우에만 해당
	bool Resize(const MkInt2& size);

	// 보이기 설정/반환
	inline void SetEnable(bool visible) { m_Enable = visible; }
	inline bool GetEnable(void) const { return m_Enable; }

	// draw call
	MkDrawCall* CreateDrawCall(int index);
	MkDrawCall* GetDrawCall(int index);

	// 렌더타겟 초기화 설정
	inline void SetClearLastRenderTarget(bool clear) { m_ClearLastRenderTarget = clear; }
	inline bool GetClearLastRenderTarget(void) const { return m_ClearLastRenderTarget; }

	// 렌더 타겟이 render to texture일 경우 텍스쳐 참조 반환
	MkRenderToTexture* GetTargetTexture(unsigned int index) const;

	// 해제
	void Clear(void);

	void UnloadResource(void);
	void ReloadResource(void);

	MkScreen();
	virtual ~MkScreen() { Clear(); }

	void __Draw(LPDIRECT3DDEVICE9 device, const MkFloat2& screenSize);

protected:

	bool m_Enable;
	bool m_ClearLastRenderTarget;
	bool m_FixedSize;

	MkRenderTarget m_RenderTarget;

	// draw call
	MkMap<int, MkDrawCall> m_DrawQueue;
};
