#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - renderer
//
// 욕심 부리지 않는 2D 렌더러
// 욕심을 부리지 않으므로 multi-thread니 백그라운드 로딩이니 체인 렌더링이니 하는 종류는 다 무시
// 그냥 상식적이고 심플한 이미지 출력이 전부
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
		eStable = 0, // 정상 진행
		eDeviceLost, // device lost 상태
		eMinimized // SW_SHOWMINIMIZED 상태
	};

	enum eEventType
	{
		eChangeDisplayMode = 0, // arg0 : size, arg1 : fullscreen
		eSetDefaultBackbufferColor // arg1 : color
	};

	typedef MkEventUnitPack2<eEventType, MkInt2, DWORD> _RendererEvent;

protected:

	MkVersionTag m_RendererVersion;

	// 렌더러 상태
	MkBitField32 m_RendererState;

	// 이벤트
	MkEventQueuePattern<_RendererEvent> m_EventQueue;

	// 아무것도 그릴 것이 없을 때의 클리어 컬러
	D3DCOLOR m_DefaultBackbufferColor;

	// draw queue
	MkDrawQueue m_DrawQueue;
};
