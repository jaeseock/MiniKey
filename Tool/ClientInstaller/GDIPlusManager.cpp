
#include "stdafx.h"
#include <assert.h>
//#include "../../Lib/MkCore/Include/MkCore_MkPathName.h"
#include "MkCore_MkPathName.h"
#include "GDIPlusManager.h"

#pragma comment(lib, "gdiplus.lib")


//------------------------------------------------------------------------------------------------//

bool GDIPlusManager::SetUp(void)
{
	Clear();

	Gdiplus::GdiplusStartupInput gpsi;
	bool ok = (Gdiplus::GdiplusStartup(&m_GdiPlusToken, &gpsi, NULL) == Gdiplus::Ok);
	assert(ok && (m_GdiPlusToken != NULL));
	return ok;
}

bool GDIPlusManager::AddImage(unsigned int index, const MkPathName& filePath, const MkInt2& position)
{
	if (m_Images.Exist(index))
	{
		m_Images[index].Clear();
	}
	else
	{
		m_Images.Create(index);
	}

	_ImageUnit& unit = m_Images[index];
	bool ok = unit.SetUp(filePath, position);
	if (!ok)
	{
		m_Images.Erase(index);
	}
	return ok;
}

bool GDIPlusManager::AddImage(unsigned int index, int resID, const MkInt2& position)
{
	if (m_Images.Exist(index))
	{
		m_Images[index].Clear();
	}
	else
	{
		m_Images.Create(index);
	}

	_ImageUnit& unit = m_Images[index];
	bool ok = unit.SetUp(resID, position);
	if (!ok)
	{
		m_Images.Erase(index);
	}
	return ok;
}

void GDIPlusManager::DeleteImage(unsigned int index)
{
	if (m_Images.Exist(index))
	{
		m_Images[index].Clear();
		m_Images.Erase(index);
	}
}

MkInt2 GDIPlusManager::GetImagePosition(unsigned int index) const
{
	return m_Images.Exist(index) ? m_Images[index].GetRect().position : MkInt2::Zero;
}

MkInt2 GDIPlusManager::GetImageSize(unsigned int index) const
{
	return m_Images.Exist(index) ? m_Images[index].GetRect().size : MkInt2::Zero;
}

void GDIPlusManager::Draw(CWnd* wnd, unsigned int index)
{
	if (m_Images.Exist(index))
	{
		CWindowDC dc(wnd);
		Gdiplus::Graphics g(dc.GetSafeHdc());
		m_Images[index].Draw(g);
	}
}

void GDIPlusManager::Draw(CWnd* wnd, unsigned int index, const MkInt2& newPos, const MkInt2& newSize)
{
	if (m_Images.Exist(index))
	{
		CWindowDC dc(wnd);
		Gdiplus::Graphics g(dc.GetSafeHdc());
		m_Images[index].Draw(g, newPos, newSize);
	}
}

void GDIPlusManager::Clear(void)
{
	m_Images.Clear();
	
	if (m_GdiPlusToken != NULL)
	{
		Gdiplus::GdiplusShutdown(m_GdiPlusToken);
		m_GdiPlusToken = NULL;
	}
}

GDIPlusManager& GDIPlusManager::GetInstance(void)
{
	static GDIPlusManager mgr;
	return mgr;
}

GDIPlusManager::GDIPlusManager()
{
	m_GdiPlusToken = NULL;
}

//------------------------------------------------------------------------------------------------//

bool GDIPlusManager::ImageSet::SetSkin(WORD uniqueID, WORD state, const MkPathName& filePath)
{
	unsigned int uState = static_cast<unsigned int>(state);
	Clear(uState);

	unsigned int id = (static_cast<unsigned int>(uniqueID) << 16) + uState;
	bool ok = GP_MGR.AddImage(id, filePath);
	if (ok)
	{
		m_Images.Create(uState, id);
	}
	return ok;
}

bool GDIPlusManager::ImageSet::SetSkin(WORD uniqueID, WORD state, int resID)
{
	unsigned int uState = static_cast<unsigned int>(state);
	Clear(uState);

	unsigned int id = (static_cast<unsigned int>(uniqueID) << 16) + uState;
	bool ok = GP_MGR.AddImage(id, resID);
	if (ok)
	{
		m_Images.Create(uState, id);
	}
	return ok;
}

void GDIPlusManager::ImageSet::Draw(CWnd* wnd, unsigned int state)
{
	if ((wnd != NULL) && m_Images.Exist(state))
	{
		GP_MGR.Draw(wnd, m_Images[state]);
	}
}

void GDIPlusManager::ImageSet::Draw(CWnd* wnd, unsigned int state, const MkInt2& newPos, const MkInt2& newSize)
{
	if ((wnd != NULL) && m_Images.Exist(state))
	{
		GP_MGR.Draw(wnd, m_Images[state], newPos, newSize);
	}
}

void GDIPlusManager::ImageSet::Clear(void)
{
	MkMapLooper<unsigned int, unsigned int> looper(m_Images);
	MK_STL_LOOP(looper)
	{
		GP_MGR.DeleteImage(looper.GetCurrentField());
	}
	m_Images.Clear();
}

void GDIPlusManager::ImageSet::Clear(unsigned int state)
{
	if (m_Images.Exist(state))
	{
		GP_MGR.DeleteImage(m_Images[state]);
		m_Images.Erase(state);
	}
}

MkInt2 GDIPlusManager::ImageSet::GetImagePosition(unsigned int state) const
{
	return m_Images.Exist(state) ? GP_MGR.GetImagePosition(m_Images[state]) : MkInt2::Zero;
}

MkInt2 GDIPlusManager::ImageSet::GetImageSize(unsigned int state) const
{
	return m_Images.Exist(state) ? GP_MGR.GetImageSize(m_Images[state]) : MkInt2::Zero;
}

//------------------------------------------------------------------------------------------------//

bool GDIPlusManager::_ImageUnit::SetUp(const MkPathName& filePath, const MkInt2& position)
{
	Clear();

	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);

	do
	{
		m_Image = Gdiplus::Image::FromFile(fullPath.GetPtr());
		if (m_Image == NULL)
			break;

		int imgWidth = static_cast<int>(m_Image->GetWidth());
		int imgHeight = static_cast<int>(m_Image->GetHeight());
		if ((imgWidth == 0) || (imgHeight == 0))
			break;

		m_Rect.position = position;
		m_Rect.size = MkInt2(imgWidth, imgHeight);
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool GDIPlusManager::_ImageUnit::SetUp(int resID, const MkInt2& position)
{
	Clear();

	do
	{
		m_Image = _LoadPNG(resID);
		if (m_Image == NULL)
			break;

		int imgWidth = static_cast<int>(m_Image->GetWidth());
		int imgHeight = static_cast<int>(m_Image->GetHeight());
		if ((imgWidth == 0) || (imgHeight == 0))
			break;

		m_Rect.position = position;
		m_Rect.size = MkInt2(imgWidth, imgHeight);
		return true;
	}
	while (false);

	Clear();
	return false;
}

void GDIPlusManager::_ImageUnit::Draw(Gdiplus::Graphics& g)
{
	if (m_Image != NULL)
	{
		g.DrawImage(m_Image, m_Rect.position.x, m_Rect.position.y, m_Rect.size.x, m_Rect.size.y);
	}
}

void GDIPlusManager::_ImageUnit::Draw(Gdiplus::Graphics& g, const MkInt2& newPos, const MkInt2& newSize)
{
	if (m_Image != NULL)
	{
		Gdiplus::Rect rect(newPos.x, newPos.y, newSize.x, newSize.y);
		g.DrawImage(m_Image, rect, newPos.x, newPos.y, newSize.x, newSize.y, Gdiplus::UnitPixel, NULL, NULL, NULL);
	}
}

void GDIPlusManager::_ImageUnit::Clear(void)
{
	MK_DELETE(m_Image);
	m_Rect.Clear();
}

Gdiplus::Image* GDIPlusManager::_ImageUnit::_LoadPNG(int resID)
{
	Gdiplus::Image* image = NULL;
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(resID), L"PNG");
	if (hRes != NULL)
	{
		IStream* stream = NULL;
		if (::CreateStreamOnHGlobal(NULL, TRUE, &stream) == S_OK)
		{
			void* resData = ::LockResource(::LoadResource(hInstance, hRes));
			DWORD resSize = ::SizeofResource(hInstance, hRes);
			DWORD rw = 0;
			stream->Write(resData, resSize, &rw);
			image = Gdiplus::Image::FromStream(stream);
			stream->Release();
		}
	}

	return image;
}

//------------------------------------------------------------------------------------------------//