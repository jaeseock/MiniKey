#pragma once


//------------------------------------------------------------------------------------------------//
// cursor(pointer) ������
//
// �Է� ���� delay mode��,
// - true�� Set...Cursor() ȣ��� �ٷ� ����Ǵ� ���� �ƴ϶� ������� ��ϵǾ��ٰ� CommitCurrentCursor() ȣ��� �ݿ� ��
// - false�� Set...Cursor() ȣ��� �ٷ� ����
// - �⺻�� true ����
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

	// MkDataNode�� Ŀ�� ����Ʈ ���
	void SetUp(const MkDataNode* node);

	// ���� Ŀ�� ���
	bool RegisterCursor(eCursorType cursorType, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY);

	// ���� Ŀ�� ����
	bool SetCursorType(eCursorType cursorType);

	// ���� Ŀ�� ��ȯ
	inline eCursorType GetCursorType(void) const { return m_CurrentCursorType; }

	// ����
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
