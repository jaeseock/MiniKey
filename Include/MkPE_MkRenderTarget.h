#pragma once


//------------------------------------------------------------------------------------------------//
// render target
// - 렌더링 대상 정의
// - 백버퍼 혹은 render to texture 가능
//------------------------------------------------------------------------------------------------//

#include "MkPE_MkRenderToTexture.h"


class MkRenderTarget
{
public:

	// render target -> backbuffer로 초기화
	void SetUp(void);

	// render target -> render to texture로 초기화
	bool SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format);
	bool SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats);

	// 크기 변경
	// render to texture인 경우에만 해당
	bool Resize(const MkInt2& size);

	// 해제
	void Clear(void);

	// 배경색 반환
	D3DCOLOR GetBackgroundColor(void) const { return m_BackgroundColor; }

	// 텍스쳐 참조 반환
	const MkRenderToTexture* GetTargetTexture(unsigned int index) const;

	// 리소스 재초기화
	void UnloadResource(void);
	void ReloadResource(void);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	void __Push(void);
	void __Pop(void);
	
public:

	MkRenderTarget();
	~MkRenderTarget() { Clear(); }

protected:

	MkArray<MkRenderToTexture*> m_TargetTexture;
	
	D3DCOLOR m_BackgroundColor;
};
