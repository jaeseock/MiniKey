
#include "MkCore_MkCheck.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkRenderStateSetter.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkDrawingMonitor.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkWindowEventManager.h"

#include "MkS2D_MkSRect.h"
#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkRenderer.h"


#define BKDEF_EMPTY_FRAME_COLOR D3DCOLOR_XRGB(0, 0, 0)


//------------------------------------------------------------------------------------------------//

bool MkRenderer::SetUp(MkListeningWindow* targetWindow, int clientWidth, int clientHeight, bool fullScreen)
{
	MK_CHECK((targetWindow != NULL) && (targetWindow->GetWindowHandle() != NULL), L"NULL window로 MkRenderer 초기화 시도")
		return false;

	// listener에 등록
	targetWindow->RegisterWindowUpdateListener(this);

	// log msg
	MkStr msg;
	msg.Reserve(1024);
	msg += L"< Renderer >";
	msg += MkStr::CR;
	msg += L"   - MkRender version : ";
	msg += m_RendererVersion.ToString();
	MK_DEV_PANEL.MsgToLog(msg, false);
	
	// 장치 초기화
	if (!MK_DISPLAY_MGR.SetUp(targetWindow, MkInt2(clientWidth, clientHeight), fullScreen))
		return false;

	LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();

	MK_DEV_PANEL.MsgToLog(L"", false);

	// scene building용 template node 등록
	MkSRect::__GenerateBuildingTemplate();
	MkSceneNode::__GenerateBuildingTemplate();
	MkBaseWindowNode::__GenerateBuildingTemplate();

	// config file 로딩
	// 시스템간 의존성을 고려해 순서대로 시스템 초기화
	MkDataNode renderConfiguration;
	bool configExist = renderConfiguration.Load(MKDEF_S2D_CONFIG_FILE_NAME);

	// 0. font resource
	if (!MK_FONT_MGR.SetUp(device, (configExist) ? renderConfiguration.GetChildNode(MKDEF_S2D_FONT_RESOURCE_NODE_NAME) : NULL))
		return false;

	if (configExist)
	{
		// 1. scene deco text table
		MkStr decoTablePath;
		if (renderConfiguration.GetData(MKDEF_S2D_SCENE_DECO_TEXT_TABLE_FILE_PATH, decoTablePath, 0) && (!decoTablePath.Empty()))
		{
			MK_CHECK(MK_WIN_RES_MGR.LoadDecoText(decoTablePath), decoTablePath + L" 파일로 scene deco text table 로딩 실패")
				return false;
		}

		// 2. window type image set
		if (renderConfiguration.ChildExist(MKDEF_S2D_WINDOW_TYPE_IMAGE_SET_NODE_NAME))
		{
			MK_WIN_RES_MGR.LoadWindowTypeImageSet(*renderConfiguration.GetChildNode(MKDEF_S2D_WINDOW_TYPE_IMAGE_SET_NODE_NAME));
		}
		
		// 3. window preset
		if (renderConfiguration.ChildExist(MKDEF_S2D_WINDOW_PRESET_NODE_NAME))
		{
			MK_WIN_RES_MGR.LoadWindowPreset(*renderConfiguration.GetChildNode(MKDEF_S2D_WINDOW_PRESET_NODE_NAME));
		}
	}

	// render state 초기화
	MK_RENDER_STATE.SetUp(device);

	return true;
}

void MkRenderer::Update(void)
{
	MkFocusProfiler focusProfiler(L"MkRenderer::Update");

	LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
	if (device == NULL)
		return;

	if (m_RendererState.Check(eDeviceLost))
	{
		HRESULT hr = device->TestCooperativeLevel();
		if (hr == D3DERR_DEVICELOST)
		{
			return; // 아직 device lost 상태
		}
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			if (MK_DISPLAY_MGR.ResetDevice())
			{
				m_RendererState.Clear(eDeviceLost);
			}
		}
	}
	else
	{
		// renderer 이벤트 처리
		{
			MkFocusProfiler focusProfiler(L"MkRenderer::Update(0) renderer event");

			MkArray<_RendererEvent> eventQueue;
			if (m_EventQueue.Consume(eventQueue)) // event 처리
			{
				MK_INDEXING_LOOP(eventQueue, i)
				{
					const _RendererEvent& evt = eventQueue[i];
					switch (evt.eventType)
					{
					case eChangeDisplayMode:
						MK_DISPLAY_MGR.ChangeDisplayMode(evt.arg0, (evt.arg1 == TRUE));
						break;
					case eSetDefaultBackbufferColor:
						m_DefaultBackbufferColor = evt.arg1;
						break;
					}
				}
				eventQueue.Clear();
			}
		}

		// window 이벤트 처리
		{
			MkFocusProfiler focusProfiler(L"MkRenderer::Update(1) window event");

			MK_WIN_EVENT_MGR.Update();
		}

		// 최소화상태가 아니면 렌더링 진행
		if (!m_RendererState.Check(eMinimized))
		{
			MK_DRAWING_MONITOR.Reset();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(2) draw");

				MK_RENDER_STATE.Begin();

				if (!m_DrawQueue.Draw())
				{
					device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), BKDEF_EMPTY_FRAME_COLOR, 1.f, 0);
				}
			}

			MK_DRAWING_MONITOR.EndOfFrame();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(2) present");

				if (device->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST)
				{
					m_RendererState.Set(eDeviceLost);
				}
			}
		}
	}
}

void MkRenderer::Clear(void)
{
	m_EventQueue.Clear();

	m_DrawQueue.Clear();

	MK_TEXTURE_POOL.Clear();
	MK_FONT_MGR.Clear();
	MK_DISPLAY_MGR.Clear();
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ChangeDisplayMode(const MkInt2& size, bool fullScreen)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eChangeDisplayMode, size, (fullScreen) ? TRUE : FALSE));
}

void MkRenderer::SetDefaultBackbufferColor(const MkColor& color)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eSetDefaultBackbufferColor, MkInt2(0, 0), color.ConvertToD3DCOLOR()));
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ListenWindowSizeUpdate(const MkIntRect& newRect)
{
	MK_DISPLAY_MGR.ResetDeviceByClientSize();
}

void MkRenderer::ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect)
{
	switch (newShowCmd)
	{
	case SW_SHOWNORMAL:
	case SW_SHOWMAXIMIZED:
		MK_DISPLAY_MGR.ResetDeviceByClientSize();
		break;

	case SW_SHOWMINIMIZED:
		m_RendererState.Set(eMinimized);
		break;
	}
}

void MkRenderer::ListenWindowRestored(const MkIntRect& newRect)
{
	m_RendererState.Clear(eMinimized);
}

MkRenderer::MkRenderer() : MkSingletonPattern<MkRenderer>()
{
	m_RendererVersion.SetUp(MKDEF_S2D_MAJOR_VERSION, MKDEF_S2D_MINOR_VERSION, MKDEF_S2D_BUILD_VERSION, MKDEF_S2D_REVISION_VERSION);
	m_RendererState.Clear();
	m_DefaultBackbufferColor = D3DCOLOR_XRGB(0, 0, 0);
}

//------------------------------------------------------------------------------------------------//
