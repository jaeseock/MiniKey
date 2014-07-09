#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkDecoStr.h"


class MkTextCacheStep
{
public:

	// �ʱ�ȭ
	bool SetUp(const MkDecoStr& decoStr);

	// ������ deco str ��ȯ
	inline const MkDecoStr& GetDecoStr(void) const { return m_DecoStr; }

	// �ؽ��� ���� ��ȯ
	inline void GetTargetTexture(MkBaseTexturePtr& buffer) const { buffer = m_TargetTexture; }

	// �׸���
	void Draw(void);

	// ����
	void Clear(void);

	MkTextCacheStep();
	virtual ~MkTextCacheStep() { Clear(); }

protected:

	MkDecoStr m_DecoStr;

	MkBaseTexturePtr m_TargetTexture;

	bool m_DrawText;
};
