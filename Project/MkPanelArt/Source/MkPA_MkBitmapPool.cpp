
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkBitmapTexture.h"
#include "MkPA_MkBitmapPool.h"


#define MKDEF_BITMAPS_ON_POOL_MSG_TEXT L"텍스쳐 풀에 존재하는 비트맵 수 : "

//------------------------------------------------------------------------------------------------//

bool MkBitmapPool::LoadBitmapTexture(const MkPathName& filePath)
{
	MkHashStr currKey = filePath;

	if (m_Pool.Exist(currKey)) // 이미 존재. 무시
		return true;

	MK_CHECK(!filePath.Empty(), L"비어 있는 bitmap file 경로로 로딩 시도")
		return false;

	MK_CHECK(MkFileManager::CheckAvailable(filePath), L"존재하지 않는 bitmap file 로딩 시도 : " + filePath)
		return false;

	MkByteArray srcData;
	MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"bitmap data 로딩 실패 : " + filePath)
		return false;
	
	MkBitmapTexture* bitmapTex = new MkBitmapTexture;
	MK_CHECK(bitmapTex != NULL, L"bitmap alloc 실패")
		return false;

	MkPathName imgInfoPath = filePath;
	MkDataNode imgInfoNode;
	MkDataNode* imgInfoPtr = NULL;
	if (imgInfoPath.ChangeFileExtension(MKDEF_PA_IMAGE_INFO_FILE_EXT))
	{
		MkByteArray data;
		if (MkFileManager::GetFileData(imgInfoPath, data) && imgInfoNode.Load(data))
		{
			imgInfoPtr = &imgInfoNode;
		}
	}
	
	MK_CHECK(bitmapTex->SetUp(srcData, imgInfoPtr), filePath + L" bitmap 초기화 실패")
	{
		delete bitmapTex;
		return false;
	}

	bitmapTex->SetPoolKey(currKey);

	m_Pool.Create(currKey) = bitmapTex; // ref+

	const MkHashStr& groupName = bitmapTex->GetImageInfo().GetGroup();
	m_GroupTable.Create(groupName, currKey); // groupName이 empty일 수도 있음

	if (groupName.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"bitmap 생성 : " + filePath, true);
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"bitmap 생성(" + groupName.GetString() + L") : " + filePath, true);
	}

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_Pool.GetSize()));
	return true;
}

MkBaseTexture* MkBitmapPool::GetBitmapTexture(const MkPathName& filePath)
{
	// MkHashStr > MkPathName 변환은 문자열 복사만 일어나지만 MkPathName > MkHashStr 변환은 해쉬키 생성 비용이 추가됨
	return GetBitmapTexture(MkHashStr(filePath));
}

MkBaseTexture* MkBitmapPool::GetBitmapTexture(const MkHashStr& filePath)
{
	MkPathName path = filePath;
	MkHashStr key = path;

	// 없으면 생성
	if (!m_Pool.Exist(key))
	{
		if (!LoadBitmapTexture(path))
			return NULL;
	}
	
	// 참조 반환
	return m_Pool[key];
}

bool MkBitmapPool::UnloadBitmapTexture(const MkPathName& filePath)
{
	return UnloadBitmapTexture(MkHashStr(filePath));
}

bool MkBitmapPool::UnloadBitmapTexture(const MkHashStr& filePath)
{
	MkPathName path = filePath;
	MkHashStr key = path;

	if (m_Pool.Exist(key))
	{
		if (m_Pool[key].GetReferenceCounter() == 1)
		{
			m_Pool.Erase(key);

			MK_DEV_PANEL.MsgToLog(L"bitmap 삭제 : " + key.GetString(), true);
			MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_Pool.GetSize()));
			return true;
		}
		return false;
	}
	return true;
}

void MkBitmapPool::UnloadGroup(const MkHashStr& groupName)
{
	if (m_GroupTable.Exist(groupName))
	{
		MkMultiMapLooper<MkHashStr, MkHashStr> looper(m_GroupTable, groupName);
		MK_STL_LOOP(looper)
		{
			const MkHashStr& currKey = looper.GetCurrentField();
			
			MK_CHECK(m_Pool[currKey].GetReferenceCounter() == 1, L"<WARNING> 참조 중인 bitmap texture " + currKey.GetString() + L"를 삭제 시도")
				continue;

			m_Pool.Erase(currKey);

			MK_DEV_PANEL.MsgToLog(L"bitmap 삭제 : " + currKey.GetString(), true);
		}

		m_GroupTable.Erase(groupName);

		MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_Pool.GetSize()));
	}
}

void MkBitmapPool::Clear(void)
{
	m_Pool.Clear();
	m_GroupTable.Clear();

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_Pool.GetSize()));
}

//------------------------------------------------------------------------------------------------//
