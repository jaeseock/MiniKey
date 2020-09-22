
//------------------------------------------------------------------------------------------------//

//#include <windows.h>

#include "MkCore_MkExcelFileInterface.h"

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

// steam -----------------------------------------------------

// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		return true; // EXIT_SUCCESS
	}

	virtual void StartLooping(void)
	{
	}

	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList)
	{
		if (draggedFilePathList.GetSize() == 1)
		{
			MkExcelFileInterface excel;
			if (excel.SetUp(draggedFilePathList[0]))
			{
				if (excel.SetActiveSheet(0))
				{
					// KR first
					if (m_KR.Empty())
					{
						unsigned int y = 1;
						int emptyCellCount = 0;

						while (emptyCellCount < 100)
						{
							MkStr enable;
							if (excel.GetData(y, 3, enable) && (!enable.Empty()))
							{
								if (!enable.ToBool())
								{
									MkStr idBuf, titleBuf, txtBuf[6], npcBuf;
									excel.GetData(y, 0, idBuf);
									excel.GetData(y, 1, titleBuf);
									for (int i=0; i<6; ++i)
									{
										excel.GetData(y, i + 4, txtBuf[i]);
									}
									excel.GetData(y, 10, npcBuf);

									MkHashStr npcKey = npcBuf;
									MkArray<QuestData>& qdList = m_KR.Exist(npcKey) ? m_KR[npcKey] : m_KR.Create(npcKey);

									QuestData& qd = qdList.PushBack();
									qd.id = idBuf;
									qd.title = titleBuf;
									for (int i=0; i<6; ++i)
									{
										qd.txt[i] = txtBuf[i];
									}
								}

								emptyCellCount = 0;
							}
							else
							{
								++emptyCellCount;
							}

							++y;
						}
					}
					// US second
					else
					{
						unsigned int y = 1;
						int emptyCellCount = 0;

						while (emptyCellCount < 100)
						{
							MkStr enable;
							if (excel.GetData(y, 3, enable) && (!enable.Empty()))
							{
								if (!enable.ToBool())
								{
									MkStr idBuf, titleBuf, txtBuf[6], npcBuf;
									excel.GetData(y, 0, idBuf);
									excel.GetData(y, 1, titleBuf);
									for (int i=0; i<6; ++i)
									{
										excel.GetData(y, i + 4, txtBuf[i]);
									}
									excel.GetData(y, 10, npcBuf);

									MkHashStr npcKey = npcBuf;
									MkArray<QuestData>& qdList = m_US.Exist(npcKey) ? m_US[npcKey] : m_US.Create(npcKey);

									QuestData& qd = qdList.PushBack();
									qd.id = idBuf;
									qd.title = titleBuf;
									for (int i=0; i<6; ++i)
									{
										qd.txt[i] = txtBuf[i];
									}
								}

								emptyCellCount = 0;
							}
							else
							{
								++emptyCellCount;
							}

							++y;
						}
					}
					/*
					// 무효화 파일 삭제
					unsigned int y = 1;
					int emptyCellCount = 0;

					int disableQuestCount = 0;
					int deletionCount = 0;

					while (emptyCellCount < 100)
					{
						MkStr enable;
						if (excel.GetData(y, 3, enable) && (!enable.Empty()))
						{
							if (!enable.ToBool())
							{
								++disableQuestCount;

								for (int x=4; x<10; ++x)
								{
									MkStr txtBuf;
									if (excel.GetData(y, x, txtBuf) && (!txtBuf.Empty()))
									{
										MkPathName fileName = txtBuf;
										fileName.DeleteCurrentFile();
										++deletionCount;
									}
								}
							}

							emptyCellCount = 0;
						}
						else
						{
							++emptyCellCount;
						}

						++y;
					}

					MK_DEV_PANEL.MsgToLog(L"무효 퀘스트 : " + MkStr(disableQuestCount) + L" 개");
					MK_DEV_PANEL.MsgToLog(L"삭제 텍스트 : " + MkStr(deletionCount) + L" 개");
					*/
				}
				else
					::MessageBox(m_MainWindow.GetWindowHandle(), L"유효한 시트가 없습니다", L"ERROR!!!", MB_OK);
			}
			else
				::MessageBox(m_MainWindow.GetWindowHandle(), L"정상적인 엑셀 파일이 아님", L"ERROR!!!", MB_OK);
		}
		else if (draggedFilePathList.GetSize() > 1)
		{
			::MessageBox(m_MainWindow.GetWindowHandle(), L"하나의 파일만 넣어주세요~", L"ERROR!!!", MB_OK);
		}

		draggedFilePathList.Clear();
	}

	virtual void Update(void)
	{
		if ((!m_KR.Empty()) && (!m_US.Empty()))
		{
			MkArray<MkHashStr> krNpcList;
			m_KR.GetKeyList(krNpcList);

			MkArray<MkHashStr> usNpcList;
			m_US.GetKeyList(usNpcList);

			MkArray<MkHashStr> krOnly, both, usOnly;
			krNpcList.IntersectionTest(usNpcList, krOnly, both, usOnly);

			MkExcelFileInterface excel;
			if (excel.SetUp())
			{
				// 한국 전용
				if (excel.CreateSheet(L"KR only") && excel.SetActiveSheet(L"KR only"))
				{
					excel.PutData(0, 0, MkStr(L"NPC name"));
					excel.SetBorder(0, 0, 1);
					excel.PutData(0, 1, MkStr(L"ID"));
					excel.SetBorder(0, 1, 1);
					excel.PutData(0, 2, MkStr(L"Title"));
					excel.SetBorder(0, 2, 1);

					unsigned int currRow = 1;
					MK_INDEXING_LOOP(krOnly, i)
					{
						const MkHashStr& npcName = krOnly[i];
						excel.PutData(currRow, 0, npcName.GetString());
						excel.SetBorder(currRow, 0, 1);

						MkArray<QuestData>& qdList = m_KR[npcName];
						MK_INDEXING_LOOP(qdList, j)
						{
							QuestData& qd = qdList[j];
							excel.PutData(currRow, 1, qd.id);
							excel.SetBorder(currRow, 1, 1);
							excel.PutData(currRow, 2, qd.title);
							excel.SetBorder(currRow, 2, 1);
							excel.PutData(currRow, 3, qd.txt[0]);
							excel.SetBorder(currRow, 3, 1);
							excel.PutData(currRow, 4, qd.txt[1]);
							excel.SetBorder(currRow, 4, 1);
							excel.PutData(currRow, 5, qd.txt[2]);
							excel.SetBorder(currRow, 5, 1);
							excel.PutData(currRow, 6, qd.txt[3]);
							excel.SetBorder(currRow, 6, 1);
							excel.PutData(currRow, 7, qd.txt[4]);
							excel.SetBorder(currRow, 7, 1);
							excel.PutData(currRow, 8, qd.txt[5]);
							excel.SetBorder(currRow, 8, 1);

							++currRow;
						}
					}

					excel.AutoFit(0);
					excel.AutoFit(1);
					excel.AutoFit(2);
					excel.AutoFit(3);
					excel.AutoFit(4);
					excel.AutoFit(5);
					excel.AutoFit(6);
					excel.AutoFit(7);
					excel.AutoFit(8);
				}

				// 공통
				if (excel.CreateSheet(L"Both") && excel.SetActiveSheet(L"Both"))
				{
					excel.PutData(0, 0, MkStr(L"NPC name"));
					excel.SetBorder(0, 0, 1);
					excel.PutData(0, 1, MkStr(L"ID"));
					excel.SetBorder(0, 1, 1);
					excel.PutData(0, 2, MkStr(L"Title"));
					excel.SetBorder(0, 2, 1);

					unsigned int currRow = 1;
					MK_INDEXING_LOOP(both, i)
					{
						const MkHashStr& npcName = both[i];
						excel.PutData(currRow, 0, npcName.GetString());
						excel.SetBorder(currRow, 0, 1);

						{
							MkArray<QuestData>& qdList = m_KR[npcName];
							MK_INDEXING_LOOP(qdList, j)
							{
								QuestData& qd = qdList[j];
								excel.PutData(currRow, 1, qd.id);
								excel.SetBorder(currRow, 1, 1);
								excel.PutData(currRow, 2, qd.title);
								excel.SetBorder(currRow, 2, 1);
								excel.PutData(currRow, 3, qd.txt[0]);
								excel.SetBorder(currRow, 3, 1);
								excel.PutData(currRow, 4, qd.txt[1]);
								excel.SetBorder(currRow, 4, 1);
								excel.PutData(currRow, 5, qd.txt[2]);
								excel.SetBorder(currRow, 5, 1);
								excel.PutData(currRow, 6, qd.txt[3]);
								excel.SetBorder(currRow, 6, 1);
								excel.PutData(currRow, 7, qd.txt[4]);
								excel.SetBorder(currRow, 7, 1);
								excel.PutData(currRow, 8, qd.txt[5]);
								excel.SetBorder(currRow, 8, 1);

								++currRow;
							}
						}
						{
							MkArray<QuestData>& qdList = m_US[npcName];
							MK_INDEXING_LOOP(qdList, j)
							{
								QuestData& qd = qdList[j];
								excel.PutData(currRow, 1, qd.id);
								excel.SetBorder(currRow, 1, 1);
								excel.PutData(currRow, 2, qd.title);
								excel.SetBorder(currRow, 2, 1);
								excel.PutData(currRow, 3, qd.txt[0]);
								excel.SetBorder(currRow, 3, 1);
								excel.PutData(currRow, 4, qd.txt[1]);
								excel.SetBorder(currRow, 4, 1);
								excel.PutData(currRow, 5, qd.txt[2]);
								excel.SetBorder(currRow, 5, 1);
								excel.PutData(currRow, 6, qd.txt[3]);
								excel.SetBorder(currRow, 6, 1);
								excel.PutData(currRow, 7, qd.txt[4]);
								excel.SetBorder(currRow, 7, 1);
								excel.PutData(currRow, 8, qd.txt[5]);
								excel.SetBorder(currRow, 8, 1);

								++currRow;
							}
						}
					}

					excel.AutoFit(0);
					excel.AutoFit(1);
					excel.AutoFit(2);
					excel.AutoFit(3);
					excel.AutoFit(4);
					excel.AutoFit(5);
					excel.AutoFit(6);
					excel.AutoFit(7);
					excel.AutoFit(8);
				}

				// 북미 전용
				if (excel.CreateSheet(L"US only") && excel.SetActiveSheet(L"US only"))
				{
					excel.PutData(0, 0, MkStr(L"NPC name"));
					excel.SetBorder(0, 0, 1);
					excel.PutData(0, 1, MkStr(L"ID"));
					excel.SetBorder(0, 1, 1);
					excel.PutData(0, 2, MkStr(L"Title"));
					excel.SetBorder(0, 2, 1);

					unsigned int currRow = 1;
					MK_INDEXING_LOOP(usOnly, i)
					{
						const MkHashStr& npcName = usOnly[i];
						excel.PutData(currRow, 0, npcName.GetString());
						excel.SetBorder(currRow, 0, 1);

						MkArray<QuestData>& qdList = m_US[npcName];
						MK_INDEXING_LOOP(qdList, j)
						{
							QuestData& qd = qdList[j];
							excel.PutData(currRow, 1, qd.id);
							excel.SetBorder(currRow, 1, 1);
							excel.PutData(currRow, 2, qd.title);
							excel.SetBorder(currRow, 2, 1);
							excel.PutData(currRow, 3, qd.txt[0]);
							excel.SetBorder(currRow, 3, 1);
							excel.PutData(currRow, 4, qd.txt[1]);
							excel.SetBorder(currRow, 4, 1);
							excel.PutData(currRow, 5, qd.txt[2]);
							excel.SetBorder(currRow, 5, 1);
							excel.PutData(currRow, 6, qd.txt[3]);
							excel.SetBorder(currRow, 6, 1);
							excel.PutData(currRow, 7, qd.txt[4]);
							excel.SetBorder(currRow, 7, 1);
							excel.PutData(currRow, 8, qd.txt[5]);
							excel.SetBorder(currRow, 8, 1);

							++currRow;
						}
					}

					excel.AutoFit(0);
					excel.AutoFit(1);
					excel.AutoFit(2);
					excel.AutoFit(3);
					excel.AutoFit(4);
					excel.AutoFit(5);
					excel.AutoFit(6);
					excel.AutoFit(7);
					excel.AutoFit(8);
				}

				excel.SaveAs(L"Result.xlsx");
			}

			MK_DEV_PANEL.MsgToLog(L"한국 전용 : " + MkStr(krOnly.GetSize()));
			MK_DEV_PANEL.MsgToLog(L"공통 : " + MkStr(both.GetSize()));
			MK_DEV_PANEL.MsgToLog(L"북미 전용 : " + MkStr(usOnly.GetSize()));

			//MkHashMapLooper< MkHashStr, MkArray<QuestData> > krLooper(m_KR);
			//MK_STL_LOOP(krLooper)
			//{
			//	const MkHashStr& npcName = krLooper.GetCurrentKey();
			//	MkArray<QuestData>& qdList
			//}

			// end
			m_KR.Clear();
			m_US.Clear();
		}
	}

	virtual void Clear(void)
	{
	}

	TestFramework() : MkBaseFramework()
	{
	}

	virtual ~TestFramework()
	{
	}

protected:

	typedef struct _QuestData
	{
		MkStr id;
		MkStr title;
		MkStr txt[6];
	}
	QuestData;

	MkHashMap< MkHashStr, MkArray<QuestData> > m_KR;
	MkHashMap< MkHashStr, MkArray<QuestData> > m_US;
};


class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"RohanDisableQuestRemover", L"..\\", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, false, true);

	return 0;
}

//------------------------------------------------------------------------------------------------//

