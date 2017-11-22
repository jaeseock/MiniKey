
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
			return _ErrorMsg(historyDirPath, L"history 폴더가 존재하지 않습니다.");

		m_HistoryDirPath = targetDir;
	}

	{
		MkPathName targetDir;
		targetDir.ConvertToRootBasisAbsolutePath(sourceDirPath);
		if (!targetDir.CheckAvailable())
			return _ErrorMsg(sourceDirPath, L"source 폴더가 존재하지 않습니다.");

		m_SourceDirPath = targetDir;
	}

	m_HistoryList.Clear();
	MkArray<MkPathName> filePathList, extensionFilter;
	extensionFilter.PushBack(MkDataNode::DefaultFileExtension);
	m_HistoryDirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false); // 확장자로 필터링
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
		MK_DEV_PANEL.MsgToLog(L"> 기존 history 파일이 없습니다.");
	}
	else
	{
		m_HistoryList.SortInDescendingOrder();

		MK_DEV_PANEL.MsgToLog(L"> 기존 history 파일 : " + MkStr(m_HistoryList.GetSize()) + L" 개");
		MK_DEV_PANEL.MsgToLog(L"  마지막 history : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[0]));
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
		MK_DEV_PANEL.MsgToLog(MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[startIndex]) + L" 패치 내역 분석을 시작합니다.");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[startIndex]) + L" -> " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_HistoryList[endIndex]));
		MK_DEV_PANEL.MsgToLog(L"패치 내역 분석을 시작합니다.");
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
			return _ErrorMsg(historyFilePath, L"history 파일이 없습니다.");

		MkDataNode node;
		if (!node.Load(historyFilePath))
			return _ErrorMsg(historyFilePath, L"파일 로딩이 실패했습니다.");

		if (node.GetDataTypeTag() != MkPatchFileGenerator::DataType_PatchHistory)
			return _ErrorMsg(historyFilePath, L"파일은 정상적인 패치 히스토리가 아닙니다.");

		unsigned int lastWT = 0;
		node.GetData(MkPatchFileGenerator::KEY_LastWT, lastWT, 0);
		if ((prevWT != 0) && (prevWT != lastWT))
			return _ErrorMsg(MkPatchFileGenerator::ConvertWrittenTimeToStr(lastWT) + L" 패치 정보가 빠져 있습니다.");

		const MkDataNode* updateNode = node.GetChildNode(MkPatchFileGenerator::KEY_UpdateNode);
		if (updateNode == NULL)
			return _ErrorMsg(historyFilePath, L"파일에 " + MkPatchFileGenerator::KEY_UpdateNode.GetString() + L" node가 없습니다.");

		const MkDataNode* addNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_AddNode);
		const MkDataNode* repNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_ReplaceNode);
		const MkDataNode* delNode = updateNode->GetChildNode(MkPatchFileGenerator::KEY_DeleteNode);
		if ((addNode == NULL) && (repNode == NULL) && (delNode == NULL))
			return _ErrorMsg(historyFilePath, L"파일에 변경 항목이 없습니다.");

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
	MK_DEV_PANEL.MsgToLog(L"> 총 " + MkStr(m_SourceTable.GetSize()) + L" 개의 파일이 추가/변경되었습니다.");
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
		targetFullPath.DeleteCurrentDirectory(); // 삭제
	}

	MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
	MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
	MK_DEV_PANEL.MsgToLog(L"  경로에 파일 추출을 시작합니다.");
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
				MK_DEV_PANEL.MsgToLog(L"  파일 복사를 실패했습니다!!!");
			}
		}
		else
		{
			MK_DEV_PANEL.MsgToLog(L"  원본 파일이 존재하지 않습니다!!!");
		}
	}

	MK_DEV_PANEL.InsertEmptyLine();
	if (fileCount == success)
	{
		MK_DEV_PANEL.MsgToLog(L"> 총 " + MkStr(fileCount) + L" 개 파일 추출이 완료되었습니다.");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"> 총 " + MkStr(fileCount) + L" 개 파일 중" + MkStr(success) + L" 개만 추출되었습니다.");
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
