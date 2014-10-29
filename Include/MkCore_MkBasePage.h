#ifndef __MINIKEY_CORE_MKBASEPAGE_H__
#define __MINIKEY_CORE_MKBASEPAGE_H__

//------------------------------------------------------------------------------------------------//
// MkPageManager�� ��ϵǴ� ������ ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"


class MkTimeState;
class MkDataNode;

class MkBasePage
{
public:

	// �ʱ�ȭ ���� ���� ����
	// sharingNode : ������ ��ȯ�� ���������� ȣ��Ǵ� Clear() ... SetUp() ���������� �����Ǵ� �ӽ� MkDataNode
	virtual bool SetUp(MkDataNode& sharingNode) { return true; }

	// ����
	virtual void Update(const MkTimeState& timeState) {}

	// ����
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	// ���� ������ �̸� ��ȯ
	inline const MkHashStr& GetPageName(void) { return m_PageName; }

	MkBasePage(const MkHashStr& name) { m_PageName = name; }
	virtual ~MkBasePage() {}

protected:

	MkHashStr m_PageName;
};

//------------------------------------------------------------------------------------------------//

#endif
