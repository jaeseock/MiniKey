
#include "MkPE_MkRenderToTexture.h"
#include "MkPE_MkRenderTarget.h"


//------------------------------------------------------------------------------------------------//

void MkRenderTarget::SetUp(void)
{
	Clear();
}

bool MkRenderTarget::SetUp(const MkInt2& size, MkRenderToTexture::eTargetFormat format)
{
	MkArray<MkRenderToTexture::eTargetFormat> formats;
	formats.PushBack(format);

	return SetUp(size, formats);
}

bool MkRenderTarget::SetUp(const MkInt2& size, const MkArray<MkRenderToTexture::eTargetFormat>& formats)
{
	Clear();

	m_TargetTexture.Reserve(formats.GetSize());

	MK_INDEXING_LOOP(formats, i)
	{
		MkRenderToTexture* rtt = new MkRenderToTexture();
		if (rtt != NULL)
		{
			if (rtt->SetUp(size, formats[i], i))
			{
				m_TargetTexture.PushBack(rtt);
			}
			else
			{
				delete rtt;
				break;
			}
		}
	}

	bool ok = (m_TargetTexture.GetSize() == m_TargetTexture.GetSize());
	if (ok)
	{
		m_BackgroundColor = D3DCOLOR_ARGB(0, 0, 0, 0);
	}
	else
	{
		Clear();
		m_BackgroundColor = D3DCOLOR_XRGB(0xff, 0, 0); // error
	}
	return ok;
}

bool MkRenderTarget::Resize(const MkInt2& size)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		if (!m_TargetTexture[i]->Resize(size))
			return false;
	}
	return true;
}

void MkRenderTarget::Clear(void)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		delete m_TargetTexture[i];
	}
	m_TargetTexture.Clear();

	m_BackgroundColor = D3DCOLOR_XRGB(0, 0, 0xff);
}

const MkRenderToTexture* MkRenderTarget::GetTargetTexture(unsigned int index) const
{
	return m_TargetTexture.IsValidIndex(index) ? m_TargetTexture[index] : NULL;
}

void MkRenderTarget::UnloadResource(void)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		m_TargetTexture[i]->UnloadResource();
	}
}

void MkRenderTarget::ReloadResource(void)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		m_TargetTexture[i]->ReloadResource();
	}
}

void MkRenderTarget::__Push(void)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		m_TargetTexture[i]->ChangeRenderTarget();
	}
}

void MkRenderTarget::__Pop(void)
{
	MK_INDEXING_LOOP(m_TargetTexture, i)
	{
		m_TargetTexture[i]->RestoreRenderTarget();
	}
}

MkRenderTarget::MkRenderTarget()
{
	m_BackgroundColor = D3DCOLOR_XRGB(0, 0, 0xff);
}

//------------------------------------------------------------------------------------------------//
