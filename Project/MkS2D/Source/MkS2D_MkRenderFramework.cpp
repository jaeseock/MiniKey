
#include "MkS2D_MkResetableResourcePool.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkRenderStateSetter.h"
#include "MkS2D_MkS2DCheatMessage.h"
#include "MkS2D_MkDrawingMonitor.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkRenderFramework.h"

//------------------------------------------------------------------------------------------------//

MkCheatMessage* MkRenderFramework::CreateCheatMessage(void) const
{
	return new MkS2DCheatMessage;
}

bool MkRenderFramework::SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
{
	if (!MkBaseFramework::SetUp(clientWidth, clientHeight, fullScreen, arg))
		return false;

	// ∑ª¥ı∑Ø ΩÃ±€≈Ê ª˝º∫
	m_InstanceDeallocator.Expand(8);

	// 0.
	m_InstanceDeallocator.RegisterInstance(new MkResetableResourcePool());
	
	// 1.
	m_InstanceDeallocator.RegisterInstance(new MkWindowResourceManager());
	
	// 2.
	m_InstanceDeallocator.RegisterInstance(new MkDisplayManager());

	// 3.
	m_InstanceDeallocator.RegisterInstance(new MkFontManager());
	MK_RESETABLE_RESPOOL.RegisterResource(MkFontManager::InstancePtr());

	// 4.
	m_InstanceDeallocator.RegisterInstance(new MkTexturePool());

	// 5.
	m_InstanceDeallocator.RegisterInstance(new MkRenderStateSetter());

	// 6.
	m_InstanceDeallocator.RegisterInstance(new MkDrawingMonitor());

	// 7.
	m_InstanceDeallocator.RegisterInstance(new MkRenderer());

	// ∑ª¥ı∑Ø √ ±‚»≠
	if (!MK_RENDERER.SetUp(&m_MainWindow, clientWidth, clientHeight, fullScreen))
		return false;

	return true;
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
