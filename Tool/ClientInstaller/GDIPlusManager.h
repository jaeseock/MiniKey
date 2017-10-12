#pragma once

#include <windows.h>
#include <gdiplus.h>

//#include "../../Lib/MkCore/Include/MkCore_MkRect.h"
//#include "../../Lib/MkCore/Include/MkCore_MkMap.h"
#include "MkCore_MkRect.h"
#include "MkCore_MkMap.h"


//------------------------------------------------------------------------------------------------//

#define GP_MGR GDIPlusManager::GetInstance()


class MkStr;
class MkPathName;

class GDIPlusManager
{
public:

	bool SetUp(void);

	bool AddImage(unsigned int index, const MkPathName& filePath, const MkInt2& position = MkInt2::Zero);
	bool AddImage(unsigned int index, int resID, const MkInt2& position = MkInt2::Zero);

	void DeleteImage(unsigned int index);

	MkInt2 GetImagePosition(unsigned int index) const;
	MkInt2 GetImageSize(unsigned int index) const;

	void Draw(CWnd* wnd, unsigned int index);
	void Draw(CWnd* wnd, unsigned int index, const MkInt2& newPos, const MkInt2& newSize);

	void Clear(void);

	GDIPlusManager();
	~GDIPlusManager() { Clear(); }

	static GDIPlusManager& GetInstance(void);

public:

	//------------------------------------------------------------------------------------------------//

	class ImageSet
	{
	public:
		bool SetSkin(WORD uniqueID, WORD state, const MkPathName& filePath);
		bool SetSkin(WORD uniqueID, WORD state, int resID);
		void Draw(CWnd* wnd, unsigned int state);
		void Draw(CWnd* wnd, unsigned int state, const MkInt2& newPos, const MkInt2& newSize);
		void Clear(void);
		void Clear(unsigned int state);

		MkInt2 GetImagePosition(unsigned int state) const;
		MkInt2 GetImageSize(unsigned int state) const;
		
		ImageSet() {}
		~ImageSet() { Clear(); }

	protected:
		MkMap<unsigned int, unsigned int> m_Images;
	};

protected:

	//------------------------------------------------------------------------------------------------//

	class _ImageUnit
	{
	public:
		bool SetUp(const MkPathName& filePath, const MkInt2& position);
		bool SetUp(int resID, const MkInt2& position);

		void Draw(Gdiplus::Graphics& g);
		void Draw(Gdiplus::Graphics& g, const MkInt2& newPos, const MkInt2& newSize);

		void Clear(void);

		inline const MkIntRect& GetRect(void) const { return m_Rect; }

		_ImageUnit() { m_Image = NULL; }
		~_ImageUnit() { Clear(); }

	protected:

		Gdiplus::Image* _LoadPNG(int resID);

	protected:

		MkIntRect m_Rect;
		Gdiplus::Image* m_Image;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	ULONG_PTR m_GdiPlusToken;

	MkMap<unsigned int, _ImageUnit> m_Images;
};
