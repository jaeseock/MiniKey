#ifndef __MINIKEY_CORE_MKLOGMANAGER_H__
#define __MINIKEY_CORE_MKLOGMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - log manager
//
// �⺻������ �������� �̸����� �ý��� �α׸� �ϳ� ����
//
// �� �������� ���������� �ܺ� ��η� ���� �� �� ����
// (NOTE) �߼� Ŭ���̾�Ʈ ������ �α׸� ������� ��ο� ���� ��������� �־�� ��
//
// ũ���� �߻��� ũ���� ���� �α׸� ������ ����
// - CreateCrashPage()�� �����ϰ� MsgToCrashPage()�� ����� �� ���������� SendCrashLogTo()�� ���� ����
//   ex> MK_LOG_MGR.SetCrashTargetAddress(L"\\\\192.168.1.10\\project\\Log");
//       ...
//       MK_LOG_MGR.SendCrashLogTo();
// - username�� �����ð��� �α� ���� �̸��� �߰��Ǿ� ���� ���ڸ� ���������� ���� ��ġ�� �����
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkLockable.h"

#define MK_LOG_MGR MkLogManager::Instance()


class MkLogManager : public MkSwitchedSingletonPattern<MkLogManager>
{
public:

	//------------------------------------------------------------------------------------------------//
	// management
	//------------------------------------------------------------------------------------------------//

	// �� �α� ������ ���� (producer)
	// (in) pageName : ������ �α� ������ �̸�
	// (in) filePath : �α������� ������ ����, Ȥ�� root directory ��� �����
	//   ���� �������� �ʾ��� ��� root directory�� (pageName).txt�� ������
	// return : ���� ����
	bool CreateNewPage(const MkStr& pageName, const MkPathName& filePath = L"");

	// �ش� �������� �޼��� ��� (consumer)
	// (in) pageName : ��� �α� ������ �̸�
	// (in) message : ��ϵ� �޼���
	// (in) addTime : ��� �ð� ǥ�� ����
	void Msg(const MkStr& pageName, const MkStr& message, bool addTime = false);

	// ����Ʈ �ý��� �α� �������� �޼��� ��� (consumer)
	// (in) message : ��ϵ� �޼���
	// (in) addTime : ��� �ð� ǥ�� ����
	void Msg(const MkStr& message, bool addTime = false);

	// �ش� �������� targetAddress�� ���� (consumer)
	// (in) pageName : ��� �α� ������ �̸�
	// (in) targetAddress : ���۵� ���
	void SendLogTo(const MkStr& pageName, const MkPathName& targetAddress);

	// �α� ���� ���� (consumer)
	// (in) pageName : ��� �α� ������ �̸�
	void OpenLogFile(const MkStr& pageName);

	// ����Ʈ �ý��� �α� ���� ���� (consumer)
	void OpenSystemLogFile(void);

	//------------------------------------------------------------------------------------------------//
	// crashing operation
	//------------------------------------------------------------------------------------------------//

	// ũ���� �߻��� ������ ������ �ּ� ����
	inline void SetCrashTargetAddress(const MkPathName& targetAddress) { m_CrashTargetAddress = targetAddress; }

	// ũ���� �߻��� ���� ������ �α� �������� �����ϰ� message ÷�� (producer)
	void CreateCrashPage(const MkStr& message);

	// ũ���� �������� �޼��� ��� (consumer)
	void MsgToCrashPage(const MkStr& message);

	// ũ���� �α׸� ������ �ּҷ� ���� (consumer)
	void SendCrashLogTo(void);

	//
	MkLogManager();
	MkLogManager(bool enable);
	virtual ~MkLogManager() {}

	//------------------------------------------------------------------------------------------------//

protected:

	MkPathName _GetTargetPage(const MkStr& pageName);

	MkStr _MakeCurrentTimeStr(void) const;

	//------------------------------------------------------------------------------------------------//

protected:

	// ũ���� ������. ũ���� �߻��� ����
	MkLockable<MkStr> m_CrashPageName;

	// ũ���� �߻��� ���� �ּ�
	MkLockable<MkPathName> m_CrashTargetAddress;

	// ���ϰ�� ���̺�
	MkLockable< MkMap<MkStr, MkPathName> > m_PageTable;

	// critical section
	MkCriticalSection m_CS_File;
};

#endif
