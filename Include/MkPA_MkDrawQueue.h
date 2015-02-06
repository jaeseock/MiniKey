#pragma once


//------------------------------------------------------------------------------------------------//
// draw queue
// - 순서를 가진 draw step의 집합
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkPA_MkRenderTarget.h"


class MkDrawStepInterface;
class MkDrawSceneNodeStep;

class MkDrawQueue
{
public:

	// DrawStep 생성
	// stepName : step 이름
	// priority : 그리기 우선순위. 낮은 순서부터 먼저 그림. 음수일 경우 기존 step이 없으면 0, 아니면 마지막에 등록된 step의 다음 순위를 가짐
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type이 eTexture일 경우 최대 채널 수
	// size : type이 eTexture일 경우 생성 할 render to texture의 크기
	// camOffset : camera 위치 설정시의 offset
	// texFormat : type이 eTexture일 경우 생성 할 render to texture의 포맷
	MkDrawSceneNodeStep* CreateDrawSceneNodeStep(
		const MkHashStr& stepName,
		int priority = -1,
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 1,
		const MkInt2& size = MkInt2(0, 0),
		const MkFloat2& camOffset = MkFloat2(0.f, 0.f),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// step 반환
	MkDrawStepInterface* GetStep(const MkHashStr& stepName);

	// step 삭제
	void RemoveStep(const MkHashStr& stepName);

	// 그리기
	bool Draw(void);

	// 해제
	void Clear(void);

	MkDrawQueue() {}
	~MkDrawQueue() { Clear(); }

protected:

	MkHashMap<MkHashStr, int> m_NameTable;
	MkMap<int, MkDrawStepInterface*> m_Steps;
};
