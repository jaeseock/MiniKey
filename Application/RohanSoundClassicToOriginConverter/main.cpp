
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkBaseFramework.h"

#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkDataNode.h"


//------------------------------------------------------------------------------------------------//

// MainPage ����
class MainPage : public MkBasePage
{
protected:

	enum eAppState
	{
		eAp_Start = 0,

		eAp_ReadClassicNormalMonsterData,
		eAp_ReadClassicIndunMonsterData,
		eAp_ReadOriginMonsterData,
		eAp_BuildClassicBags,
		eAp_MatchData,
		eAp_ConvertClassic,
		eAp_ConvertOrigin,

		eAp_End
	};

	typedef struct _ClassicMonsterData
	{
		unsigned int type;
		MkStr name;
		MkPathName bagFile;
		MkStr model;
	}
	ClassicMonsterData;

	typedef struct _OriginMonsterData
	{
		unsigned int type;
		MkStr name;
		MkPathName bagFile;
		MkStr model;
		unsigned int matchedClassicType;
	}
	OriginMonsterData;

public:

	virtual void Update(const MkTimeState& timeState)
	{
		switch (m_AppState)
		{
		case eAp_Start:
			{
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"//------------------------------------------------------------------------------------------------//", false);
				MK_DEV_PANEL.MsgToLog(L"// [RohanSoundClassicToOriginConverter ���������~]", false);
				MK_DEV_PANEL.MsgToLog(L"//------------------------------------------------------------------------------------------------//", false);
				MK_DEV_PANEL.InsertEmptyLine();
				m_AppState = eAp_ReadClassicNormalMonsterData;
			}
			break;

		case eAp_ReadClassicNormalMonsterData:
			{
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;
				frInterface.SetUp(L"c_monster.bin");
				unsigned int fileSize = frInterface.Read(destBuffer, MkArraySection(0));
				frInterface.Clear();

				unsigned int count = fileSize / 788;
				unsigned int success = 0;

				for (unsigned int i=0; i<count; ++i) // classic ROHAN_MONSTERINFO ����
				{
					ClassicMonsterData md;
					MkHashStr modelFile;

					unsigned int pos = i * 788;
					{
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 4, 4));
						memcpy_s(reinterpret_cast<unsigned char*>(&md.type), 4, descriptor, 4);
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 58, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						md.name.ImportMultiByteString(std::string(buf));
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 110, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						MkStr model;
						model.ImportMultiByteString(std::string(buf));
						model.RemoveBlank();
						model.ToLower();
						modelFile = model;
						md.model = model;
					}

					if (!modelFile.Empty())
					{
						if (m_ClassicMonsterData.Exist(modelFile))
						{
							m_ClassicMonsterData[modelFile].PushBack(md);
						}
						else
						{
							MkArray<ClassicMonsterData>& mdArray = m_ClassicMonsterData.Create(modelFile);
							mdArray.Reserve(8);
							mdArray.PushBack(md);
						}

						++success;
					}
				}

				MK_DEV_PANEL.MsgToLog(L"eAp_ReadClassicNormalMonsterData : " + MkStr(success) + L" / " + MkStr(count), false);

				m_AppState = eAp_ReadClassicIndunMonsterData;
			}
			break;

		case eAp_ReadClassicIndunMonsterData:
			{
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;
				frInterface.SetUp(L"c_indunmonster.bin");
				unsigned int fileSize = frInterface.Read(destBuffer, MkArraySection(0));
				frInterface.Clear();

				unsigned int count = fileSize / 792;
				unsigned int success = 0;

				for (unsigned int i=0; i<count; ++i) // classic ROHAN_INDUN_MONSTERINFO ����
				{
					ClassicMonsterData md;
					MkHashStr modelFile;

					unsigned int pos = i * 792;
					{
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 8, 4));
						memcpy_s(reinterpret_cast<unsigned char*>(&md.type), 4, descriptor, 4);
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 62, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						md.name.ImportMultiByteString(std::string(buf));
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 114, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						MkStr model;
						model.ImportMultiByteString(std::string(buf));
						model.RemoveBlank();
						model.ToLower();
						modelFile = model;
						md.model = model;
					}

					if (!modelFile.Empty())
					{
						if (m_ClassicMonsterData.Exist(modelFile))
						{
							m_ClassicMonsterData[modelFile].PushBack(md);
						}
						else
						{
							MkArray<ClassicMonsterData>& mdArray = m_ClassicMonsterData.Create(modelFile);
							mdArray.Reserve(8);
							mdArray.PushBack(md);
						}

						++success;
					}
				}

				MK_DEV_PANEL.MsgToLog(L"eAp_ReadClassicIndunMonsterData : " + MkStr(success) + L" / " + MkStr(count), false);

				m_AppState = eAp_ReadOriginMonsterData;
			}
			break;

		case eAp_ReadOriginMonsterData:
			{
				MkByteArray destBuffer;
				MkInterfaceForFileReading frInterface;
				frInterface.SetUp(L"o_monster.bin");
				unsigned int fileSize = frInterface.Read(destBuffer, MkArraySection(0));
				frInterface.Clear();

				unsigned int count = fileSize / 804;
				unsigned int success = 0;

				for (unsigned int i=0; i<count; ++i) // origin ROHAN_MONSTERINFO ����
				{
					OriginMonsterData md;
					md.matchedClassicType = 0;
					MkHashStr modelFile;

					unsigned int pos = i * 804;
					{
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 4, 4));
						memcpy_s(reinterpret_cast<unsigned char*>(&md.type), 4, descriptor, 4);
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 8, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						MkStr model;
						model.ImportMultiByteString(std::string(buf));
						model.RemoveBlank();
						model.ToLower();
						modelFile = model;
						md.model = model;
					}
					{
						char buf[50] = {0, };
						MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(pos + 108, 50));
						memcpy_s(reinterpret_cast<unsigned char*>(&buf), 50, descriptor, 50);
						md.name.ImportMultiByteString(std::string(buf));
					}

					if (!modelFile.Empty())
					{
						if (m_OriginMonsterData.Exist(modelFile))
						{
							m_OriginMonsterData[modelFile].PushBack(md);
						}
						else
						{
							MkArray<OriginMonsterData>& mdArray = m_OriginMonsterData.Create(modelFile);
							mdArray.Reserve(8);
							mdArray.PushBack(md);
						}

						++success;
					}
				}

				MK_DEV_PANEL.MsgToLog(L"eAp_ReadOriginMonsterData : " + MkStr(success) + L" / " + MkStr(count), false);

				m_AppState = eAp_BuildClassicBags;
			}
			break;

		case eAp_BuildClassicBags:
			{
				MkPathName bagDir = L"Act_Resolve";
				MkArray<MkPathName> emptyPFilter;
				MkArray<MkStr> emptySFilter;
				MkArray<MkPathName> extFilter;
				extFilter.PushBack(L"bag");
				MkArray<MkPathName> fileList;

				bagDir.GetWhiteFileList(fileList, emptyPFilter, extFilter, emptySFilter, emptyPFilter, false, false);

				unsigned int success = 0;
				MK_INDEXING_LOOP(fileList, i)
				{
					const MkPathName& currFile = fileList[i];
					MkPathName fileName = currFile.GetFileName(false); // ex> L"262145[����]"

					MkStr tBuf;
					fileName.GetSubStr(MkArraySection(0, 6), tBuf);
					unsigned int type = tBuf.ToUnsignedInteger();

					if (type > 0)
					{
						m_Bags.Create(type, currFile);
						++success;
					}
				}

				MK_DEV_PANEL.MsgToLog(L"eAp_BuildClassicBags : " + MkStr(success) + L" / " + MkStr(fileList.GetSize()), false);

				m_AppState = eAp_MatchData;
			}
			break;

		case eAp_MatchData:
			{
				{
					// classic ���Ϳ� bag ���� ��Ī
					MkArray<ClassicMonsterData*> missingList(m_Bags.GetSize()); // classic ���� �� bag ������ ���� data
					MkArray<unsigned int> classicTypes(m_Bags.GetSize()); // classic�� ��� ���� type

					MkHashMapLooper<MkHashStr, MkArray<ClassicMonsterData> > looper(m_ClassicMonsterData);
					MK_STL_LOOP(looper)
					{
						MkArray<ClassicMonsterData>& arr = looper.GetCurrentField();
						MK_INDEXING_LOOP(arr, i)
						{
							ClassicMonsterData& md = arr[i];
							if (m_Bags.Exist(md.type))
							{
								md.bagFile = m_Bags[md.type];
							}
							else
							{
								missingList.PushBack(&md);
							}

							classicTypes.PushBack(md.type);
						}
					}

					// classic ���� �� bag ������ �Ҵ���� ���� ���͵��� ���� ���� ���� ���͸� �˻��� ��ü
					m_RestoredMonster.Reserve(missingList.GetSize());

					looper.SetUp(m_ClassicMonsterData);
					MK_STL_LOOP(looper)
					{
						int validIndex = -1;
						MkArray<ClassicMonsterData>& arr = looper.GetCurrentField();
						MK_INDEXING_LOOP(arr, i)
						{
							ClassicMonsterData& md = arr[i];
							if (!md.bagFile.Empty())
							{
								validIndex = i;
								break;
							}
						}

						if (validIndex != -1)
						{
							const MkPathName& sharedBagFile = arr[validIndex].bagFile;
							MK_INDEXING_LOOP(arr, i)
							{
								ClassicMonsterData& md = arr[i];
								if (md.bagFile.Empty())
								{
									md.bagFile = sharedBagFile;
									m_RestoredMonster.PushBack(&md);
								}
							}
						}
					}

					// classic ���� �� bag ������ �Ҵ���� ���� ���͵�
					if (missingList.GetSize() > 0)
					{
						MK_DEV_PANEL.InsertEmptyLine();
						MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(missingList.GetSize()) + L" ���� classic ���ʹ� bag ������ ����", false);

						unsigned int restoredCount = 0;
						MK_INDEXING_LOOP(missingList, i)
						{
							ClassicMonsterData& md = *missingList[i];
							if (!md.bagFile.Empty())
							{
								MkStr msg = L"\t" + MkStr(md.type) + L"\t" + md.name;
								msg += L" -> ";
								msg += md.bagFile;
								msg += L" ���Ϸ� ��ü";
								MK_DEV_PANEL.MsgToLog(msg, false);

								++restoredCount;
							}
						}

						if (restoredCount > 0)
						{
							MK_DEV_PANEL.MsgToLog(L"\t* classic ���� �� bag ������ ���� " + MkStr(restoredCount) + L" ���� ����", false);
							MK_DEV_PANEL.InsertEmptyLine();
						}

						MK_INDEXING_LOOP(missingList, i)
						{
							ClassicMonsterData& md = *missingList[i];
							if (md.bagFile.Empty())
							{
								MkStr msg = L"\t" + MkStr(md.type) + L"\t" + md.name;
								msg += L" (";
								msg += md.model;
								msg += L")";
								MK_DEV_PANEL.MsgToLog(msg, false);
							}
						}

						unsigned int missingCount = missingList.GetSize() - restoredCount;
						if (missingCount > 0)
						{
							MK_DEV_PANEL.MsgToLog(L"\t* classic ���� �� bag ������ ���� " + MkStr(missingCount) + L" ���� �������� ����", false);
							MK_DEV_PANEL.InsertEmptyLine();
						}

						missingList.Clear();
					}
					else
					{
						MK_DEV_PANEL.MsgToLog(L"��� classic ���ʹ� bag ������ ������ ����", false);
					}

					// bag ���� �� �ش�Ǵ� classic ���Ͱ� ���� ���ϵ�
					MkArray<unsigned int> bagKeys;
					m_Bags.GetKeyList(bagKeys);

					MkArray<unsigned int> bagOnly;
					bagKeys.GetDifferenceOfSets(classicTypes, bagOnly);
					if (bagOnly.GetSize() > 0)
					{
						MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(bagOnly.GetSize()) + L" ���� bag ���Ͽ� �ش��ϴ� ���Ͱ� ����", false);

						MK_INDEXING_LOOP(bagOnly, i)
						{
							MK_DEV_PANEL.MsgToLog(L"\t" + m_Bags[bagOnly[i]], false);
						}
						bagOnly.Clear();
					}
					else
					{
						MK_DEV_PANEL.MsgToLog(L"��� bag ���Ͽ� �ش��ϴ� classic ���� ����", false);
						MK_DEV_PANEL.InsertEmptyLine();
					}
				}

				{
					// origin ���Ϳ� classic ���� ��Ī
					MkArray<OriginMonsterData*> noneModelList(m_Bags.GetSize()); // origin ���� �� ���� ���� ���� classic ���Ͱ� ���� data
					MkArray<OriginMonsterData*> noneBagList(m_Bags.GetSize()); // origin ���� �� ���� ���� ���� classic ���Ϳ� bag ������ ���� data
					unsigned int total = 0;
					m_TargetOriginMonsterCount = 0;

					MkHashMapLooper<MkHashStr, MkArray<OriginMonsterData> > looper(m_OriginMonsterData);
					MK_STL_LOOP(looper)
					{
						const MkHashStr& key = looper.GetCurrentKey();
						MkArray<OriginMonsterData>& arr = looper.GetCurrentField();
						total += arr.GetSize();

						if (m_ClassicMonsterData.Exist(key))
						{
							// ���� ���� ���� classic ���ʹ� ���� sound�� �����Ŷ� ����
							ClassicMonsterData& cmd = m_ClassicMonsterData[key][0];

							MK_INDEXING_LOOP(arr, i)
							{
								OriginMonsterData& omd = arr[i];

								if (cmd.bagFile.Empty())
								{
									noneBagList.PushBack(&omd);
								}
								else
								{
									omd.matchedClassicType = cmd.type;
									omd.bagFile = cmd.bagFile;

									++m_TargetOriginMonsterCount;
								}
							}
						}
						else
						{
							MK_INDEXING_LOOP(arr, i)
							{
								OriginMonsterData& omd = arr[i];
								noneModelList.PushBack(&omd);
							}
						}
					}

					MK_DEV_PANEL.MsgToLog(L"��Ī ���� : " + MkStr(m_TargetOriginMonsterCount) + L" / " + MkStr(total), false);

					// origin ���� �� ���� ���� ���� classic ���Ͱ� ���� data
					if (noneModelList.GetSize() > 0)
					{
						MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(noneModelList.GetSize()) + L" ���� origin ���ʹ� ���� ���� ���� classic ���Ͱ� ����", false);

						MK_INDEXING_LOOP(noneModelList, i)
						{
							OriginMonsterData& md = *noneModelList[i];
							MK_DEV_PANEL.MsgToLog(L"\t" + MkStr(md.type) + L"\t" + md.name + L"(" + md.model + L")", false);
						}
						noneModelList.Clear();
					}

					// origin ���� �� ���� ���� ���� classic ���Ϳ� bag ������ ���� data
					if (noneBagList.GetSize() > 0)
					{
						MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(noneBagList.GetSize()) + L" ���� origin ���ʹ� ���� ���� ���� classic ���Ϳ� bag ������ ����", false);

						MK_INDEXING_LOOP(noneBagList, i)
						{
							OriginMonsterData& md = *noneBagList[i];
							MK_DEV_PANEL.MsgToLog(L"\t" + MkStr(md.type) + L"\t" + md.name + L"(" + md.model + L")", false);
						}
						noneBagList.Clear();
					}

					// �ʿ� ����
					//m_ClassicMonsterData.Clear();
					m_Bags.Clear();
				}

				m_AppState = eAp_ConvertClassic;
			}
			break;

		case eAp_ConvertClassic:
			{
				MkPathName bagDir = L"Classic\\";
				bagDir.MakeDirectoryPath();

				MkArray<ClassicMonsterData*> failedList(m_RestoredMonster.GetSize());
				MkArray<MkStr> failedLog(m_RestoredMonster.GetSize());

				
				MK_INDEXING_LOOP(m_RestoredMonster, i)
				{
					ClassicMonsterData& md = *m_RestoredMonster[i];
					
					MkPathName classicBagPath;
					classicBagPath.Reserve(100);
					classicBagPath += L"Act_Resolve\\";
					classicBagPath += md.bagFile;

					MkByteArray destBuffer;
					MkInterfaceForFileReading frInterface;
					frInterface.SetUp(classicBagPath);
					unsigned int fileSize = frInterface.Read(destBuffer, MkArraySection(0));
					frInterface.Clear();

					if (fileSize >= 4)
					{
						// ActionMasterMainHeader : 4byte
						// __ActionCommand_IO_Struct : 352 byte, type(_OwnerType) pos 280

						unsigned int count = 0;
						{
							MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(0, 4));
							memcpy_s(reinterpret_cast<unsigned char*>(&count), 4, descriptor, 4);
						}

						if ((count > 0) && (fileSize == (4 + 352 * count)))
						{
							unsigned int success = 0;
							for (unsigned int j=0; j<count; ++j)
							{
								MkByteArrayDescriptor descriptor(reinterpret_cast<const unsigned char*>(&md.type), 4);
								if (destBuffer.Overwrite(4 + 352 * j + 280, descriptor))
								{
									++success;
								}
								else
									break;
							}

							if (success == count)
							{
								MkPathName newBagPath;
								newBagPath.Reserve(100);
								newBagPath += bagDir;
								newBagPath += MkStr(md.type);
								newBagPath += L"[";
								newBagPath += md.name;
								newBagPath += L"].bag";

								MkInterfaceForFileWriting fwInterface;
								fwInterface.SetUp(newBagPath, true, false);
								if (fwInterface.Write(destBuffer, MkArraySection(0)) != fileSize)
								{
									failedLog.PushBack(newBagPath);
									failedList.PushBack(&md);
								}

								fwInterface.Clear();
							}
							else
							{
								failedLog.PushBack(L"count : " + MkStr(count) + L", success : " + MkStr(success));
								failedList.PushBack(&md);
							}
						}
						else
						{
							failedLog.PushBack(L"count : " + MkStr(count) + L", fileSize : " + MkStr(fileSize));
							failedList.PushBack(&md);
						}
					}
					else
					{
						failedLog.PushBack(L"fileSize : " + MkStr(fileSize));
						failedList.PushBack(&md);
					}
				}

				unsigned int success = m_RestoredMonster.GetSize() - failedList.GetSize();
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"��ü���� ���� : " + MkStr(success) + L" / " + MkStr(m_RestoredMonster.GetSize()), false);

				if (failedList.GetSize() > 0)
				{
					MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(failedList.GetSize()) + L" ���� classic ���ʹ� bag ���� ��ȯ ����", false);

					MK_INDEXING_LOOP(failedList, i)
					{
						ClassicMonsterData& md = *failedList[i];
						MK_DEV_PANEL.MsgToLog(L"\t" + MkStr(md.type) + L"\t" + MkStr(md.name) + L" : " + failedLog[i], false);
					}
				}
				MK_DEV_PANEL.InsertEmptyLine();

				m_AppState = eAp_ConvertOrigin;
			}
			break;

		case eAp_ConvertOrigin:
			{
				MkPathName bagDir = L"Origin\\";
				bagDir.MakeDirectoryPath();

				MkArray<OriginMonsterData*> failedList(m_TargetOriginMonsterCount);
				MkArray<MkStr> failedLog(m_TargetOriginMonsterCount);

				MkHashMapLooper<MkHashStr, MkArray<OriginMonsterData> > looper(m_OriginMonsterData);
				MK_STL_LOOP(looper)
				{
					MkArray<OriginMonsterData>& arr = looper.GetCurrentField();
					MK_INDEXING_LOOP(arr, i)
					{
						OriginMonsterData& md = arr[i];
						if (md.matchedClassicType > 0)
						{
							MkPathName classicBagPath;
							classicBagPath.Reserve(100);
							classicBagPath += L"Act_Resolve\\";
							classicBagPath += md.bagFile;

							MkByteArray destBuffer;
							MkInterfaceForFileReading frInterface;
							frInterface.SetUp(classicBagPath);
							unsigned int fileSize = frInterface.Read(destBuffer, MkArraySection(0));
							frInterface.Clear();

							if (fileSize >= 4)
							{
								// ActionMasterMainHeader : 4byte
								// __ActionCommand_IO_Struct : 352 byte, type(_OwnerType) pos 280

								unsigned int count = 0;
								{
									MkByteArrayDescriptor descriptor = destBuffer.GetMemoryBlockDescriptor(MkArraySection(0, 4));
									memcpy_s(reinterpret_cast<unsigned char*>(&count), 4, descriptor, 4);
								}

								if ((count > 0) && (fileSize == (4 + 352 * count)))
								{
									unsigned int success = 0;
									for (unsigned int j=0; j<count; ++j)
									{
										MkByteArrayDescriptor descriptor(reinterpret_cast<const unsigned char*>(&md.type), 4);
										if (destBuffer.Overwrite(4 + 352 * j + 280, descriptor))
										{
											++success;
										}
										else
											break;
									}

									if (success == count)
									{
										MkPathName newBagPath;
										newBagPath.Reserve(100);
										newBagPath += bagDir;
										newBagPath += MkStr(md.type);
										newBagPath += L"[";
										newBagPath += md.name;
										newBagPath += L"].bag";

										MkInterfaceForFileWriting fwInterface;
										fwInterface.SetUp(newBagPath, true, false);
										if (fwInterface.Write(destBuffer, MkArraySection(0)) != fileSize)
										{
											failedLog.PushBack(newBagPath);
											failedList.PushBack(&md);
										}

										fwInterface.Clear();
									}
									else
									{
										failedLog.PushBack(L"count : " + MkStr(count) + L", success : " + MkStr(success));
										failedList.PushBack(&md);
									}
								}
								else
								{
									failedLog.PushBack(L"count : " + MkStr(count) + L", fileSize : " + MkStr(fileSize));
									failedList.PushBack(&md);
								}
							}
							else
							{
								failedLog.PushBack(L"fileSize : " + MkStr(fileSize));
								failedList.PushBack(&md);
							}
						}
					}
				}

				unsigned int success = m_TargetOriginMonsterCount - failedList.GetSize();
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"��ȯ ���� : " + MkStr(success) + L" / " + MkStr(m_TargetOriginMonsterCount), false);

				if (failedList.GetSize() > 0)
				{
					MK_DEV_PANEL.MsgToLog(L"(!!!) " + MkStr(failedList.GetSize()) + L" ���� origin ���ʹ� bag ���� ��ȯ ����", false);

					MK_INDEXING_LOOP(failedList, i)
					{
						OriginMonsterData& md = *failedList[i];
						MK_DEV_PANEL.MsgToLog(L"\t" + MkStr(md.type) + L"\t" + MkStr(md.name) + L" : " + failedLog[i], false);
					}
				}
				MK_DEV_PANEL.InsertEmptyLine();

				m_AppState = eAp_End;
			}
			break;

		case eAp_End:
			break;
		}
		
	}

	MainPage(const MkHashStr& name) : MkBasePage(name)
	{
		m_AppState = eAp_Start;
		m_TargetOriginMonsterCount = 0;
	}

	virtual ~MainPage() { Clear(); }

protected:

	eAppState m_AppState;

	MkHashMap<MkHashStr, MkArray<ClassicMonsterData> > m_ClassicMonsterData;
	MkHashMap<MkHashStr, MkArray<OriginMonsterData> > m_OriginMonsterData;

	MkHashMap<unsigned int, MkPathName> m_Bags;

	MkArray<ClassicMonsterData*> m_RestoredMonster;

	unsigned int m_TargetOriginMonsterCount;
};

//------------------------------------------------------------------------------------------------//

// MainFramework ����
class MainFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new MainPage(L"Root"));
		MK_PAGE_MGR.ChangePageDirectly(L"Root");
		
		return MkBaseFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	virtual ~MainFramework() {}
};

// Application ����
class Application : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new MainFramework; }

public:
	Application() : MkWin32Application() {}
	virtual ~Application() {}
};

//------------------------------------------------------------------------------------------------//

// ��Ʈ�� ����Ʈ������ Application ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	Application application;
	application.Run(hI, L"RohanSoundClassicToOriginConverter", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

