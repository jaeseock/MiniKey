#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkS2D_MkDrawStep.h"


class MkDrawQueue
{
public:

	// DrawStep 생성
	// stepName : step 이름
	// priority : 그리기 우선순위. 낮은 순서부터 먼저 그림. 음수일 경우 기존 step이 없으면 0, 아니면 마지막에 등록된 step의 다음 순위를 가짐
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type이 eTexture일 경우 최대 채널 수
	// size : type이 eTexture일 경우 생성 할 render to texture의 크기
	// size : type이 eTexture일 경우 생성 할 render to texture의 포맷
	MkDrawStep* CreateStep(
		const MkHashStr& stepName,
		int priority = -1,
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 0,
		const MkUInt2& size = MkUInt2(0, 0),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// step 반환
	MkDrawStep* GetStep(const MkHashStr& stepName);

	// step 삭제
	bool RemoveStep(const MkHashStr& stepName);

	// 그리기
	bool Draw(void);

	// 해제
	void Clear(void);

	MkDrawQueue() {}
	~MkDrawQueue() { Clear(); }

protected:

	MkHashMap<MkHashStr, int> m_NameTable;
	MkMap<int, MkDrawStep*> m_Steps;
};
