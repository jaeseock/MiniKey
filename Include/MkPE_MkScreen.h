#pragma once


//------------------------------------------------------------------------------------------------//
// �־��� scene node(�� ��� ����)�� �׸��� ���� draw step
// backbuffer, Ȥ�� render to texture�� �׸� �� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"

#include "MkPE_MkDrawCall.h"
#include "MkPE_MkRenderTarget.h"


class MkScreen
{
public:

	// render target -> backbuffer�� �ʱ�ȭ
	void SetUp(void);

	// render target -> render to texture�� �ʱ�ȭ
	// size : texture ũ��
	bool SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format, bool fixedSize = false);
	bool SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats, bool fixedSize = false);

	// ũ�� ����
	// render to texture�� ��쿡�� �ش�
	bool Resize(const MkInt2& size);

	// ���̱� ����/��ȯ
	inline void SetEnable(bool visible) { m_Enable = visible; }
	inline bool GetEnable(void) const { return m_Enable; }

	// draw call
	MkDrawCall* CreateDrawCall(int index);
	MkDrawCall* GetDrawCall(int index);

	// ����Ÿ�� �ʱ�ȭ ����
	inline void SetClearLastRenderTarget(bool clear) { m_ClearLastRenderTarget = clear; }
	inline bool GetClearLastRenderTarget(void) const { return m_ClearLastRenderTarget; }

	// ���� Ÿ���� render to texture�� ��� �ؽ��� ���� ��ȯ
	MkRenderToTexture* GetTargetTexture(unsigned int index) const;

	// ����
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
