#pragma once


//------------------------------------------------------------------------------------------------//
// draw queue
// - ������ ���� draw step�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkPA_MkRenderTarget.h"


class MkDrawStepInterface;
class MkDrawSceneNodeStep;

class MkDrawQueue
{
public:

	// DrawStep ����
	// stepName : step �̸�
	// priority : �׸��� �켱����. ���� �������� ���� �׸�. ������ ��� ���� step�� ������ 0, �ƴϸ� �������� ��ϵ� step�� ���� ������ ����
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type�� eTexture�� ��� �ִ� ä�� ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ũ��
	// camOffset : camera ��ġ �������� offset
	// texFormat : type�� eTexture�� ��� ���� �� render to texture�� ����
	MkDrawSceneNodeStep* CreateDrawSceneNodeStep(
		const MkHashStr& stepName,
		int priority = -1,
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 1,
		const MkInt2& size = MkInt2(0, 0),
		const MkFloat2& camOffset = MkFloat2(0.f, 0.f),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// step ��ȯ
	MkDrawStepInterface* GetStep(const MkHashStr& stepName);

	// step ����
	void RemoveStep(const MkHashStr& stepName);

	// �׸���
	bool Draw(void);

	// ����
	void Clear(void);

	MkDrawQueue() {}
	~MkDrawQueue() { Clear(); }

protected:

	MkHashMap<MkHashStr, int> m_NameTable;
	MkMap<int, MkDrawStepInterface*> m_Steps;
};
