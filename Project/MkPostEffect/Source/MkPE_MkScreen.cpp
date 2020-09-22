
#include "MkPE_MkScreen.h"


//------------------------------------------------------------------------------------------------//

void MkScreen::SetUp(void)
{
	m_RenderTarget.SetUp();
}

bool MkScreen::SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format, bool fixedSize)
{
	m_FixedSize = fixedSize;
	return m_RenderTarget.SetUp(size, format);
}

bool MkScreen::SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats, bool fixedSize)
{
	m_FixedSize = fixedSize;
	return m_RenderTarget.SetUp(size, formats);
}

bool MkScreen::Resize(const MkInt2& size)
{
	if (m_FixedSize)
	{
		const MkRenderToTexture* rtt = m_RenderTarget.GetTargetTexture(0);
		if (rtt == NULL)
			return false;

		MkInt2 originSize = rtt->GetSize();
		return m_RenderTarget.Resize(originSize);

	}
	return m_RenderTarget.Resize(size);
}

MkDrawCall* MkScreen::CreateDrawCall(int index)
{
	MkDrawCall* drawCall = NULL;
	if (m_DrawQueue.Exist(index))
	{
		drawCall = &m_DrawQueue[index];
		drawCall->Clear();
	}
	else
	{
		drawCall = &m_DrawQueue.Create(index);
	}
	return drawCall;
}

MkDrawCall* MkScreen::GetDrawCall(int index)
{
	return m_DrawQueue.Exist(index) ? &m_DrawQueue[index] : NULL;
}

MkRenderToTexture* MkScreen::GetTargetTexture(unsigned int index) const
{
	const MkRenderToTexture* texture = m_RenderTarget.GetTargetTexture(index);
	return (texture == NULL) ? NULL : const_cast<MkRenderToTexture*>(texture);
}

void MkScreen::Clear(void)
{
	m_RenderTarget.Clear();
	m_DrawQueue.Clear();
}

void MkScreen::UnloadResource(void)
{
	m_RenderTarget.UnloadResource();
}

void MkScreen::ReloadResource(void)
{
	m_RenderTarget.ReloadResource();
}

MkScreen::MkScreen()
{
	m_Enable = true;
	m_ClearLastRenderTarget = false;
	m_FixedSize = false;
}

void MkScreen::__Draw(LPDIRECT3DDEVICE9 device, const MkFloat2& screenSize)
{
	if (m_Enable && (device != NULL))
	{
		m_RenderTarget.__Push();
		
		DWORD rtClearFlag = D3DCLEAR_ZBUFFER; // D3DCLEAR_STENCIL는 사용하지 않음
		if (m_ClearLastRenderTarget)
		{
			rtClearFlag |= D3DCLEAR_TARGET;
		}
		device->Clear(0, NULL, rtClearFlag, m_RenderTarget.GetBackgroundColor(), 1.f, 0);

		MkMapLooper<int, MkDrawCall> looper(m_DrawQueue);
		MK_STL_LOOP(looper)
		{
			looper.GetCurrentField().__Draw(device, screenSize);
		}
		
		m_RenderTarget.__Pop();
	}
}

//------------------------------------------------------------------------------------------------//
