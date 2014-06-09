#pragma once


//------------------------------------------------------------------------------------------------//
// render target
// - �⺻���� ����ۿ� ������
// - render to texture �ؽ��� �߰� �� �ؽ��Ŀ� ������ ����
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkRenderToTexture.h"


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

	// �ʱ�ȭ
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type�� eTexture�� ��� �ִ� ä�� ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ũ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ����
	bool SetUp(
		eTargetType type = eBackbuffer,
		unsigned int count = 0,
		const MkUInt2& size = MkUInt2(0, 0),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// ���� ����
	void SetBackgroundColor(const MkColor& color);
	void SetBackgroundColor(D3DCOLOR color);

	// Ÿ�� ��ȯ
	inline eTargetType GetTargetType(void) const { return m_TargetType; }

	// ����
	void Clear(void);

	// ������ ����Ÿ���� ũ�� ��ȯ
	MkUInt2 GetScreenSize(void);

	// ���� ��ȯ
	inline const MkFloatRect& GetRegionRect(void) const { return m_RegionRect; }

	// �ؽ��� ���� ��ȯ
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
