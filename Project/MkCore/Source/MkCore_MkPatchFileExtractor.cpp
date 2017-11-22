
//#include <atltime.h>
#include "MkCore_MkDevPanel.h"
//#include "MkCore_MkInterfaceForFileWriting.h"
//#include "MkCore_MkInterfaceForFileReading.h"
//#include "MkCore_MkFileSystem.h"
//#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileExtractor.h"

//------------------------------------------------------------------------------------------------//

bool MkPatchFileExtractor::SetDirectoryPath(const MkPathName& historyDirPath, const MkPathName& sourceDirPath)
{
	{
		MkPathName targetDir;
		targetDir.ConvertToRootBasisAbsolutePath(historyDirPath);
		if (!targetDir.CheckAvailable())
			return _ErrorMsg(historyDirPath, L"history ������ �������� �ʽ��ϴ�.");

		m_HistoryDirPath = targetDir;
	}

	{
		MkPathName targetDir;
		targetDir.ConvertToRootBasisAbsolutePath(sourceDirPath);
		if (!targetDir.CheckAvailable())
			return _ErrorMsg(sourceDirPath, L"source ������ �������� �ʽ��ϴ�.");

		m_SourceDirPath = targetDir;
	}

	m_HistoryList.Clear();
	MkArray<MkPathName> filePathList, extensionFilter;
	extensionFilter.PushBack(MkDataNode::DefaultFileExtension);
	m_HistoryDirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false); // Ȯ���ڷ� ���͸�
	m_HistoryList.Reserve(filePathList.GetSize());

	MK_INDEXING_LOOP(filePathList, i)
	{
		MkPathName currName = filePathList[i].GetFileName(false);
		if (currName.IsDigit())
		{
			m_HistoryList.PushBack(currName.ToUnsignedInteger());
		}
	}
	
	if (m_HistoryList.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"> ���� history ������ �����ϴ�.");
	}
	else
	{
		m_HistoryList.SortInDescendingOrder();

		MK_DEV_PANEL.MsgToLog(L"> ���� history ���� : " + MkStr(m_HistoryList.GetSize()) + L" ��");
		MK_DEV_PANEL.MsgToLog(L"  ������ history : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[0]));
	}
	MK_DEV_PANEL.InsertEmptyLine();
	return true;
}

bool MkPatchFileExtractor::MakeUpdateHistory(unsigned int startIndex, unsigned int endIndex)
{
	m_SourceTable.Clear();

	if (m_HistoryDirPath.Empty())
		return false;

	if (!m_HistoryList.IsValidIndex(startIndex))
		return false;

	if (!m_HistoryList.IsValidIndex(endIndex))
		return false;

	if (startIndex < endIndex)
		return false;

	MkArray<unsigned int> targetList;
	if (!m_HistoryList.GetSubArray(MkArraySection(endIndex, startIndex - endIndex + 1), targetList))
		return false;

	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	if (startIndex == endIndex)
	{
		MK_DEV_PANEL.MsgToLog(MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[startIndex]) + L" ��ġ ���� �м��� �����մϴ�.");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[startIndex]) + L" -> " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[endIndex]));
		MK_DEV_PANEL.MsgToLog(L"��ġ ���� �м��� �����մϴ�.");
	}
	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");

	unsigned int prevWT = 0;
	MK_INDEXING_RLOOP(targetList, i)
	{
		unsigned int currWT = targetList[i];
		
		MkPathName historyFilePath = m_HistoryDirPath;
		historyFilePath += currWT;
		historyFilePath += L".";
		historyFilePath += MkDataNode::DefaultFileExtension;

		if (!historyFilePath.CheckAvailable())
			return _ErrorMsg(historyFilePath, L"history ������ �����ϴ�.");

		MkDataNode node;
		if (!node.Load(historyFilePath))
			return _ErrorMsg(historyFilePath, L"���� �ε��� �����߽��ϴ�.");

		if (node.GetDataTypeTag() != MkPatchFileGenerator::DataType_PatchHistory)
			return _ErrorMsg(historyFilePath, L"������ �������� ��ġ �����丮�� �ƴմϴ�.");

		unsigned int lastWT = 0;
		node.GetData(MkPatchFileGenerator::KEY_LastWT, lastWT, 0);
		if ((prevWT != 0) && (prevWT != lastWT))
			return _ErrorMsg(MkPatchFileGenerator::ConvertWrittenTimeToStr(lastWT) + L" ��ġ ������ ���� �ֽ��ϴ�.");

		const MkDataNode* updateNode = node.GetChildNode(MkPatchFileGenerator::KEY_UpdateNode);
		if (updateNode == NULL)
			return _ErrorMsg(historyFilePath, L"���Ͽ� " + MkPatchFileGenerator::KEY_UpdateNode.GetString() + L" node�� �����ϴ�.");

		const MkDataNode* addNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_AddNode);
		const MkDataNode* repNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_ReplaceNode);
		const MkDataNode* delNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_DeleteNode);
		if ((addNode == NULL) && (repNode == NULL) && (delNode == NULL))
			return _ErrorMsg(historyFilePath, L"���Ͽ� ���� �׸��� �����ϴ�.");

		MK_DEV_PANEL.MsgToLog(L"[ " + MkPatchFileGenerator::ConvertWrittenTimeToStr(currWT) + L" ]");

		if (delNode != NULL)
		{
			MkArray<MkHashStr> keys;
			delNode->GetChildNodeList(keys);
			MK_INDEXING_LOOP(keys, j)
			{
				const MkHashStr& currKey = keys[j];
				if (m_SourceTable.Exist(currKey))
				{
					m_SourceTable.Erase(currKey);
				}
				MK_DEV_PANEL.MsgToLog(L"  - " + currKey.GetString());
			}
		}

		if (addNode != NULL)
		{
			MkArray<MkHashStr> keys;
			addNode->GetChildNodeList(keys);
			MK_INDEXING_LOOP(keys, j)
			{
				const MkHashStr& currKey = keys[j];
				if (m_SourceTable.Exist(currKey))
				{
					m_SourceTable[currKey] = currWT;
				}
				else
				{
					m_SourceTable.Create(currKey, currWT);
				}
				MK_DEV_PANEL.MsgToLog(L"  + " + currKey.GetString());
			}
		}

		if (repNode != NULL)
		{
			MkArray<MkHashStr> keys;
			repNode->GetChildNodeList(keys);
			MK_INDEXING_LOOP(keys, j)
			{
				const MkHashStr& currKey = keys[j];
				if (m_SourceTable.Exist(currKey))
				{
					m_SourceTable[currKey] = currWT;
				}
				else
				{
					m_SourceTable.Create(currKey, currWT);
				}
				MK_DEV_PANEL.MsgToLog(L"  * " + currKey.GetString());
			}
		}

		MK_DEV_PANEL.InsertEmptyLine();
	}

	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	MK_DEV_PANEL.MsgToLog(L"> �� " + MkStr(m_SourceTable.GetSize()) + L" ���� ������ �߰�/����Ǿ����ϴ�.");
	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	MK_DEV_PANEL.InsertEmptyLine();
	
	if (m_SourceTable.Empty())
		return false;

	MkHashMapLooper<MkHashStr, unsigned int> looper(m_SourceTable);
	MK_STL_LOOP(looper)
	{
		MK_DEV_PANEL.MsgToLog(looper.GetCurrentKey().GetString());
		MK_DEV_PANEL.MsgToLog(L"  > " + MkPatchFileGenerator::ConvertWrittenTimeToStr(looper.GetCurrentField()));
		MK_DEV_PANEL.InsertEmptyLine();
	}

	return true;
}

bool MkPatchFileExtractor::Extract(const MkPathName& targetDirPath)
{
	if (m_SourceTable.Empty())
		return false;

	if (m_SourceDirPath.Empty())
		return false;

	MkPathName targetFullPath;
	targetFullPath.ConvertToRootBasisAbsolutePath(targetDirPath);
	if (targetFullPath.CheckAvailable())
	{
		targetFullPath.DeleteCurrentDirectory(); // ����
	}

	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
	MK_DEV_PANEL.MsgToLog(L"  ��ο� ���� ������ �����մϴ�.");
	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");

	unsigned int fileCount = m_SourceTable.GetSize();
	unsigned int success = 0;

	MkHashMapLooper<MkHashStr, unsigned int> looper(m_SourceTable);
	MK_STL_LOOP(looper)
	{
		MkPathName srcFilePath = m_SourceDirPath;
		srcFilePath += looper.GetCurrentKey().GetString();

		MK_DEV_PANEL.MsgToLog(L"> " + looper.GetCurrentKey().GetString());

		if (srcFilePath.CheckAvailable())
		{
			MkPathName destFilePath = targetFullPath;
			destFilePath += looper.GetCurrentKey().GetString();

			if (srcFilePath.CopyCurrentFile(destFilePath))
			{
				++success;
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"  ���� ���縦 �����߽��ϴ�!!!");
			}
		}
		else
		{
			MK_DEV_PANEL.MsgToLog(L"  ���� ������ �������� �ʽ��ϴ�!!!");
		}
	}

	MK_DEV_PANEL.InsertEmptyLine();
	if (fileCount == success)
	{
		MK_DEV_PANEL.MsgToLog(L"> �� " + MkStr(fileCount) + L" �� ���� ������ �Ϸ�Ǿ����ϴ�.");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"> �� " + MkStr(fileCount) + L" �� ���� ��" + MkStr(success) + L" ���� ����Ǿ����ϴ�.");
	}
	MK_DEV_PANEL.InsertEmptyLine();

	targetFullPath.OpenDirectoryInExplorer();
	return true;
}

bool MkPatchFileExtractor::_ErrorMsg(const MkStr& msg) const
{
	MK_DEV_PANEL.MsgToLog(L"> " + msg);
	MK_DEV_PANEL.InsertEmptyLine();
	return false;
}

bool MkPatchFileExtractor::_ErrorMsg(const MkStr& filePath, const MkStr& msg) const
{
	MK_DEV_PANEL.MsgToLog(L"> " + filePath);
	MK_DEV_PANEL.MsgToLog(L"  " + msg);
	MK_DEV_PANEL.InsertEmptyLine();
	return false;
}

//------------------------------------------------------------------------------------------------//
