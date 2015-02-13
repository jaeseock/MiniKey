#pragma once


//------------------------------------------------------------------------------------------------//
// cursor(pointer) 관리자
//
// 입력 모드로 delay mode가,
// - true면 Set...Cursor() 호출시 바로 변경되는 것이 아니라 대상으로 등록되었다가 CommitCurrentCursor() 호출시 반영 됨
// - false면 Set...Cursor() 호출시 바로 변경
// - 기본은 true 상태
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

#include "MkPA_MkBaseResetableResource.h"


#define MK_CURSOR_MGR MkCursorManager::Instance()


class MkDataNode;

class MkCursorManager : public MkBaseResetableResource, public MkSingletonPattern<MkCursorManager>
{
public:

	enum eCursorType
	{
		eNone = -1,
		eNormal = 0,
		eExcutable,
		eDisable,
		eMovable,
		eMoving,
		eTextCursor,

		eMaxType
	};

public:

	// MkDataNode로 커서 리스트 등록
	void SetUp(const MkDataNode* node);

	// 수동 커서 등록
	bool RegisterCursor(eCursorType cursorType, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY);

	// 현재 커서 지정
	bool SetCursorType(eCursorType cursorType);

	// 현재 커서 반환
	inline eCursorType GetCursorType(void) const { return m_CurrentCursorType; }

	// 해제
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkBaseResetableResource
	//------------------------------------------------------------------------------------------------//

	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	MkCursorManager();
	virtual ~MkCursorManager() { Clear(); }

	void __ConsumeSetCursorMsg(void);

protected:

	typedef struct __CursorInfo
	{
		MkHashStr filePath;
		unsigned int hotspotX;
		unsigned int hotspotY;
	}
	_CursorInfo;

protected:

	MkHashMap<eCursorType, _CursorInfo> m_CursorInfoTable;

	eCursorType m_CurrentCursorType;
};
