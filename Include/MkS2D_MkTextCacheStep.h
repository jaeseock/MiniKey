#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkDecoStr.h"


class MkTextCacheStep
{
public:

	// �ʱ�ȭ
	bool SetUp(const MkDecoStr& decoStr);

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
