#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkS2D_MkDrawStep.h"


class MkDrawQueue
{
public:

	// DrawStep ����
	// stepName : step �̸�
	// priority : �׸��� �켱����. ���� �������� ���� �׸�. ������ ��� ���� step�� ������ 0, �ƴϸ� �������� ��ϵ� step�� ���� ������ ����
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type�� eTexture�� ��� �ִ� ä�� ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ũ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ����
	MkDrawStep* CreateStep(
		const MkHashStr& stepName,
		int priority = -1,
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 0,
		const MkUInt2& size = MkUInt2(0, 0),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// step ��ȯ
	MkDrawStep* GetStep(const MkHashStr& stepName);

	// step ����
	bool RemoveStep(const MkHashStr& stepName);

	// �׸���
	bool Draw(void);

	// ����
	void Clear(void);

	MkDrawQueue() {}
	~MkDrawQueue() { Clear(); }

protected:

	MkHashMap<MkHashStr, int> m_NameTable;
	MkMap<int, MkDrawStep*> m_Steps;
};
