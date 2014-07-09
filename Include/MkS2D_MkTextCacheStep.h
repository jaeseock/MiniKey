#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkDecoStr.h"


class MkTextCacheStep
{
public:

	// 초기화
	bool SetUp(const MkDecoStr& decoStr);

	// 설정된 deco str 반환
	inline const MkDecoStr& GetDecoStr(void) const { return m_DecoStr; }

	// 텍스쳐 참조 반환
	inline void GetTargetTexture(MkBaseTexturePtr& buffer) const { buffer = m_TargetTexture; }

	// 그리기
	void Draw(void);

	// 해제
	void Clear(void);

	MkTextCacheStep();
	virtual ~MkTextCacheStep() { Clear(); }

protected:

	MkDecoStr m_DecoStr;

	MkBaseTexturePtr m_TargetTexture;

	bool m_DrawText;
};
