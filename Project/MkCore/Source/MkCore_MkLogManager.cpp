
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkLogManager.h"


//------------------------------------------------------------------------------------------------//

bool MkLogManager::CreateNewPage(const MkStr& pageName, const MkPathName& filePath)
{
	if ((!IsEnable()) || pageName.Empty())
		return false;

	// generate path
	MkPathName targetFilePath;
	if (filePath.Empty())
	{
		targetFilePath.ConvertToRootBasisAbsolutePath(MkStr(pageName) + L".log");
	}
	else
	{
		targetFilePath.ConvertToRootBasisAbsolutePath(filePath);
		if (!targetFilePath.GetPath().CheckAvailable())
			return false;
	}

	// create page
	MkMap<MkStr, MkPathName> tmpTable = m_PageTable;
	bool ok = !tmpTable.Exist(pageName);
	if (ok)
	{
		tmpTable.Create(pageName, targetFilePath);

		MkStr msgBuffer;
		msgBuffer.Reserve(400);
		msgBuffer += L"//-----------------------------------------------------------------------------//\n";
		msgBuffer += L"// [ ";
		msgBuffer += pageName;
		msgBuffer += L" ]\n";
		msgBuffer += L"//    - Begin at : ";
		msgBuffer += MK_SYS_ENV.GetCurrentSystemDate();
		msgBuffer += L" ";
		msgBuffer += _MakeCurrentTimeStr();
		msgBuffer += L"\n";
		msgBuffer += L"//-----------------------------------------------------------------------------//\n\n";

		ok = msgBuffer.WriteToTextFile(targetFilePath, true);
		if (ok)
		{
			m_PageTable = tmpTable;
		}
	}

	return ok;
}

void MkLogManager::Msg(const MkStr& pageName, const MkStr& message, bool addTime)
{
	if (!IsEnable())
		return;

	MkPathName targetPage = _GetTargetPage(pageName);
	if (!targetPage.Empty())
	{
		// time tag
		MkStr buffer;
		buffer.Reserve(20 + message.GetSize());
		if (addTime)
		{
			buffer += _MakeCurrentTimeStr();
			buffer += L" ";
		}

		// message
		buffer += message;
		buffer += L"\n";

		// print
		buffer.WriteToTextFile(targetPage, false);
	}
}

void MkLogManager::Msg(const MkStr& message, bool addTime)
{
	Msg(MkPathName::GetApplicationName(), message, addTime);
}

void MkLogManager::SendLogTo(const MkStr& pageName, const MkPathName& targetAddress)
{
	if (!IsEnable())
		return;

	if (targetAddress.Empty())
		return;

	MkPathName targetPage = _GetTargetPage(pageName);
	if (!targetPage.Empty())
	{
		MkPathName address = targetAddress;
		address.CheckAndAddBackslash();

		// ��ġ���� ����
		MkPathName batchFilePath;
		batchFilePath.ConvertToRootBasisAbsolutePath(MKDEF_SENDING_PATCH_FILE_NAME);

		MkStr dateStamp = MK_SYS_ENV.GetCurrentSystemDate();
		dateStamp.RemoveKeyword(L".");
		MkStr fileName = targetPage.GetFileName();

		// mkdir ��� ���
		MkStr batchBuffer;
		batchBuffer.Reserve(100 + (address.GetSize() * 2) + (dateStamp.GetSize() * 2) + targetPage.GetSize() + fileName.GetSize());
		batchBuffer += L"mkdir \"";
		batchBuffer += address;
		batchBuffer += dateStamp;
		batchBuffer += L"\"\n"; // DQM�� �־�� ��λ� ������ �ν�

		// copy ��� ���
		batchBuffer += L"copy \"";
		batchBuffer += targetPage;
		batchBuffer += L"\" \"";
		batchBuffer += address;
		batchBuffer += dateStamp;
		batchBuffer += L"\\";
		batchBuffer += fileName;
		batchBuffer += L"\"";

		// lock
		MkWrapInCriticalSection(m_CS_File)
		{
			// ��ġ���� ���
			batchBuffer.WriteToTextFile(batchFilePath);

			// ���� (�ӽ� ��ġ���� �����Ϸ��� �ּ� ���� ��)
			if (batchFilePath.ExcuteWindowProcess(batchFilePath))
			{
				// �ʹ� ���� ������ ���� ����
				//Sleep(1000);

				// ��ġ���� ����
				batchFilePath.DeleteCurrentFile();
			}
		}
	}
}

void MkLogManager::OpenLogFile(const MkStr& pageName)
{
	if (!IsEnable())
		return;

	MkPathName targetPage = _GetTargetPage(pageName);
	if (!targetPage.Empty())
	{
		targetPage.OpenFileInVerb();
	}
}

void MkLogManager::OpenSystemLogFile(void)
{
	OpenLogFile(MkPathName::GetApplicationName());
}

void MkLogManager::CreateCrashPage(const MkStr& message)
{
	if (!IsEnable())
		return;

	MkStr cpName = m_CrashPageName;
	if (!cpName.Empty()) // �̹� ũ���� �������� ����
		return;

	// ���� �̸�
	MkStr userName = MK_SYS_ENV.GetCurrentLogOnUserName();
	
	// Ÿ�� ������
	MkStr timeStamp = MK_SYS_ENV.GetCurrentSystemTime();
	timeStamp.RemoveKeyword(L":");

	// ������ ����
	cpName.Reserve(30 + userName.GetSize() + timeStamp.GetSize());
	cpName += MKDEF_CRASH_LOG_FILE_PREFIX;
	cpName += userName;
	cpName += L"_";
	cpName += timeStamp;

	m_CrashPageName = cpName;
	if (CreateNewPage(m_CrashPageName))
	{
		Msg(m_CrashPageName, message); // �޼��� ���
	}
}

void MkLogManager::MsgToCrashPage(const MkStr& message)
{
	if (!IsEnable())
		return;

	MkStr cpName = m_CrashPageName;
	if (!cpName.Empty())
	{
		Msg(m_CrashPageName, message, false);
	}
}

void MkLogManager::SendCrashLogTo(void)
{
	if (!IsEnable())
		return;

	MkStr cpName = m_CrashPageName;
	if (cpName.Empty())
		return;

	MkPathName crAddress = m_CrashTargetAddress;
	if (crAddress.Empty())
		return;

	SendLogTo(cpName, crAddress);
}

MkLogManager::MkLogManager() : MkSwitchedSingletonPattern<MkLogManager>()
{
	CreateNewPage(MkPathName::GetApplicationName());
}

MkLogManager::MkLogManager(bool enable) : MkSwitchedSingletonPattern<MkLogManager>(enable)
{
	CreateNewPage(MkPathName::GetApplicationName());
}

//------------------------------------------------------------------------------------------------//

MkPathName MkLogManager::_GetTargetPage(const MkStr& pageName)
{
	MkMap<MkStr, MkPathName> tmpTable = m_PageTable;
	return (tmpTable.Exist(pageName)) ? tmpTable[pageName] : L"";
}

MkStr MkLogManager::_MakeCurrentTimeStr(void) const
{
	MkStr timeBuf = MK_SYS_ENV.GetCurrentSystemTime();
	MkStr rlt;
	rlt.Reserve(2 + timeBuf.GetSize());
	rlt += L"[";
	rlt += timeBuf;
	rlt += L"]";
	return rlt;
}

//------------------------------------------------------------------------------------------------//

