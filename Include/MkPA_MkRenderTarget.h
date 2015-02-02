#pragma once


//------------------------------------------------------------------------------------------------//
// render target
// - 렌더링 대상 정의
// - 백버퍼 혹은 render to texture 가능
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkRenderToTexture.h"


class MkColor;

class MkRenderTarget
{
public:

	enum eTargetType
	{
		eBackbuffer = 0,
		eTexture
	};

public:

	// 초기화
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type이 eTexture일 경우 최대 채널 수
	// size : type이 eTexture일 경우 생성 할 render to texture의 크기
	// format : type이 eTexture일 경우 생성 할 render to texture의 포맷
	bool SetUp(
		eTargetType type = eBackbuffer,
		unsigned int count = 0,
		const MkInt2& size = MkInt2(0, 0),
		MkRenderToTexture::eFormat format = MkRenderToTexture::eRGBA);

	// 배경색 지정
	void SetBackgroundColor(const MkColor& color);
	void SetBackgroundColor(D3DCOLOR color);

	// 타입 반환
	inline eTargetType GetTargetType(void) const { return m_TargetType; }

	// 해제
	void Clear(void);

	// 지정된 렌더타겟의 크기 반환
	MkInt2 GetScreenSize(void) const;

	// 지정된 영역 정보 반환
	inline const MkFloatRect& GetNewestRegionRect(void) const { return m_RegionRect; }

	// 텍스쳐 참조 반환
	inline unsigned int GetTextureCount(void) const { return m_TargetTexture.GetSize(); }
	const MkRenderToTexture* GetTargetTexture(unsigned int index) const;

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	// begin scene
	bool __BeginScene(void);

	// clear buffer (D3DCLEAR_TARGET / D3DCLEAR_ZBUFFER)
	void __Clear(DWORD clearFlag);

	// end scene
	void __EndScene(void);
	
public:

	MkRenderTarget();
	~MkRenderTarget() { Clear(); }

protected:

	MkArray<MkRenderToTexturePtr> m_TargetTexture;
	
	eTargetType m_TargetType;
	D3DCOLOR m_BackgroundColor;

	MkFloatRect m_RegionRect;
};
