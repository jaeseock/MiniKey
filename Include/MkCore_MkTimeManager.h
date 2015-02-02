#ifndef __MINIKEY_CORE_MKTIMEMANAGER_H__
#define __MINIKEY_CORE_MKTIMEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - time manager
//
// �ð� ������(���� ����) ������ ũ�� �ΰ��� ����� ����
// - �Ϲ����� ������� ���
//   * elapsed�� �������̶� ����ȸ���� �ǹ̰� ����
//   * �ǻ糭���� ������� ������ �����
//   * ��������� ���� ���� ������ ����
//   * ������ ��ŵ�� �ʿ� ����
// - RTS���ӿ��� ���̴� ������ ���
//   * �ʴ� ����Ƚ��, elapsed ���� ����(������ ��ŵ���� ���� ī��Ʈ ����)
//   * �������� ��ŵ �� �� ������ ���¸� ��ȭ��Ű�� ��� ������ ȣ��ȸ��, �ð����� ����
//   * ������ �ǻ糭�� ���·� seed �� ȣ��ȸ�� ������ ���� ����� ������
//   * ��������� ���� ���� ������ ��Ģ�� ������ �ؼ��ؾ� ��
// - MkTimeManager�� �ι�° ����� ä��
//
// GetTickCount() ����̹Ƿ� SetUp()�� �ִ� ��ȿ�Ⱓ�� 49.7��(60fps�� �뷫 259,200,000 frames)
//
// ������� ��� ���ؼ��� ���� ������ �ƴ� state�� ī�Ǻ��� �� ����
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkTimeState.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkSingletonPattern.h"


#define MK_TIME_MGR MkTimeManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkStr;

class MkTimeManager : public MkSingletonPattern<MkTimeManager>
{
public:

	// �ʱ�ȭ
	void SetUp(void);

	// ���� �� ���࿩�� ����
	void Update(void);

	// ���� ����
	inline void GetCurrentTimeState(MkTimeState& buffer) { buffer = m_CurrentTimeState; }

	// ���¸� "��:��:��:�и��� (������ī��Ʈ)" ������ ���ڿ��� ��ȯ
	void GetCurrentTimeState(MkStr& buffer);

	MkTimeManager();
	virtual ~MkTimeManager() {}

protected:

	MkLockable<DWORD> m_ZeroTime; // SetUp()�� ȣ��� ������ �ð�
	MkLockable<MkTimeState> m_CurrentTimeState;
};

//------------------------------------------------------------------------------------------------//

#endif
