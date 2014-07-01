#pragma once


//------------------------------------------------------------------------------------------------//
// bitmap texture 생성, 삭제 관리자
// - thread이슈가 사라지면(single-thread) 초기화/종료 위치에 대해 신경 쓰지 않아도 되므로 pooling만 관리하면 됨
// - render to texture는 pooling의 필요성이 없으므로 알아서 해야 함
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

	// bitmap texture를 로드 해 pool에 저장
	bool LoadBitmapTexture(const MkPathName& filePath);

	// bitmap texture 참조를 texture에 담아 반환
	// 이전 생성된 객체가 없으면 생성 후 반환
	void GetBitmapTexture(const MkPathName& filePath, MkBaseTexturePtr& texture);

	// 해당 group에 속한 bitmap texture들을 pool에서 해제
	// (NOTE) 호출 전 해당 텍스쳐들에 대한 외부 참조는 모두 해제된 상태이어야 함
	void UnloadGroup(unsigned int group);

	// 예외 상황 무시하고 모두 해제
	void Clear(void);

	MkTexturePool() : MkSingletonPattern<MkTexturePool>() {}
	virtual ~MkTexturePool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseTexturePtr> m_BitmapPool;
	MkMultiMap<unsigned int, MkHashStr> m_GroupTable;
};
