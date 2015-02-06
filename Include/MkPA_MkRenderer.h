#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - renderer
//
// ��� �θ��� �ʴ� 2D ������
// ����� �θ��� �����Ƿ� multi-thread�� ��׶��� �ε��̴� ü�� �������̴� �ϴ� ������ �� ����
// �׳� ������̰� ������ �̹��� ����� ����
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkBitField32.h"
#include "MkCore_MkVersionTag.h"
#include "MkCore_MkWindowUpdateListener.h"
#include "MkCore_MkEventQueuePattern.h"

#include "MkPA_MkDrawQueue.h"


#define MK_RENDERER MkRenderer::Instance()


//------------------------------------------------------------------------------------------------//

class MkColor;
class MkListeningWindow;

class MkRenderer : public MkSingletonPattern<MkRenderer>, public MkWindowUpdateListener
{
public:

	bool SetUp(MkListeningWindow* targetWindow, int clientWidth, int clientHeight, bool fullScreen);

	void Update(void);

	void Clear(void);

	inline MkDrawQueue& GetDrawQueue(void) { return m_DrawQueue; }

	inline const MkVersionTag& GetRendererVersion(void) const { return m_RendererVersion; }

	//------------------------------------------------------------------------------------------------//
	// render event
	//------------------------------------------------------------------------------------------------//

	void ChangeDisplayMode(const MkInt2& size, bool fullScreen);

	void SetDefaultBackbufferColor(const MkColor& color);

	//------------------------------------------------------------------------------------------------//
	// MkWindowUpdateListener
	//------------------------------------------------------------------------------------------------//

	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect);
	virtual void ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect);
	virtual void ListenWindowRestored(const MkIntRect& newRect);

public:

	MkRenderer();
	virtual ~MkRenderer() { Clear(); }

protected:

	enum eRendererState
	{
		eStable = 0, // ���� ����
		eDeviceLost, // device lost ����
		eMinimized // SW_SHOWMINIMIZED ����
	};

	enum eEventType
	{
		eChangeDisplayMode = 0, // arg0 : size, arg1 : fullscreen
		eSetDefaultBackbufferColor // arg1 : color
	};

	typedef MkEventUnitPack2<eEventType, MkInt2, DWORD> _RendererEvent;

protected:

	MkVersionTag m_RendererVersion;

	// ������ ����
	MkBitField32 m_RendererState;

	// �̺�Ʈ
	MkEventQueuePattern<_RendererEvent> m_EventQueue;

	// �ƹ��͵� �׸� ���� ���� ���� Ŭ���� �÷�
	D3DCOLOR m_DefaultBackbufferColor;

	// draw queue
	MkDrawQueue m_DrawQueue;
};
