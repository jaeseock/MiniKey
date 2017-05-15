
#include <Windows.h>
#include <urlmon.h>
#include <Wininet.h>

#include "MkCore_MkPathName.h"
#include "MkCore_MkFileDownloader.h"

#pragma comment (lib, "Urlmon.lib")
#pragma comment (lib, "Wininet.lib")


//------------------------------------------------------------------------------------------------//
// download call back
//------------------------------------------------------------------------------------------------//

class _DownloadCallback : public IBindStatusCallback
{
public:

	inline bool IsComplete(void) const { return m_Complete; }

	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText)
	{
		switch (ulStatusCode)
		{
		case BINDSTATUS_ENDDOWNLOADDATA:
			m_Complete = (ulProgress == ulProgressMax);
			break;
		}

		return S_OK; //E_ABORT
	}
 
	// E_NOTIMPL
	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding __RPC_FAR *pib) { return E_NOTIMPL; }
	STDMETHOD(GetPriority)(LONG __RPC_FAR *pnPriority) { return E_NOTIMPL;}
	STDMETHOD(OnLowResource)(DWORD reserved) { return E_NOTIMPL; }
    STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError) { return E_NOTIMPL; }
    STDMETHOD(GetBindInfo)(DWORD __RPC_FAR *grfBINDF, BINDINFO __RPC_FAR *pbindinfo) { return E_NOTIMPL; }
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC __RPC_FAR *pformatetc, STGMEDIUM __RPC_FAR *pstgmed) { return E_NOTIMPL; }
	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown __RPC_FAR *punk) { return E_NOTIMPL; }
	STDMETHOD(QueryInterface)(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) { return E_NOTIMPL; }
	STDMETHOD_(ULONG, AddRef)() { return 0; }
	STDMETHOD_(ULONG, Release)() { return 0; }

	_DownloadCallback() : IBindStatusCallback() { m_Complete = false; }
	virtual ~_DownloadCallback() {}

protected:

	bool m_Complete;
};

//------------------------------------------------------------------------------------------------//

MkFileDownloader::eResult MkFileDownloader::Start(const MkStr& fileUrl, const MkPathName& destFilePath, int retryCount)
{
	if (destFilePath.IsDirectoryPath())
		return eDestFilePathIsDirectory;

	MkPathName localFilePath;
	localFilePath.ConvertToRootBasisAbsolutePath(destFilePath);
	localFilePath.GetPath().MakeDirectoryPath();

	int tryCount = GetMax<int>(1, retryCount + 1);
	BOOL rlt = ::DeleteUrlCacheEntry(fileUrl.GetPtr());

	_DownloadCallback callback;
	for (int i=0; i<tryCount; ++i)
	{
		switch (::URLDownloadToFile(NULL, fileUrl.GetPtr(), localFilePath.GetPtr(), 0, &callback))
		{
		case S_OK:
			{
				if (callback.IsComplete())
					return eSuccess;
			}
			break;

		case INET_E_OBJECT_NOT_FOUND:
			return eInvalidURL;

		case E_OUTOFMEMORY:
			return eOutOfMemory;

		default:
			break;
		}

		::Sleep(1000); // 1초 뒤 재도전
	}

	localFilePath.DeleteCurrentFile();
	return eNetworkError;
}

//------------------------------------------------------------------------------------------------//