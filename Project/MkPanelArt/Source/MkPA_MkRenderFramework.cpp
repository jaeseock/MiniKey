
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkResetableResourcePool.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkCursorManager.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkShaderEffectPool.h"
//#include "MkS2D_MkS2DCheatMessage.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkRenderer.h"
#include "MkPA_MkRenderFramework.h"

//------------------------------------------------------------------------------------------------//

//MkCheatMessage* MkRenderFramework::CreateCheatMessage(void) const
//{
//	return new MkS2DCheatMessage;
//}

bool MkRenderFramework::SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
{
	if (!MkBaseFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine))
		return false;

	// ∑ª¥ı∑Ø ΩÃ±€≈Ê ª˝º∫
	
	// 0.
	m_InstanceDeallocator.RegisterInstance(new MkHiddenEditBox());

	// 1.
	m_InstanceDeallocator.RegisterInstance(new MkResetableResourcePool());
	
	// 2.
	m_InstanceDeallocator.RegisterInstance(new MkStaticResourceContainer());

	// 3.
	m_InstanceDeallocator.RegisterInstance(new MkDeviceManager());

	// 4.
	m_InstanceDeallocator.RegisterInstance(new MkFontManager());
	MK_RESETABLE_RESPOOL.RegisterResource(MkFontManager::InstancePtr());

	// 5.
	m_InstanceDeallocator.RegisterInstance(new MkBitmapPool());

	// 6.
	m_InstanceDeallocator.RegisterInstance(new MkCursorManager());
	MK_RESETABLE_RESPOOL.RegisterResource(MkCursorManager::InstancePtr());

	// 7.
	m_InstanceDeallocator.RegisterInstance(new MkRenderStateSetter());

	// 8.
	m_InstanceDeallocator.RegisterInstance(new MkShaderEffectPool());
	MK_RESETABLE_RESPOOL.RegisterResource(MkShaderEffectPool::InstancePtr());

	// 9.
	m_InstanceDeallocator.RegisterInstance(new MkDrawingMonitor());

	// 10.
	m_InstanceDeallocator.RegisterInstance(new MkRenderer());

	// ∑ª¥ı∑Ø √ ±‚»≠
	if (!MK_RENDERER.SetUp(&m_MainWindow, clientWidth, clientHeight, fullScreen))
		return false;

	return true;
}

void MkRenderFramework::ConsumeSetCursorMsg(void)
{
	MK_CURSOR_MGR.__ConsumeSetCursorMsg();
}

void MkRenderFramework::Update(void)
{
	MK_RENDERER.Update();
}

void MkRenderFramework::Clear(void)
{
	MK_RENDERER.Clear();
}

//------------------------------------------------------------------------------------------------//
