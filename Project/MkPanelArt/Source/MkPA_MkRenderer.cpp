
#include "MkCore_MkCheck.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkCursorManager.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkSceneNode.h"
//#include "MkS2D_MkWindowResourceManager.h"
//#include "MkS2D_MkHiddenEditBox.h"

//#include "MkS2D_MkSRect.h"
//#include "MkS2D_MkSceneNodeFamilyDefinition.h"

#include "MkPA_MkRenderer.h"


#define MKDEF_EMPTY_FRAME_COLOR D3DCOLOR_XRGB(0, 0, 0)


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
	msg += MkStr::CRLF;
	msg += L"   - MkRender version : ";
	msg += m_RendererVersion.ToString();
	MK_DEV_PANEL.MsgToLog(msg, false);
	
	// 장치 초기화
	if (!MK_DEVICE_MGR.SetUp(targetWindow, MkInt2(clientWidth, clientHeight), fullScreen))
		return false;

	MK_DEV_PANEL.MsgToLog(L"", false);

	// hidden edit box
	MK_EDIT_BOX.SetUp(targetWindow->GetWindowHandle(), targetWindow->GetInstanceHandle());

	// build scene node hierarchy
	MkSceneNode::__BuildSceneNodeTypeHierarchy();

	// hidden edit box
	//MK_EDIT_BOX.SetUp(targetWindow->GetWindowHandle(), targetWindow->GetInstanceHandle());

	// config file 로딩
	// 시스템간 의존성을 고려해 순서대로 시스템 초기화
	MkDataNode renderConfigNode;
	bool configExist = renderConfigNode.Load(MKDEF_PA_CONFIG_FILE_NAME);

	// 0. font resource
	MK_FONT_MGR.SetUp(renderConfigNode.GetChildNode(L"FontResource"), L"맑은고딕");

	// 1. static resource
	const MkDataNode* staticResourceNode = renderConfigNode.GetChildNode(L"StaticResource");

	// 1.0. text
	MK_STATIC_RES.LoadTextNode(staticResourceNode);

	// 1.1. theme
	if (!MK_STATIC_RES.LoadWindowThemeSet(staticResourceNode->GetChildNode(L"Theme")))
		return false;
	
	// 3. cursor list
	MK_CURSOR_MGR.SetUp(renderConfigNode.GetChildNode(L"CursorType"));
	MK_CURSOR_MGR.SetCursorType(MkCursorManager::eNormal);

/*
	if (configExist)
	{
		// 1. scene deco text table
		MkStr decoTablePath;
		if (renderConfigNode.GetData(MKDEF_PA_SCENE_DECO_TEXT_TABLE_FILE_PATH, decoTablePath, 0) && (!decoTablePath.Empty()))
		{
			MK_CHECK(MK_WIN_RES_MGR.LoadDecoText(decoTablePath), decoTablePath + L" 파일로 scene deco text table 로딩 실패")
				return false;
		}

		// 2. window type image set
		if (renderConfigNode.ChildExist(MKDEF_PA_WINDOW_TYPE_IMAGE_SET_NODE_NAME))
		{
			MK_WIN_RES_MGR.LoadWindowTypeImageSet(*renderConfigNode.GetChildNode(MKDEF_PA_WINDOW_TYPE_IMAGE_SET_NODE_NAME));
		}
		
		// 3. window preset
		if (renderConfigNode.ChildExist(MKDEF_PA_WINDOW_PRESET_NODE_NAME))
		{
			MK_WIN_RES_MGR.LoadWindowPreset(*renderConfigNode.GetChildNode(MKDEF_PA_WINDOW_PRESET_NODE_NAME));
		}

		
	}

	// scene building용 template node 등록
	MkSRect::__GenerateBuildingTemplate();
	MkSceneNodeFamilyDefinition::GenerateBuildingTemplate();
*/
	return true;
}

void MkRenderer::Update(void)
{
	MkFocusProfiler focusProfiler(L"MkRenderer::Update");

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
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
			if (MK_DEVICE_MGR.ResetDevice())
			{
				m_RendererState.Clear(eDeviceLost);
			}
		}
	}
	else
	{
		// renderer 이벤트 처리
		{
			MkFocusProfiler focusProfiler(L"MkRenderer::Update(0) event op.");

			MkArray<_RendererEvent> eventQueue;
			if (m_EventQueue.Consume(eventQueue)) // event 처리
			{
				MK_INDEXING_LOOP(eventQueue, i)
				{
					const _RendererEvent& evt = eventQueue[i];
					switch (evt.eventType)
					{
					case eChangeDisplayMode:
						MK_DEVICE_MGR.ChangeDisplayMode(evt.arg0, (evt.arg1 == TRUE));
						MK_INPUT_MGR.UpdateTargetWindowClientSize(NULL);
						break;
					case eSetDefaultBackbufferColor:
						m_DefaultBackbufferColor = evt.arg1;
						break;
					}
				}
				eventQueue.Clear();
			}
		}

		// 최소화상태가 아니면 렌더링 진행
		if (!m_RendererState.Check(eMinimized))
		{
			MK_DRAWING_MONITOR.Reset();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(1) update");

				MK_STATIC_RES.Update();
			}

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(2) draw");

				MK_RENDER_STATE.Begin();

				if (!m_DrawQueue.Draw())
				{
					device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), MKDEF_EMPTY_FRAME_COLOR, 1.f, 0);
				}
			}
			
			MK_DRAWING_MONITOR.EndOfFrame();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(3) present");

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

	MK_STATIC_RES.Clear();
	MK_CURSOR_MGR.Clear();
	MK_BITMAP_POOL.Clear();
	MK_FONT_MGR.Clear();
	MK_DEVICE_MGR.Clear();
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ChangeDisplayMode(const MkInt2& size, bool fullScreen)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eChangeDisplayMode, size, (fullScreen) ? TRUE : FALSE));
}

void MkRenderer::SetDefaultBackbufferColor(const MkColor& color)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eSetDefaultBackbufferColor, MkInt2::Zero, color.ConvertToD3DCOLOR()));
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ListenWindowSizeUpdate(const MkIntRect& newRect)
{
	MK_DEVICE_MGR.ResetDeviceByClientSize();
}

void MkRenderer::ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect)
{
	switch (newShowCmd)
	{
	case SW_SHOWNORMAL:
	case SW_SHOWMAXIMIZED:
		MK_DEVICE_MGR.ResetDeviceByClientSize();
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
	m_RendererVersion.SetUp(MKDEF_PA_MAJOR_VERSION, MKDEF_PA_MINOR_VERSION, MKDEF_PA_BUILD_VERSION, MKDEF_PA_REVISION_VERSION);
	m_RendererState.Clear();
	m_DefaultBackbufferColor = D3DCOLOR_XRGB(0, 0, 0);
}

//------------------------------------------------------------------------------------------------//
