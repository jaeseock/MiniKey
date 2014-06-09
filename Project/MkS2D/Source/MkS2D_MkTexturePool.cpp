
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkBitmapTexture.h"
#include "MkS2D_MkTexturePool.h"


#define MKDEF_BITMAPS_ON_POOL_MSG_TEXT L"�ؽ��� Ǯ�� �����ϴ� ��Ʈ�� �� : "

//------------------------------------------------------------------------------------------------//

bool MkTexturePool::LoadBitmapTexture(const MkPathName& filePath, unsigned int group)
{
	MkHashStr currKey = filePath;

	if (m_BitmapPool.Exist(currKey)) // �̹� ����. ����
		return true;

	MK_CHECK(MkFileManager::CheckAvailable(filePath), L"�������� �ʴ� bitmap file �ε� �õ� : " + filePath)
		return false;

	MkByteArray srcData;
	MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"bitmap data �ε� ���� : " + filePath)
		return false;
	
	MkBitmapTexture* bitmapTex = new MkBitmapTexture;
	MK_CHECK(bitmapTex != NULL, L"bitmap alloc ����")
		return false;

	MkPathName imgSubsetPath = filePath;
	MkDataNode imgSubsetNode;
	MkDataNode* imgSubsetPtr = NULL;
	if (imgSubsetPath.ChangeFileExtension(MKDEF_S2D_IMAGE_SUBSET_FILE_EXTENSION))
	{
		MkByteArray subsetData;
		if (MkFileManager::GetFileData(imgSubsetPath, subsetData) && imgSubsetNode.Load(subsetData))
		{
			imgSubsetPtr = &imgSubsetNode;
		}
	}
	
	MK_CHECK(bitmapTex->SetUp(srcData, imgSubsetPtr), filePath + L" bitmap �ʱ�ȭ ����")
	{
		delete bitmapTex;
		return false;
	}

	bitmapTex->SetPoolKey(currKey);
	bitmapTex->SetBitmapGroup(group);

	m_BitmapPool.Create(currKey) = bitmapTex; // ref+

	m_GroupTable.Create(group, currKey);

	MK_DEV_PANEL.MsgToLog(L"> MkTexturePool > bitmap ���� : " + filePath, true);
	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_BitmapPool.GetSize()));
	return true;
}

void MkTexturePool::GetBitmapTexture(const MkPathName& filePath, MkBaseTexturePtr& texture, unsigned int group)
{
	MK_CHECK(texture == NULL, L"�����Ϸ��� texture ptr�� �̹� ���� ��� ����")
		return;

	MkHashStr currKey = filePath;

	// ������ ����
	if (!m_BitmapPool.Exist(currKey))
	{
		if (!LoadBitmapTexture(filePath, group))
			return;
	}
	
	// ���� ��ȯ
	texture = m_BitmapPool[currKey];
}

void MkTexturePool::UnloadGroup(unsigned int group)
{
	if (m_GroupTable.Exist(group))
	{
		MkMultiMapLooper<unsigned int, MkHashStr> looper(m_GroupTable, group);
		MK_STL_LOOP(looper)
		{
			const MkHashStr& currKey = looper.GetCurrentField();
			
			MK_CHECK(m_BitmapPool[currKey].GetReferenceCounter() == 1, L"<WARNING> ���� ���� bitmap texture " + currKey.GetString() + L"�� ����") {}

			m_BitmapPool.Erase(currKey);

			MK_DEV_PANEL.MsgToLog(L"> MkTexturePool > bitmap ���� : " + currKey.GetString(), true);
			
		}

		m_GroupTable.Erase(group);

		MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_BitmapPool.GetSize()));
	}
}

void MkTexturePool::Clear(void)
{
	m_BitmapPool.Clear();
	m_GroupTable.Clear();

	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL, MKDEF_BITMAPS_ON_POOL_MSG_TEXT + MkStr(m_BitmapPool.GetSize()));
}

//------------------------------------------------------------------------------------------------//
