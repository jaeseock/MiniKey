
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"


//------------------------------------------------------------------------------------------------//

const static MkStr FILE_BLOCK_BEGIN = L"<FILE ";
const static MkStr HISTORY_BLOCK_BEGIN = L"<HISTORY ";
const static MkStr FILE_BLOCK_END = L">";

const static MkStr LASTWRITE_TAG_BEGIN = L"lastwrite=\"";
const static MkStr LOCALFILE_TAG_BEGIN = L"localfile=\"";
const static MkStr LOCALPATH_TAG_BEGIN = L"localpath=\"";
const static MkStr ALL_TAG_END = L"\"";

const static MkStr DIR_TAG = L"\\";

const static MkStr MSG_PREFIX_FILE = L"���� : ";
const static MkStr MSG_PREFIX_SUCCESS = L"���� : ";


// MainFramework ����
class MainFramework : public MkBaseFramework
{
protected:

	enum eApplicationState
	{
		eDoNothing = 0,
		eLoadXML,
		eChangeTimeStamp,
		eShowResult
	};

public:

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		if (draggedFilePathList.GetSize() == 1)
		{
			m_XMLFilePath = draggedFilePathList[0];
			draggedFilePathList.Clear();

			m_ApplicationState = eLoadXML;
		}
	}

	virtual void Update(void)
	{
		switch (m_ApplicationState)
		{
		case eDoNothing: // ���
			break;

		case eLoadXML: // xml ���� �о�鿩 text�� ����
			{
				bool ok = m_XmlText.ReadTextFile(m_XMLFilePath, false);
				m_ApplicationState = (ok) ? eChangeTimeStamp : eShowResult;

				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(m_XMLFilePath + MkStr(L" �б� ") + ((ok) ? L"����" : L"����"));
				
				MK_DEV_PANEL.MsgToFreeboard(1, MSG_PREFIX_SUCCESS + MkStr(m_TotalSuccessCount));

				MK_DEV_PANEL.MsgToFreeboard(0, MkStr(m_XmlText.GetSize()));
			}
			break;

		case eChangeTimeStamp:
			{
				const int countPerFrame = 100;
				for (int i=0; i<countPerFrame; ++i)
				{
					m_CurrXMLPos = m_XmlText.EraseFirstBlock(m_CurrXMLPos, HISTORY_BLOCK_BEGIN, FILE_BLOCK_END);
					if (m_CurrXMLPos == MKDEF_ARRAY_ERROR)
					{
						m_XMLFilePath.ChangeFileExtension(L"txt");
						m_XmlText.WriteToTextFile(m_XMLFilePath);

						m_ApplicationState = eShowResult;
						break;
					}
				}
				MK_DEV_PANEL.MsgToFreeboard(1, MkStr(m_XmlText.GetSize()));

				++m_TmpCnt;
				if ((m_TmpCnt % 100) == 99)
				{
					MkPathName backup = m_XMLFilePath;
					backup.ChangeFileName(L"backup_" + MkStr(m_TmpCnt));
					m_XmlText.WriteToTextFile(backup);
				}
				/*
				const int countPerFrame = 100;
				for (int i=0; i<countPerFrame; ++i)
				{
					// ���� ������ ����
					MkStr buffer;
					m_CurrXMLPos = m_XmlText.GetFirstStackBlock(m_CurrXMLPos, FILE_BLOCK_BEGIN, FILE_BLOCK_END, buffer);
					if (m_CurrXMLPos == MKDEF_ARRAY_ERROR)
					{
						m_ApplicationState = eShowResult;
						break;
					}
					else
					{
						// last write time stamp ����
						unsigned int currLinePos = 0;
						MkStr lastWriteBuffer;
						currLinePos = buffer.GetFirstBlock(currLinePos, LASTWRITE_TAG_BEGIN, ALL_TAG_END, lastWriteBuffer);
						if (currLinePos == MKDEF_ARRAY_ERROR) // tag�� ����
							continue;

						wchar_t* _tmp = NULL;
						unsigned __int64 timeStamp = _wcstoui64(lastWriteBuffer, &_tmp, 10);
						if (timeStamp == 0ui64) // �����ð��� 0
							continue;

						FILETIME* fileTime = reinterpret_cast<FILETIME*>(&timeStamp);
						
						// local file name ����
						MkStr localFileBuffer;
						currLinePos = buffer.GetFirstBlock(currLinePos, LOCALFILE_TAG_BEGIN, ALL_TAG_END, localFileBuffer);
						if ((currLinePos == MKDEF_ARRAY_ERROR) || localFileBuffer.Empty()) // tag�� ���ų� �����
							continue;

						// local path ����
						MkStr localPathBuffer;
						currLinePos = buffer.GetFirstBlock(currLinePos, LOCALPATH_TAG_BEGIN, ALL_TAG_END, localPathBuffer);
						if ((currLinePos == MKDEF_ARRAY_ERROR) || localPathBuffer.Empty()) // tag�� ���ų� �����
							continue;

						// ���� ���� ���� ����
						++m_TotalFileTagCount;
						MK_DEV_PANEL.MsgToFreeboard(0, MSG_PREFIX_FILE + MkStr(m_TotalFileTagCount));

						// ������ ��� ���������Ƿ� �ش� ������ �����ð��� ����
						localPathBuffer += DIR_TAG;
						localPathBuffer += localFileBuffer;

						HANDLE hFile = CreateFile(localPathBuffer.GetPtr(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							if (SetFileTime(hFile, NULL, NULL, fileTime) != 0)
							{
								++m_TotalSuccessCount; // success

								MK_DEV_PANEL.MsgToFreeboard(1, MSG_PREFIX_SUCCESS + MkStr(m_TotalSuccessCount));
							}

							CloseHandle(hFile);
						}
					}
				}
				*/
			}
			break;

		case eShowResult: // ��� ���
			{
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"���� ���� ���� : " + MkStr(m_TotalFileTagCount) + L" files");
				MK_DEV_PANEL.MsgToLog(L"���� �����ð� ���� ���� : " + MkStr(m_TotalSuccessCount) + L" files");

				m_ApplicationState = eDoNothing;
			}
			break;
		}
	}

	MainFramework() : MkBaseFramework()
	{
		m_ApplicationState = eDoNothing;
		m_TotalFileTagCount = 0;
		m_TotalSuccessCount = 0;
		m_CurrXMLPos = 0;

		m_TmpCnt = 0;
	}

	virtual ~MainFramework() {}

protected:

	eApplicationState m_ApplicationState;

	MkPathName m_XMLFilePath;
	MkStr m_XmlText;

	unsigned int m_TotalFileTagCount;
	unsigned int m_TotalSuccessCount;
	unsigned int m_CurrXMLPos;

	unsigned int m_TmpCnt;
};

// TestApplication ����
class MainApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new MainFramework; }

public:
	MainApplication() : MkWin32Application() {}
	virtual ~MainApplication() {}
};

//------------------------------------------------------------------------------------------------//

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MainApplication application;
	application.Run(hI, L"Rohan time stamp recovery", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, true, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

