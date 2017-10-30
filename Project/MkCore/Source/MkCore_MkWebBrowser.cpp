
#include <Windows.h>
#include <Wininet.h>
#include "comdef.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkWebBrowser.h"

//---------------------------------------------------------------------------//

bool MkWebBrowser::Open(HWND parent, const MkStr& pageUrl, int posX, int posY, int width, int height, bool show)
{
	if ((m_Browser != NULL) || pageUrl.Empty())
		return false;

	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&m_Browser);
	if (SUCCEEDED(hr) && (m_Browser != NULL))
	{
		m_Browser->put_Width((long)width);
		m_Browser->put_Height((long)height);
		m_Browser->put_Left((long)posX);
		m_Browser->put_Top((long)posY);
		m_Browser->put_MenuBar(VARIANT_FALSE);
		m_Browser->put_ToolBar(VARIANT_FALSE);
		m_Browser->put_AddressBar(VARIANT_FALSE);
		m_Browser->put_StatusBar(VARIANT_FALSE);

		BSTR cBuf = SysAllocString(pageUrl.GetPtr());

		VARIANT vtFlag2 = {0};
		vtFlag2.vt = VT_I4;
		vtFlag2.lVal = navNoHistory | navNoReadFromCache | navNoWriteToCache;

		VARIANT vtTarget2 = {0};
		vtTarget2.vt = VT_BSTR;
		vtTarget2.bstrVal = SysAllocString(L"_top");

		VARIANT vtEmpty2 = {0};
		VariantInit(&vtEmpty2);

		VARIANT vtHeader2 = {0};
		vtHeader2.vt = VT_BSTR;
		vtHeader2.bstrVal = SysAllocString(L"Content-Type: application/x-www-form-urlencoded\r\n");

		hr = m_Browser->Navigate(cBuf, &vtFlag2, &vtTarget2, &vtEmpty2, &vtHeader2);

		SysFreeString(cBuf);
		SysFreeString(vtHeader2.bstrVal);
		SysFreeString(vtTarget2.bstrVal);

		if (SUCCEEDED(hr))
		{
			m_Browser->put_Visible((show) ? VARIANT_TRUE : VARIANT_FALSE);
			m_Browser->get_HWND((long*)&m_hWnd);

			if (parent != NULL)
			{
				m_StyleBackup = static_cast<DWORD>(::GetWindowLongPtr(m_hWnd, GWL_STYLE));
				::SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
				::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
				::SetParent(m_hWnd, parent);
			}

			return true;
		}
		else
		{
			Close();
		}
	}
	return false;
}

void MkWebBrowser::Close(void)
{
	if (m_Browser != NULL)
	{
		HWND parent = ::GetParent(m_hWnd);
		if (parent != NULL)
		{
			::ShowWindow(m_hWnd, SW_HIDE);
			::SetParent(m_hWnd, NULL);
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, m_StyleBackup);
			::SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
		}

		if (m_Browser->Quit() == S_OK)
		{
			m_Browser->Release();

			CoUninitialize();

			m_Browser = NULL;
			m_hWnd = NULL;
		}
	}
}

MkWebBrowser::MkWebBrowser()
{
	m_Browser = NULL;
	m_hWnd = NULL;
}

HWND MkWebBrowser::Open(const MkStr& pageUrl, int posX, int posY, int width, int height)
{
	if (pageUrl.Empty())
		return NULL;

	HRESULT hr;
	IWebBrowser2* pWebBrowser = NULL;
	VARIANT vtHeader2 = {0};
	VARIANT vtTarget2 = {0};
	VARIANT vtEmpty2 = {0};

	vtHeader2.vt = VT_BSTR;
	vtHeader2.bstrVal = SysAllocString(L"Content-Type: application/x-www-form-urlencoded\r\n");

	vtTarget2.vt = VT_BSTR;
	vtTarget2.bstrVal = SysAllocString(L"_top");

	VariantInit(&vtEmpty2);

	CoInitialize(NULL);
	CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser);

	pWebBrowser->put_Width((long)width);
	pWebBrowser->put_Height((long)height);
	pWebBrowser->put_Left((long)posX);
	pWebBrowser->put_Top((long)posY);
	pWebBrowser->put_MenuBar(VARIANT_TRUE);
	pWebBrowser->put_ToolBar(VARIANT_TRUE);
	pWebBrowser->put_AddressBar(VARIANT_TRUE);
	pWebBrowser->put_StatusBar(VARIANT_TRUE);

	BSTR cBuf = SysAllocString(pageUrl.GetPtr());
	hr = pWebBrowser->Navigate(cBuf, &vtEmpty2, &vtTarget2, &vtEmpty2, &vtHeader2);

	SysFreeString(cBuf);
	SysFreeString(vtHeader2.bstrVal);
	SysFreeString(vtTarget2.bstrVal);

	HWND exh = NULL;
	if (SUCCEEDED(hr))
	{
		pWebBrowser->put_Visible(VARIANT_TRUE);
		pWebBrowser->get_HWND((long*)&exh);
	}
	else
	{
		pWebBrowser->Quit();
	}

	pWebBrowser->Release();

	CoUninitialize();
	return exh;
}

//---------------------------------------------------------------------------//
