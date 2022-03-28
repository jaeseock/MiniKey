#ifndef __MINIKEY_CORE_MKSYSTEMENVIRONMENT_H__
#define __MINIKEY_CORE_MKSYSTEMENVIRONMENT_H__


//------------------------------------------------------------------------------------------------//
// global instance - system environment
//
// application �������� system, OS �� ���� ���� ����
// �������� ���� ���� ����
//
// �ʱ�ȭ ���� ���� ���� getter�� �����ϹǷ� thread-safe�� ����� �ʿ� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkVersionTag.h"

#define MK_SYS_ENV MkSystemEnvironment::Instance()


class MkSystemEnvironment : public MkSingletonPattern<MkSystemEnvironment>
{
public:

	//------------------------------------------------------------------------------------------------//

	// ���� ���
	enum eBuildMode
	{
		eDebug = 0,
		eRelease,
		eShipping
	};

	// cpu ������
	enum eProcessorArchitecture
	{
		eEtcPA = 0, // itanium or unknown
		eX86,
		eX64
	};

	//------------------------------------------------------------------------------------------------//

	// ���ø����̼� ���� ���� ��ȯ
	inline const MkVersionTag& GetApplicationVersionTag(void) { return m_ApplicationVersion; }

	// ���ø����̼� ���� ��� ��ȯ
	inline eBuildMode GetBuildMode(void) const { return m_BuildMode; }

	// cpu ������ ��ȯ
	inline eProcessorArchitecture GetProcessorArchitecture(void) const { return m_ProcessorArchitecture; }

	// core �� ��ȯ
	inline unsigned int GetNumberOfProcessors(void) const { return m_NumberOfProcessors; }

	// Windows ���� ���� ��ȯ
	// http://msdn.microsoft.com/en-us/library/ms724834(v=vs.85).aspx
	inline const MkStr& GetWindowsVersion(void) const { return m_WindowsVersion; }

	// Windows�� XP Ȥ�� �׺��� �ֽ� �������� ���� ��ȯ
	inline bool CheckWindowsIsXpOrHigher(void) const { return m_WindowsIsXpOrHigher; }

	// Windows�� Vista Ȥ�� �׺��� �ֽ� �������� ���� ��ȯ
	inline bool CheckWindowsIsVistaOrHigher(void) const { return m_WindowsIsVistaOrHigher; }

	// ����ȭ�� �ػ� ��ȯ
	inline const MkInt2& GetBackgroundResolution(void) const { return m_BackgroundResolution; }

	// ���� ���ڸ� ���� (��.��.��)
	static MkStr GetCurrentSystemDate(void);

	// ���� �ð��� ���� (��:��:��)
	static MkStr GetCurrentSystemTime(void);

	// ���� �ý��� �α׿µ� ���� �̸��� ����
	inline const MkStr& GetCurrentLogOnUserName(void) const { return m_CurrentUserName; }

	// �ý��� ���� ���
	void __PrintSystemInformationToLog(void) const;

	//------------------------------------------------------------------------------------------------//

	MkSystemEnvironment();
	virtual ~MkSystemEnvironment() {}

protected:

	void _Initialize(void);

	//------------------------------------------------------------------------------------------------//

protected:

	eBuildMode m_BuildMode;

	eProcessorArchitecture m_ProcessorArchitecture;

	unsigned int m_NumberOfProcessors;

	MkStr m_WindowsVersion;
	bool m_WindowsIsXpOrHigher;
	bool m_WindowsIsVistaOrHigher;

	MkInt2 m_BackgroundResolution;

	MkStr m_CurrentUserName;

	MkVersionTag m_ApplicationVersion;
	MkVersionTag m_MiniKeyVersion;
};

//------------------------------------------------------------------------------------------------//

#endif
