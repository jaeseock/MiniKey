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
#include "MkCore_MkPathName.h"

#include "MkS2D_MkBaseResetableResource.h"
#include "MkS2D_MkBaseTexture.h"


#define MK_CURSOR_MGR MkCursorManager::Instance()


class MkDataNode;

class MkCursorManager : public MkBaseResetableResource, public MkSingletonPattern<MkCursorManager>
{
public:

	// MkDataNode�� Ŀ�� ����Ʈ ���
	void RegisterCursor(const MkDataNode& node);

	// Ŀ�� ����Ʈ ���
	bool RegisterCursor(const MkHashStr& key, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY);

	// set/get delay mode
	void SetDelayMode(bool enable);
	inline bool GetDelayMode(void) const { return m_DelayMode; }

	// ���� Ŀ�� ����
	// deley mode�� commit ������� ��ϵǰ� �ƴϸ� �ٷ� ���� ��
	bool SetCurrentCursor(const MkHashStr& key);

	// ���� Ŀ���� �Ϲ�/�ɵ�/�ź� Ŀ�� ����
	inline void SetNormalCursor(void) { SetCurrentCursor(m_NormalCursor); }
	inline void SetActionCursor(void) { SetCurrentCursor(m_ActionCursor); }
	inline void SetDisableCursor(void) { SetCurrentCursor(m_DisableCursor); }

	// delay mode�� ������ Ŀ�� �ݿ�
	bool CommitCurrentCursor(void);

	// ���� Ŀ�� ��ȯ
	inline const MkHashStr& GetCurrentCursor(void) const { return m_CurrentCursor; }

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
