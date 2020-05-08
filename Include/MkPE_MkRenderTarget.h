#pragma once


//------------------------------------------------------------------------------------------------//
// render target
// - ������ ��� ����
// - ����� Ȥ�� render to texture ����
//------------------------------------------------------------------------------------------------//

#include "MkPE_MkRenderToTexture.h"


class MkRenderTarget
{
public:

	// render target -> backbuffer�� �ʱ�ȭ
	void SetUp(void);

	// render target -> render to texture�� �ʱ�ȭ
	bool SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format);
	bool SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats);

	// ũ�� ����
	// render to texture�� ��쿡�� �ش�
	bool Resize(const MkInt2& size);

	// ����
	void Clear(void);

	// ���� ��ȯ
	D3DCOLOR GetBackgroundColor(void) const { return m_BackgroundColor; }

	// �ؽ��� ���� ��ȯ
	const MkRenderToTexture* GetTargetTexture(unsigned int index) const;

	// ���ҽ� ���ʱ�ȭ
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
