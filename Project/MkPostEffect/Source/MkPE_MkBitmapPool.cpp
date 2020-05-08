
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkFileManager.h"

#include "MkPE_MkBitmapTexture.h"
#include "MkPE_MkBitmapPool.h"


//------------------------------------------------------------------------------------------------//

bool MkBitmapPool::LoadBitmapTexture(const MkPathName& filePath)
{
	MkHashStr currKey = filePath;

	if (m_Pool.Exist(currKey)) // 이미 존재. 무시
		return true;

	MK_CHECK(!filePath.Empty(), L"비어 있는 bitmap file 경로로 로딩 시도")
		return false;

	MkByteArray srcData;
	MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"bitmap data 로딩 실패 : " + filePath)
		return false;
	
	MkBitmapTexture* bitmapTex = new MkBitmapTexture;
	MK_CHECK(bitmapTex != NULL, L"bitmap alloc 실패")
		return false;

	MK_CHECK(bitmapTex->SetUp(srcData), filePath + L" bitmap 초기화 실패")
	{
		delete bitmapTex;
		return false;
	}

	bitmapTex->SetPoolKey(currKey);

	m_Pool.Create(currKey, bitmapTex);
	return true;
}

MkBaseTexture* MkBitmapPool::GetBitmapTexture(const MkPathName& filePath)
{
	return GetBitmapTexture(MkHashStr(filePath)); // 해쉬키 생성
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

void MkBitmapPool::UnloadBitmapTexture(const MkPathName& filePath)
{
	UnloadBitmapTexture(MkHashStr(filePath)); // 해쉬키 생성
}

void MkBitmapPool::UnloadBitmapTexture(const MkHashStr& filePath)
{
	MkPathName path = filePath;
	MkHashStr key = path;

	if (m_Pool.Exist(key))
	{
		delete m_Pool[key];
		m_Pool.Erase(key);
	}
}

void MkBitmapPool::Clear(void)
{
	MkHashMapLooper<MkHashStr, MkBaseTexture*> looper(m_Pool);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Pool.Clear();
}

MkBitmapPool& MkBitmapPool::GetInstance(void)
{
	static MkBitmapPool instance;
	return instance;
}

//------------------------------------------------------------------------------------------------//
