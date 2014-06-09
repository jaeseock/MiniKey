
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkPageManager.h"


//------------------------------------------------------------------------------------------------//

bool MkPageManager::SetUp(MkBasePage* rootPage)
{
	MK_CHECK(rootPage != NULL, L"NULL 루트페이지 등록 시도")
		return false;

	MK_CHECK(!rootPage->GetPageName().Empty(), L"루트페이지 이름이 없음")
		return false;

	m_PageTree.SetUp(rootPage->GetPageName(), rootPage);
	return true;
}

bool MkPageManager::RegisterChildPage(const MkHashStr& parentPage, MkBasePage* childPage)
{
	MK_CHECK(childPage != NULL, L"NULL 자식페이지 등록 시도")
		return false;

	MK_CHECK(!parentPage.Empty(), L"부모페이지 이름이 없음")
		return false;

	MK_CHECK(!childPage->GetPageName().Empty(), L"자식페이지 이름이 없음")
		return false;

	MK_CHECK(m_PageTree.Exist(parentPage), L"부모페이지가 존재하지 않음")
		return false;
	
	return m_PageTree.AttachChildNode(parentPage, childPage->GetPageName(), childPage);
}

void MkPageManager::SetPageFlowTable(const MkHashStr& pageIn, const MkHashStr& condition, const MkHashStr& pageOut)
{
	m_StateFSM.SetTable(pageIn, condition, pageOut);
}

bool MkPageManager::SetMoveMessage(const MkHashStr& moveMessage)
{
	// moveMessage로 인한 페이지 변화가 있어야지만 적용
	bool ok = (m_StateFSM.Proceed(moveMessage, false) == MkFiniteStateMachinePattern<MkHashStr, MkHashStr>::eStateChanged);
	if (ok)
	{
		m_MoveMessage = moveMessage;
	}
	return ok;
}

bool MkPageManager::ChangePageDirectly(const MkHashStr& pageName)
{
	MK_CHECK(IsLeaf(pageName), MkStr(pageName) + L" 페이지는 leaf가 아니어서 이동 불가")
		return false;
	
	// pageName으로 인한 페이지 변화가 있어야지만 적용
	bool ok = ((pageName != GetCurrentPageName()) && (m_PageTree.Exist(pageName)));
	if (ok)
	{
		m_ChangeMessage = pageName;
	}
	return ok;
}

void MkPageManager::__Update(void)
{
	// move message 명령이 있는 경우 실행
	if (!m_MoveMessage.Empty())
	{
		m_StateFSM.Proceed(m_MoveMessage);
		m_MoveMessage.Clear();
	}

	// change page 명령이 있는 경우 실행
	if (!m_ChangeMessage.Empty())
	{
		m_StateFSM.SetCurrentState(m_ChangeMessage);
		m_ChangeMessage.Clear();
	}

	// 페이지 변경이 있는 경우 Clear/SetUp
	_CheckAndUpdatePageMovement();

	// 루트부터 지정된 페이지까지 갱신
	MkTimeState timeState;
	MK_TIME_MGR.GetCurrentTimeState(timeState);

	MK_INDEXING_LOOP(m_PathFromRootToActivatingLeaf, i)
	{
		m_PageTree.GetInstancePtr(m_PathFromRootToActivatingLeaf[i])->Update(timeState);
	}
}

void MkPageManager::__Clear(void)
{
	// 동작중인 페이지가 있으면 Clear() 호출
	if (!m_PathFromRootToActivatingLeaf.Empty())
	{
		MK_INDEXING_RLOOP(m_PathFromRootToActivatingLeaf, i)
		{
			m_PageTree.GetInstancePtr(m_PathFromRootToActivatingLeaf[i])->Clear();
		}
	}

	// 해제
	m_StateFSM.Clear();
	m_PageTree.Clear();
	m_PathFromRootToActivatingLeaf.Clear();
	m_LastTargetPageName.Clear();

	m_MoveMessage.Clear();
	m_ChangeMessage.Clear();
}

bool MkPageManager::_CheckAndUpdatePageMovement(void)
{
	// 현 페이지
	const MkHashStr& currentPage = GetCurrentPageName();

	// 변화가 없으면 리턴
	if (m_LastTargetPageName == currentPage)
		return true;

	// clear path
	MkDeque<MkHashStr> clearPath = m_PathFromRootToActivatingLeaf;

	// new path
	MkArray<MkHashStr> newBuffer;
	m_PageTree.GetPathToTargetNode(currentPage, newBuffer);
	MkDeque<MkHashStr> newPath;
	MK_INDEXING_LOOP(newBuffer, i)
	{
		newPath.PushBack(newBuffer[i]);
	}

	// setup path
	MkDeque<MkHashStr> setupPath = newPath;

	// count
	unsigned int cClear = clearPath.GetSize();
	unsigned int cSetup = setupPath.GetSize();
	unsigned int count = (cClear < cSetup) ? cClear : cSetup;

	// 동일 경로 삭제
	for (unsigned int i=0; i<count; ++i)
	{
		if (m_PathFromRootToActivatingLeaf[i] == newPath[i])
		{
			clearPath.PopFront();
			setupPath.PopFront();
		}
		else
			break;
	}

	// clear
	MkStr msgBuf;
	msgBuf.Reserve(clearPath.GetSize() * 64 + 100);
	msgBuf += L"> MkPageManager::Clear()";
	MK_INDEXING_RLOOP(clearPath, i)
	{
		m_PageTree.GetInstancePtr(clearPath[i])->Clear();
		msgBuf += L" - ";
		msgBuf += clearPath[i];
	}
	if (!clearPath.Empty())
	{
		MK_DEV_PANEL.MsgToLog(msgBuf, true);
	}

	// setup
	MkDataNode sharingNode(L"_SharingNode");
	msgBuf.Flush();
	msgBuf.Reserve(setupPath.GetSize() * 64 + 100);
	msgBuf += L"> MkPageManager::SetUp()";
	MK_INDEXING_LOOP(setupPath, i)
	{
		bool ok = m_PageTree.GetInstancePtr(setupPath[i])->SetUp(sharingNode);
		MK_CHECK(ok, MkStr(m_PageTree.GetInstancePtr(setupPath[i])->GetPageName()) + L"페이지 SetUp 실패")
			return false;

		msgBuf += L" - ";
		msgBuf += setupPath[i];
	}
	if (!setupPath.Empty())
	{
		MK_DEV_PANEL.MsgToLog(msgBuf, true);
	}

	// change
	m_PathFromRootToActivatingLeaf = newPath;
	m_LastTargetPageName = currentPage;
	return true;
}

//------------------------------------------------------------------------------------------------//
