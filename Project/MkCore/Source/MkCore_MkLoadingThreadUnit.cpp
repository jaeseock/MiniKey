
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

		// �ε��� LoadingTargetInfo�� ��� ��
		MkBackgroundLoader::LoadingTargetInfo currTarget;
		if (!MK_BG_LOADER.__GetNextLoadingTarget(currTarget))
			break;

		MkBaseLoadingTarget* loadingTarget = currTarget.target;

		// �����°� �ƴ� target�� ����
		if (loadingTarget->GetDataState() != MkBaseLoadingTarget::eWaiting)
			continue;
		
		// �ε� ����
		loadingTarget->__SetDataState(MkBaseLoadingTarget::eLoading);

		// ���� ���翩�� ����
		MK_CHECK(MkFileManager::CheckAvailable(currTarget.filePath), MkStr(currTarget.filePath) + L"������ �������� ����")
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eError);
			continue;
		}

		// ������ �о����
		MkByteArray byteArray;
		MK_CHECK(MkFileManager::GetFileData(currTarget.filePath, byteArray), MkStr(currTarget.filePath) + L"���� �ε� ����")
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eError);
			continue;
		}

		// �ʱ�ȭ �� ���࿩�� �˻�
		if (m_CurrentState != eRunning)
			break;

		if (loadingTarget->__GetInterrupt())
		{
			loadingTarget->__SetDataState(MkBaseLoadingTarget::eStop);
			continue;
		}

		// �ʱ�ȭ
		MkBaseLoadingTarget::eDataState state =
			(loadingTarget->CustomSetUp(byteArray, currTarget.filePath, currTarget.argument)) ?
			MkBaseLoadingTarget::eComplete : MkBaseLoadingTarget::eError;

		loadingTarget->__SetDataState(state);

		MK_CHECK(state == MkBaseLoadingTarget::eComplete, MkStr(currTarget.filePath) + L"���� �����ͷ� �ʱ�ȭ ����")
			continue;
		
#if (MKDEF_SHOW_BGLOADING_COMPLETE)
		MK_DEV_PANEL.MsgToLog(L"> background loading���� " + MkStr(currTarget.filePath) + L" ���� �ε� �Ϸ�", true);
#endif
	}
}

//------------------------------------------------------------------------------------------------//
