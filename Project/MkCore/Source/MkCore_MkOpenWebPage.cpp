
#include <Windows.h>
#include "comdef.h"
#include "exdisp.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkOpenWebPage.h"

//---------------------------------------------------------------------------//

void MkOpenWebPage::Open(const MkStr& pageUrl, int posX, int posY, int width, int height)
{
	if (pageUrl.Empty())
		return;

	HRESULT hr;
	IWebBrowser2 *pWebBrowser = NULL;
	VARIANT vtHeader2={0};
	VARIANT vtTarget2={0};
	VARIANT vtEmpty2={0};

	vtHeader2.vt = VT_BSTR;
	vtHeader2.bstrVal = SysAllocString(L"Content-Type: application/x-www-form-urlencoded\r\n");

	vtTarget2.vt = VT_BSTR;
	vtTarget2.bstrVal = SysAllocString(L"_top");

	VariantInit(&vtEmpty2);

	CoInitialize(NULL);
	CoCreateInstance
		(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER,IID_IWebBrowser2, (void**)&pWebBrowser);

	pWebBrowser->put_Width((long)width);
	pWebBrowser->put_Height((long)height);
	pWebBrowser->put_Left((long)posX);
	pWebBrowser->put_Top((long)posY);
	pWebBrowser->put_MenuBar(VARIANT_FALSE);
	pWebBrowser->put_ToolBar(VARIANT_FALSE);
	pWebBrowser->put_AddressBar(VARIANT_TRUE);
	pWebBrowser->put_StatusBar(VARIANT_FALSE);
	pWebBrowser->put_Visible(VARIANT_TRUE);
	HWND exh;
	pWebBrowser->get_HWND((long*)&exh);
	//SetForegroundWindow(exh);

	BSTR cBuf = SysAllocString(pageUrl.GetPtr());
	hr = pWebBrowser->Navigate(cBuf, &vtEmpty2, &vtTarget2, &vtEmpty2, &vtHeader2);
	if (SUCCEEDED(hr))
	{
	}

	SysFreeString(cBuf);
	SysFreeString(vtHeader2.bstrVal);
	SysFreeString(vtTarget2.bstrVal);
	pWebBrowser->Release();

	CoUninitialize();
}

//---------------------------------------------------------------------------//

