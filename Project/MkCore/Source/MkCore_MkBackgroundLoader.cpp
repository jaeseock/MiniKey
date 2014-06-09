
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBackgroundLoader.h"

#define MKDEF_FILEIO_MSG_TEXT L"file count to loading : "

//------------------------------------------------------------------------------------------------//

bool MkBackgroundLoader::RegisterLoadingTarget
(const MkLoadingTargetPtr& loadingTarget, const MkPathName& filePath, const MkStr& argument)
{
	MK_CHECK(loadingTarget != NULL, L"NULL MkBaseLoadingTarget으로 " + MkStr(filePath) + L" 파일 로딩 시도")
		return false;

	unsigned int queueSize;
	MkWrapInCriticalSection(m_CS)
	{
		LoadingTargetInfo& ti = m_LoadingTargetList.PushBack();
		ti.target = loadingTarget; // +ref
		ti.filePath = filePath;
		ti.argument = argument;

		queueSize = m_LoadingTargetList.GetSize();
	}

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_FILEIO, MKDEF_FILEIO_MSG_TEXT + MkStr(queueSize));
	return true;
}

bool MkBackgroundLoader::__GetNextLoadingTarget(LoadingTargetInfo& buffer)
{
	bool found = false;
	unsigned int queueSize = 0;
	MkWrapInCriticalSection(m_CS)
	{
		while (true)
		{
			if ((!found) && (!m_LoadingTargetList.Empty()))
			{
				LoadingTargetInfo& firstTarget = m_LoadingTargetList[0];

				if (firstTarget.target->__GetInterrupt())
				{
					firstTarget.target->__SetDataState(MkBaseLoadingTarget::eStop);
				}
				else
				{
					buffer = firstTarget; // +ref
					found = true;
				}

				m_LoadingTargetList.PopFront(); // -ref
			}
			else
				break;
		}

		queueSize = m_LoadingTargetList.GetSize();
	}

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_FILEIO, MKDEF_FILEIO_MSG_TEXT + MkStr(queueSize));
	return found;
}

void MkBackgroundLoader::__Clear(void)
{
	MkScopedCriticalSection(m_CS);
	m_LoadingTargetList.Clear();
}

//------------------------------------------------------------------------------------------------//
