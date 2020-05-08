
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkBitmapTexture.h"
#include "MkPA_MkBitmapPool.h"


#define MKDEF_BITMAPS_ON_POOL_MSG_TEXT L"�ؽ��� Ǯ�� �����ϴ� ��Ʈ�� �� : "

//------------------------------------------------------------------------------------------------//

bool MkBitmapPool::LoadBitmapTexture(const MkPathName& filePath)
{
	MkHashStr currKey = filePath;

	if (m_Pool.Exist(currKey)) // �̹� ����. ����
		return true;

	MK_CHECK(!filePath.Empty(), L"��� �ִ� bitmap file ��η� �ε� �õ�")
		return false;

	MK_CHECK(MkFileManager::CheckAvailable(filePath), L"�������� �ʴ� bitmap file �ε� �õ� : " + filePath)
		return false;

	MkByteArray srcData;
	MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"bitmap data �ε� ���� : " + filePath)
		return false;
	
	MkBitmapTexture* bitmapTex = new MkBitmapTexture;
	MK_CHECK(bitmapTex != NULL, L"bitmap alloc ����")
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
	
	MK_CHECK(bitmapTex->SetUp(srcData, imgInfoPtr), filePath + L" bitmap �ʱ�ȭ ����")
	{
		delete bitmapTex;
		return false;
	}

	bitmapTex->SetPoolKey(currKey);

	m_Pool.Create(currKey) = bitmapTex; // ref+

	const MkHashStr& groupName = bitmapTex->GetImageInfo().GetGroup();
	m_GroupTable.Create(groupName, currKey); // groupName�� empty�� ���� ����

	if (groupName.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"bitmap ���� : " + filePath, true);
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"bitmap ����(" + groupName.GetString() + L") : " + filePath, true);
	}

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_Pool.GetSize()));
	return true;
}

MkBaseTexture* MkBitmapPool::GetBitmapTexture(const MkPathName& filePath)
{
	// MkHashStr > MkPathName ��ȯ�� ���ڿ� ���縸 �Ͼ���� MkPathName > MkHashStr ��ȯ�� �ؽ�Ű ���� ����� �߰���
	return GetBitmapTexture(MkHashStr(filePath));
}

MkBaseTexture* MkBitmapPool::GetBitmapTexture(const MkHashStr& filePath)
{
	MkPathName path = filePath;
	MkHashStr key = path;

	// ������ ����
	if (!m_Pool.Exist(key))
	{
		if (!LoadBitmapTexture(path))
			return NULL;
	}
	
	// ���� ��ȯ
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

			MK_DEV_PANEL.MsgToLog(L"bitmap ���� : " + key.GetString(), true);
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
			
			MK_CHECK(m_Pool[currKey].GetReferenceCounter() == 1, L"<WARNING> ���� ���� bitmap texture " + currKey.GetString() + L"�� ���� �õ�")
				continue;

			m_Pool.Erase(currKey);

			MK_DEV_PANEL.MsgToLog(L"bitmap ���� : " + currKey.GetString(), true);
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
