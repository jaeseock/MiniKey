#pragma once


//------------------------------------------------------------------------------------------------//
// (PVC)static image�� ����� ���� draw scene
//
// alpha�� ���� ������ ������ ����
// - ���� ���� : _SetNoneAlphaArguments()�� ����
// - �Ϲ����� ����ä���� ��� : _SetAlphaChannelArguments()�� ����
// - Ư�� �÷�Ű�� ���İ����� ��� : _SetAlphaKeyArguments()�� ����
//
// ���� �׸��� ���� ����� _DrawContents()�� �������� ��� �� ��
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkDrawStepInterface.h"


class MkDrawToStaticTextureStep : public MkDrawStepInterface
{
public:

	// ��� �ؽ��� ��ȯ
	virtual void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const { buffer = m_TargetTexture; }

	// �׸���
	virtual bool Draw(void);

	// ����
	virtual void Clear(void);

	MkDrawToStaticTextureStep();
	virtual ~MkDrawToStaticTextureStep() { Clear(); }

protected:

	enum eAlphaType
	{
		eNone = 0, // ���� ����
		eAlphaChannel, // �Ϲ����� ����ä���� ���
		eAlphaKey // Ư�� �÷�Ű�� ���İ����� ���
	};

	void _SetNoneAlphaArguments(const MkInt2& size, DWORD bgColor);
	void _SetAlphaChannelArguments(const MkInt2& size, DWORD bgColor);
	void _SetAlphaKeyArguments(const MkInt2& size, DWORD alphaKey);

	// (PVF) ���� �׸� ����� ���⼭ ����
	// ��ȯ���� ���� ����
	virtual bool _DrawContents(void) = NULL;

	// �ؽ��� ��ü ����
	// (NOTE) ȣ�� �� _SetNoneAlphaArguments()/_SetAlphaChannelArguments()/_SetAlphaKeyArguments() �� �ϳ��� ����� �����̾�� ��
	bool _CreateTexture(void);

protected:

	MkInt2 m_TextureSize;
	eAlphaType m_AlphaType;
	DWORD m_BackgroundColor;
	DWORD m_AlphaKey;

	MkBaseTexturePtr m_TargetTexture;

	bool m_GenerateTexture;
};
