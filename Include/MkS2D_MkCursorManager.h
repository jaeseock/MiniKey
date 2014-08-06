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
#include "MkCore_MkPathName.h"

#include "MkS2D_MkBaseResetableResource.h"
#include "MkS2D_MkBaseTexture.h"


#define MK_CURSOR_MGR MkCursorManager::Instance()


class MkDataNode;

class MkCursorManager : public MkBaseResetableResource, public MkSingletonPattern<MkCursorManager>
{
public:

	// MkDataNode로 커서 리스트 등록
	void RegisterCursor(const MkDataNode& node);

	// 커서 리스트 등록
	bool RegisterCursor(const MkHashStr& key, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY);

	// set/get delay mode
	void SetDelayMode(bool enable);
	inline bool GetDelayMode(void) const { return m_DelayMode; }

	// 현재 커서 지정
	// deley mode면 commit 대상으로 등록되고 아니면 바로 변경 함
	bool SetCurrentCursor(const MkHashStr& key);

	// 현재 커서로 일반/능동/거부 커서 지정
	inline void SetNormalCursor(void) { SetCurrentCursor(m_NormalCursor); }
	inline void SetActionCursor(void) { SetCurrentCursor(m_ActionCursor); }
	inline void SetDisableCursor(void) { SetCurrentCursor(m_DisableCursor); }

	// delay mode시 프레임 커서 반영
	bool CommitCurrentCursor(void);

	// 현재 커서 반환
	inline const MkHashStr& GetCurrentCursor(void) const { return m_CurrentCursor; }

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

	void _LoadCursor(const MkDataNode& node, const MkHashStr& dataKey, MkHashStr& targetName);

	bool _SetCurrentCursor(const MkHashStr& key);

	typedef struct __CursorInfo
	{
		MkPathName filePath;
		unsigned int hotspotX;
		unsigned int hotspotY;
	}
	_CursorInfo;

protected:

	MkHashMap<MkHashStr, _CursorInfo> m_CursorInfoTable;

	bool m_DelayMode;

	MkHashStr m_CurrentCursor;
	MkBaseTexturePtr m_CurrentTexture;

	MkHashStr m_CursorOnFrame;
	MkHashStr m_NormalCursor;
	MkHashStr m_ActionCursor;
	MkHashStr m_DisableCursor;
};
