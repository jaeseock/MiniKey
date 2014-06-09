#ifndef __MINIKEY_CORE_MKPAGEMANAGER_H__
#define __MINIKEY_CORE_MKPAGEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - page manager
//
// ���� ������ ����ȭ �ϰ� SetUp/Clear/Update�� ü�������� �ڵ�ȭ �ϱ� ���� �ý���
// - �� page�� Ʈ�������� �̷�
// - Root �������� ����
// - SetUp ������ root���� leaf
// - Update ������ root���� leaf
// - Clear ������ leaf ���� root
// - leaf ���������� leaf �������� �̵���, ��� �ִܰŸ��� �������� Clear()/SetUp() ����
//
// (NOTE) thread-safe�� ������� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkFiniteStateMachinePattern.h"
#include "MkCore_MkUniqueKeyTreePattern.h"
#include "MkCore_MkBasePage.h"
#include "MkCore_MkHashStr.h"

#define MK_PAGE_MGR MkPageManager::Instance()


class MkPageManager : public MkSingletonPattern<MkPageManager>
{
public:
	//------------------------------------------------------------------------------------------------//

	// �ʱ�ȭ
	// ��Ʈ ������ ���
	bool SetUp(MkBasePage* rootPage);
	
	// ������ ���
	bool RegisterChildPage(const MkHashStr& parentPage, MkBasePage* childPage);

	// ������ ���̺�(FSM) ���
	void SetPageFlowTable(const MkHashStr& pageIn, const MkHashStr& condition, const MkHashStr& pageOut);

	// FSM�� ���� �޼����� ������ ������ �̵�
	// Update()�� �����
	bool SetMoveMessage(const MkHashStr& moveMessage);

	// ���� leaf ������� �̵�
	// Update()�� �����
	bool ChangePageDirectly(const MkHashStr& pageName);

	// �־��� �������� leaf���� ���� ��ȯ
	inline bool IsLeaf(const MkHashStr& pageName) const { return m_PageTree.IsLeaf(pageName); }

	// �� ������ �̸�
	inline const MkHashStr& GetCurrentPageName(void) const { return m_StateFSM.GetCurrentState(); }

	//------------------------------------------------------------------------------------------------//
	// implementation

	void __Update(void);

	void __Clear(void);

	MkPageManager() : MkSingletonPattern<MkPageManager>() {}
	virtual ~MkPageManager() { __Clear(); }

protected:

	bool _CheckAndUpdatePageMovement(void);

	//------------------------------------------------------------------------------------------------//

protected:

	// FSM
	MkFiniteStateMachinePattern<MkHashStr, MkHashStr> m_StateFSM;

	// ������
	MkUniqueKeyTreePattern<MkHashStr, MkBasePage> m_PageTree;
	MkDeque<MkHashStr> m_PathFromRootToActivatingLeaf;
	MkHashStr m_LastTargetPageName;

	// message
	MkHashStr m_MoveMessage;
	MkHashStr m_ChangeMessage;
};

#endif
