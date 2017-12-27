
#include <Windows.h>
#include <urlmon.h>
#include <Wininet.h>

#include "MkCore_MkBaseThreadUnit.h"
#include "MkCore_MkThreadManager.h"
#include "MkCore_MkFileDownloader.h"

#pragma comment (lib, "Urlmon.lib")
#pragma comment (lib, "Wininet.lib")


//------------------------------------------------------------------------------------------------//

bool MkFileDownInfo::SetUp(const MkStr& fileURL, const MkPathName& destFilePath)
{
	if (destFilePath.IsDirectoryPath())
		return false;

	m_FileURL = fileURL;

	m_DestLocalPath.ConvertToRootBasisAbsolutePath(destFilePath);
	m_DestLocalPath.GetPath().MakeDirectoryPath();

	__SetDownState(eDS_Wait);
	__SetFileState(0, 0);
	return true;
}

MkFileDownInfo::eDownState MkFileDownInfo::GetDownState(void)
{
	return m_DownState;
}

unsigned int MkFileDownInfo::GetFileSize(void) { return m_FileSize; }
unsigned int MkFileDownInfo::GetDownSize(void) { return m_DownSize; }

float MkFileDownInfo::GetProgress(void)
{
	unsigned int fs = m_FileSize;
	unsigned int ds = m_DownSize;
	return ((fs == 0) || (ds == 0)) ? 0.f : (static_cast<float>(ds) / static_cast<float>(fs));
}

MkFileDownInfo::MkFileDownInfo()
{
	__SetDownState(eDS_Wait);
	__SetFileState(0, 0);
}

void MkFileDownInfo::__GetTargetInfo(MkStr& fileURL, MkPathName& destLocalPath)
{
	fileURL = m_FileURL;
	destLocalPath = m_DestLocalPath;
}

void MkFileDownInfo::__SetDownState(eDownState downState)
{
	m_DownState = downState;
}

void MkFileDownInfo::__SetFileState(unsigned int fileSize, unsigned int downSize)
{
	m_FileSize = fileSize;
	m_DownSize = downSize;
}

//------------------------------------------------------------------------------------------------//
// download call back
//------------------------------------------------------------------------------------------------//

class __DownloadCallback : public IBindStatusCallback
{
public:

	inline void SetUp(MkFileDownInfo* info)
	{
		m_FileDownInfo = info;
		if (m_FileDownInfo != NULL)
		{
			info->__SetDownState(MkFileDownInfo::eDS_Downloading);
		}

		m_Complete = false;
	}

	inline bool IsComplete(void) { return m_Complete; }

	inline void SetInterrupt(bool enable) { m_Interrupt = enable; }
	inline bool GetInterrupt(void) { return m_Interrupt; }

	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText)
	{
		switch (ulStatusCode)
		{
		case BINDSTATUS_ENDDOWNLOADDATA:
			m_Complete = (ulProgress == ulProgressMax);
			break;
		}

		if (m_FileDownInfo != NULL)
		{
			m_FileDownInfo->__SetFileState(ulProgressMax, ulProgress);
		}

		return (m_Interrupt) ? E_ABORT : S_OK;
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

	__DownloadCallback() : IBindStatusCallback()
	{
		m_FileDownInfo = NULL;
		m_Complete = false;
		m_Interrupt = false;
	}

	virtual ~__DownloadCallback() {}

protected:

	MkFileDownInfo* m_FileDownInfo; // producer == consumer, MkDownloadThreadUnit
	bool m_Complete; // producer == consumer, MkDownloadThreadUnit
	MkLockable<bool> m_Interrupt; // producer:other thread, consumer:MkDownloadThreadUnit(this)
};

static __DownloadCallback gDownloadCallBack;

//------------------------------------------------------------------------------------------------//
// download thread
//------------------------------------------------------------------------------------------------//

class MkDownloadThreadUnit : public MkBaseThreadUnit
{
public:

	virtual void Update(const MkTimeState& timeState)
	{
		while (true)
		{
			if (m_CurrentState != eRunning)
				break;

			MkFileDownInfo* infoPtr = MK_FILE_DOWNLOADER.__GetNextFileInfo();
			if (infoPtr == NULL)
				break;

			MkStr fileURL;
			MkPathName destLocalPath;
			infoPtr->__GetTargetInfo(fileURL, destLocalPath);

			::DeleteUrlCacheEntry(fileURL.GetPtr());

			gDownloadCallBack.SetUp(infoPtr);

			int tryCnt = 6;
			while (tryCnt != 0)
			{
				if (_DownloadTarget(fileURL, destLocalPath, infoPtr))
					break;

				::Sleep(1000); // 1초 뒤 재도전
				--tryCnt;
			}

			if (tryCnt == 0)
			{
				infoPtr->__SetDownState(MkFileDownInfo::eDS_Failed);
			}
		}
	}

	MkDownloadThreadUnit(const MkHashStr& threadName) : MkBaseThreadUnit(threadName) {}
	virtual ~MkDownloadThreadUnit() {}

protected:

	bool _DownloadTarget(const MkStr& fileURL, const MkPathName& destLocalPath, MkFileDownInfo* infoPtr) // 반환값은 종료 여부
	{
		switch (::URLDownloadToFile(NULL, fileURL.GetPtr(), destLocalPath.GetPtr(), 0, &gDownloadCallBack))
		{
		case S_OK:
			{
				if (gDownloadCallBack.IsComplete())
				{
					infoPtr->__SetDownState(MkFileDownInfo::eDS_Complete);
					return true;
				}
			}
			break;

		case E_ABORT:
			infoPtr->__SetDownState(MkFileDownInfo::eDS_Abort);
			return true;

		case INET_E_OBJECT_NOT_FOUND:
			infoPtr->__SetDownState(MkFileDownInfo::eDS_InvalidURL);
			return true;

		case E_OUTOFMEMORY:
			infoPtr->__SetDownState(MkFileDownInfo::eDS_OutOfMemory);
			return true;
		}
		return false;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkFileDownloader::DownloadFileRightNow(const MkStr& fileUrl, const MkPathName& destFilePath)
{
	if (destFilePath.IsDirectoryPath())
		return false;

	MkPathName localFilePath;
	localFilePath.ConvertToRootBasisAbsolutePath(destFilePath);
	localFilePath.GetPath().MakeDirectoryPath();

	::DeleteUrlCacheEntry(fileUrl.GetPtr());

	__DownloadCallback callback;
	for (int i=0; i<6; ++i)
	{
		switch (::URLDownloadToFile(NULL, fileUrl.GetPtr(), localFilePath.GetPtr(), 0, &callback))
		{
		case S_OK:
			{
				if (callback.IsComplete())
					return true;
			}
			break;

		case INET_E_OBJECT_NOT_FOUND:
		case E_OUTOFMEMORY:
			return false;
		}

		::Sleep(1000); // 1초 뒤 재도전
	}
	return false;
}

//------------------------------------------------------------------------------------------------//

MkFileDownInfo* MkFileDownloader::DownloadFile(const MkStr& fileURL, const MkPathName& destFilePath)
{
	MkFileDownInfo* fileDownInfo = new MkFileDownInfo;
	if (fileDownInfo != NULL)
	{
		if (fileDownInfo->SetUp(fileURL, destFilePath))
		{
			_OpenDownloadThread();
			if (m_ThreadMgr != NULL)
			{
				MkWrapInCriticalSection(m_CS)
				{
					m_FileDownInfoList.PushBack(fileDownInfo);
				}
				return fileDownInfo;
			}
		}
		delete fileDownInfo;
	}
	return NULL;
}

void MkFileDownloader::StopDownload(void)
{
	gDownloadCallBack.SetInterrupt(true);

	MkScopedCriticalSection(m_CS);
	MK_INDEXING_LOOP(m_FileDownInfoList, i)
	{
		m_FileDownInfoList[i]->__SetDownState(MkFileDownInfo::eDS_Abort);
	}
	m_FileDownInfoList.Clear();
}

MkFileDownloader& MkFileDownloader::Instance(void)
{
	static MkFileDownloader instance;
	return instance;
}

MkFileDownloader::MkFileDownloader()
{
	m_ThreadMgr = NULL;
}

MkFileDownInfo* MkFileDownloader::__GetNextFileInfo(void)
{
	MkFileDownInfo* infoPtr = NULL;

	MkWrapInCriticalSection(m_CS)
	{
		if (!m_FileDownInfoList.Empty())
		{
			infoPtr = m_FileDownInfoList[0];
			m_FileDownInfoList.PopFront();
		}
	}

	return infoPtr;
}

void MkFileDownloader::__CloseDownloadThread(void)
{
	if (m_ThreadMgr != NULL)
	{
		m_ThreadMgr->StopAll();
		MK_DELETE(m_ThreadMgr);
	}
}

void MkFileDownloader::_OpenDownloadThread(void)
{
	if (m_ThreadMgr == NULL)
	{
		do
		{
			m_ThreadMgr = new MkThreadManager;
			if (m_ThreadMgr == NULL)
				break;

			MkDownloadThreadUnit* threadUnit = new MkDownloadThreadUnit(L"MkDownloadThreadUnit");
			if (threadUnit == NULL)
				break;

			if (!m_ThreadMgr->RegisterThreadUnit(threadUnit))
				break;

			m_ThreadMgr->WakeUpAll();

			if (!m_ThreadMgr->WaitTillAllThreadsAreRunning())
				break;

			return;
		}
		while (false);

		__CloseDownloadThread();
	}
}
