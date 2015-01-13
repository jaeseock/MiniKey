#ifndef __MINIKEY_CORE_MKCHECK_H__
#define __MINIKEY_CORE_MKCHECK_H__

//------------------------------------------------------------------------------------------------//
// ���� ����
//
// "MK_CHECK(condition, message) action" �������� condition�� false�� �� �ش� action�� ����
// ���� ��忡 ���� �ٸ��� ����
// - debug : �޼��� �ڽ��� �������� ����. yes�� ���߰� no�� �α� ����� action ����
// - release : �޼��� �ڽ��� �������� ����. yes�� ���� ���� �߻�, no�� �α� ����� action ����
// - shipping : action�� ����
//
// ��ũ�� �κ��� ������ ������ �ڵ忡�� "MK_CHECK(index != 0, L"index is " + MkStr(rlt)) {...}" ������
// message�� ���ڿ� �������� ����ϴ� ��찡 ������ �� ������ ���� ����� ��� �۾��̹Ƿ� condition��
// false�� ��츸 ����ǵ��� �ϱ� ���ؼ���
//
// (NOTE) assert()�ʹ� �޸� release, shipping ��忡�� ���õ��� ����
//------------------------------------------------------------------------------------------------//
// ex>
//	MK_CHECK(ptr != NULL, L"pointer is NULL")
//	{
//		// if condition is false,
//		m_Available = false;
//	}
//
//	MK_CHECK(count == 0, L"count is not zero, " + MKStr(count))
//		return false;
//------------------------------------------------------------------------------------------------//


#ifdef _DEBUG

#define MK_CHECK(condition, message) if (!(condition)) if (MkCheck::ExceptionForDebug(message))

#else
#ifdef MK_SHIPPING

#define MK_CHECK(condition, message) if (!(condition))

#else

#define MK_CHECK(condition, message) if (!(condition)) if (MkCheck::ExceptionForRelease(__FUNCTION__, __LINE__, message))

#endif // MK_SHIPPING
#endif // _DEBUG

//------------------------------------------------------------------------------------------------//

class MkCheck
{
public:

	static bool ExceptionForDebug(const wchar_t* message);

	static bool ExceptionForRelease(const char* function, long lineNum, const wchar_t* message);
};

//------------------------------------------------------------------------------------------------//

#endif
