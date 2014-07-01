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

#include "MkS2D_MkBaseTexture.h"


#define MK_TEXTURE_POOL MkTexturePool::Instance()


class MkPathName;

class MkTexturePool : public MkSingletonPattern<MkTexturePool>
{
public:

	// bitmap texture�� �ε� �� pool�� ����
	bool LoadBitmapTexture(const MkPathName& filePath);

	// bitmap texture ������ texture�� ��� ��ȯ
	// ���� ������ ��ü�� ������ ���� �� ��ȯ
	void GetBitmapTexture(const MkPathName& filePath, MkBaseTexturePtr& texture);

	// �ش� group�� ���� bitmap texture���� pool���� ����
	// (NOTE) ȣ�� �� �ش� �ؽ��ĵ鿡 ���� �ܺ� ������ ��� ������ �����̾�� ��
	void UnloadGroup(unsigned int group);

	// ���� ��Ȳ �����ϰ� ��� ����
	void Clear(void);

	MkTexturePool() : MkSingletonPattern<MkTexturePool>() {}
	virtual ~MkTexturePool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseTexturePtr> m_BitmapPool;
	MkMultiMap<unsigned int, MkHashStr> m_GroupTable;
};
