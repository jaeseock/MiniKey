#pragma once


//------------------------------------------------------------------------------------------------//
// single-thread용 bitmap texture 생성, 삭제 관리자
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPE_MkBaseTexture.h"


#define MK_BITMAP_POOL MkBitmapPool::GetInstance()


class MkPathName;

class MkBitmapPool
{
public:

	// bitmap texture를 로드 해 pool에 저장
	bool LoadBitmapTexture(const MkPathName& filePath);

	// bitmap texture 참조를 texture에 담아 반환
	// 이전 생성된 객체가 없으면 생성 후 반환
	MkBaseTexture* GetBitmapTexture(const MkPathName& filePath);
	MkBaseTexture* GetBitmapTexture(const MkHashStr& filePath);

	// 해당 bitmap texture를 pool에서 해제
	// (NOTE) 호출 전 해당 텍스쳐에 대한 외부 참조는 모두 해제된 상태이어야 함
	void UnloadBitmapTexture(const MkPathName& filePath);
	void UnloadBitmapTexture(const MkHashStr& filePath);

	// 예외 상황 무시하고 모두 해제
	void Clear(void);

	static MkBitmapPool& GetInstance(void);

	MkBitmapPool() {}
	virtual ~MkBitmapPool() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkBaseTexture*> m_Pool;
};
