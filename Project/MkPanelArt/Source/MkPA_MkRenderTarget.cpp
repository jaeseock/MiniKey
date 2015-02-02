
#include "MkCore_MkColor.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkRenderTarget.h"


//------------------------------------------------------------------------------------------------//

bool MkRenderTarget::SetUp(eTargetType type, unsigned int count, const MkInt2& size, MkRenderToTexture::eFormat format)
{
	Clear();

	m_TargetType = type;

	bool ok = true;
	if (m_TargetType == eTexture)
	{
		unsigned int success = 0;
		m_TargetTexture.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			MkRenderToTexture* rtt = new MkRenderToTexture();
			if (rtt != NULL)
			{
				if (rtt->SetUp(size, format, i))
				{
					m_TargetTexture.PushBack() = rtt;
					++success;
				}
				else
				{
					delete rtt;
					Clear();
					break;
				}
			}
		}

		ok = (success == count);
	}

	if (ok)
	{
		MkInt2 ss = GetScreenSize();
		m_RegionRect.size = MkFloat2(static_cast<float>(ss.x), static_cast<float>(ss.y));
	}

	return ok;
}

void MkRenderTarget::SetBackgroundColor(const MkColor& color)
{
	m_BackgroundColor = static_cast<D3DCOLOR>(color.ConvertToD3DCOLOR());
}

void MkRenderTarget::SetBackgroundColor(D3DCOLOR color)
{
	m_BackgroundColor = color;
}

void MkRenderTarget::Clear(void)
{
	m_TargetType = eBackbuffer;
	m_TargetTexture.Clear();
	m_RegionRect.Clear();
}

MkInt2 MkRenderTarget::GetScreenSize(void) const
{
	switch (m_TargetType)
	{
	case eBackbuffer:
		return MK_DEVICE_MGR.GetCurrentResolution();

	case eTexture:
		{
			if (!m_TargetTexture.Empty())
			{
				return m_TargetTexture[0]->GetSize();
			}
		}
		break;
	}
	return MkInt2(0, 0);
}

const MkRenderToTexture* MkRenderTarget::GetTargetTexture(unsigned int index) const
{
	return m_TargetTexture.IsValidIndex(index) ? m_TargetTexture[index] : NULL;
}

bool MkRenderTarget::__BeginScene(void)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	if ((m_TargetType == eTexture) && (!m_TargetTexture.Empty()))
	{
		MK_INDEXING_LOOP(m_TargetTexture, i)
		{
			m_TargetTexture[i]->ChangeRenderTarget();
		}
	}

	// count render target set
	MK_DRAWING_MONITOR.IncreaseRenderTargetCounter();
	
	return SUCCEEDED(device->BeginScene());
}

void MkRenderTarget::__Clear(DWORD clearFlag)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device != NULL)
	{
		device->Clear(0, NULL, clearFlag, m_BackgroundColor, 1.f, 0);
	}
}

void MkRenderTarget::__EndScene(void)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device != NULL)
	{
		if (SUCCEEDED(device->EndScene()))
		{
			if ((m_TargetType == eTexture) && (!m_TargetTexture.Empty()))
			{
				MK_INDEXING_LOOP(m_TargetTexture, i)
				{
					m_TargetTexture[i]->RestoreRenderTarget();
				}
			}
		}
	}
}

MkRenderTarget::MkRenderTarget()
{
	m_TargetType = eBackbuffer;
	SetBackgroundColor(MkColor::Blue);
}

//------------------------------------------------------------------------------------------------//
