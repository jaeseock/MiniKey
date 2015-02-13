#pragma once


//------------------------------------------------------------------------------------------------//
// bitmap texture ����, ���� ������
// - thread�̽��� �������(single-thread) �ʱ�ȭ/���� ��ġ�� ���� �Ű� ���� �ʾƵ� �ǹǷ� pooling�� �����ϸ� ��
// - render to texture�� pooling�� �ʿ伺�� �����Ƿ� �˾Ƽ� �ؾ� ��
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMultiMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"

#include "MkPA_MkBaseTexture.h"


#define MK_BITMAP_POOL MkBitmapPool::Instance()


class MkPathName;

class MkBitmapPool : public MkSingletonPattern<MkBitmapPool>
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

	// �ش� group�� ���� bitmap texture���� pool���� ����
	// (NOTE) ȣ�� �� �ش� �ؽ��ĵ鿡 ���� �ܺ� ������ ��� ������ �����̾�� ��
	void UnloadGroup(const MkHashStr& groupName);

	// ���� ��Ȳ �����ϰ� ��� ����
	void Clear(void);

	MkBitmapPool() : MkSingletonPattern<MkBitmapPool>() {}
	virtual ~MkBitmapPool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseTexturePtr> m_Pool;
	MkMultiMap<MkHashStr, MkHashStr> m_GroupTable;
};
