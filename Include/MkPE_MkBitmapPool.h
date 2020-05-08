#pragma once


//------------------------------------------------------------------------------------------------//
// single-thread�� bitmap texture ����, ���� ������
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPE_MkBaseTexture.h"


#define MK_BITMAP_POOL MkBitmapPool::GetInstance()


class MkPathName;

class MkBitmapPool
{
public:

	// bitmap texture�� �ε� �� pool�� ����
	bool LoadBitmapTexture(const MkPathName& filePath);

	// bitmap texture ������ texture�� ��� ��ȯ
	// ���� ������ ��ü�� ������ ���� �� ��ȯ
	MkBaseTexture* GetBitmapTexture(const MkPathName& filePath);
	MkBaseTexture* GetBitmapTexture(const MkHashStr& filePath);

	// �ش� bitmap texture�� pool���� ����
	// (NOTE) ȣ�� �� �ش� �ؽ��Ŀ� ���� �ܺ� ������ ��� ������ �����̾�� ��
	void UnloadBitmapTexture(const MkPathName& filePath);
	void UnloadBitmapTexture(const MkHashStr& filePath);

	// ���� ��Ȳ �����ϰ� ��� ����
	void Clear(void);

	static MkBitmapPool& GetInstance(void);

	MkBitmapPool() {}
	virtual ~MkBitmapPool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseTexture*> m_Pool;
};
