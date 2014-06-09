
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBackgroundLoader.h"
#include "MkCore_MkLoadingThreadUnit.h"


//------------------------------------------------------------------------------------------------//

void MkLoadingThreadUnit::Update(const MkTimeState& timeState)
{
	if ((MkBackgroundLoader::InstancePtr() == NULL) || (MkFileManager::InstancePtr() == NULL))
		return;

	while (true)
	{
		if (m_CurrentState != eRunning)
			break;

		// 로딩할 LoadingTargetInfo를 얻어 옴
		MkBackgroundLoader::LoadingTargetInfo currTarget;
		if (!MK_BG_LOADER.__GetNextLoadingTarget(currTarget))
			break;

		MkBaseLoadingTarget* loadingTarget = currTarget.target;

		// 대기상태가 아닌 target은 무시
		if (loadingTarget->GetDataState() != MkBaseLoadingTarget::eWaiting)
			continue;
		
		// 로딩 시작
		loadingTarget->__SetDataState(MkBaseLoadingTarget::eLoading);

		// 파일 존재여부 점검
		MK_CHECK(MkFileManager::CheckAvailable(currTarget.filePath), MkStr(currTarget.filePath) + L"파일이 존재하지 않음")
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eError);
			continue;
		}

		// 데이터 읽어들임
		MkByteArray byteArray;
		MK_CHECK(MkFileManager::GetFileData(currTarget.filePath, byteArray), MkStr(currTarget.filePath) + L"파일 로딩 실패")
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eError);
			continue;
		}

		// 초기화 전 진행여부 검사
		if (m_CurrentState != eRunning)
			break;

		if (loadingTarget->__GetInterrupt())
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eStop);
			continue;
		}

		// 초기화
		MkBaseLoadingTarget::eDataState state =
			(loadingTarget->CustomSetUp(byteArray, currTarget.filePath, currTarget.argument)) ?
			MkBaseLoadingTarget::eComplete : MkBaseLoadingTarget::eError;

		loadingTarget->__SetDataState(state);

		MK_CHECK(state == MkBaseLoadingTarget::eComplete, MkStr(currTarget.filePath) + L"파일 데이터로 초기화 실패")
			continue;
		
#if (MKDEF_SHOW_BGLOADING_COMPLETE)
		MK_DEV_PANEL.MsgToLog(L"> background loading으로 " + MkStr(currTarget.filePath) + L" 파일 로딩 완료", true);
#endif
	}
}

//------------------------------------------------------------------------------------------------//
