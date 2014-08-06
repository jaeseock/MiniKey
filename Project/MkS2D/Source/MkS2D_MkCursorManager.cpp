
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkCursorManager.h"


//------------------------------------------------------------------------------------------------//

void MkCursorManager::RegisterCursor(const MkDataNode& node)
{
	MkArray<MkHashStr> cursorList;
	if (node.GetChildNodeList(cursorList) > 0)
	{
		const MkHashStr filePathKey = L"FilePath";
		const MkHashStr hotspotKey = L"Hotspot";

		MK_INDEXING_LOOP(cursorList, i)
		{
			const MkDataNode& targetNode = *node.GetChildNode(cursorList[i]);

			MkStr fp;
			MkInt2 hs;
			if (targetNode.GetData(filePathKey, fp, 0) && targetNode.GetData(hotspotKey, hs, 0))
			{
				RegisterCursor(cursorList[i], fp, static_cast<unsigned int>(hs.x), static_cast<unsigned int>(hs.y));
			}
		}

		_LoadCursor(node, L"NormalCursor", m_NormalCursor);
		_LoadCursor(node, L"ActionCursor", m_ActionCursor);
		_LoadCursor(node, L"DisableCursor", m_DisableCursor);

		SetNormalCursor();
		CommitCurrentCursor();
	}
}

void MkCursorManager::SetDelayMode(bool enable)
{
	if ((!m_DelayMode) && enable) // off -> on
	{
		m_CursorOnFrame.Clear();
	}

	m_DelayMode = enable;
}

bool MkCursorManager::RegisterCursor(const MkHashStr& key, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY)
{
	MK_CHECK(!key.Empty(), L"cursor로 비어 있는 key 등록 시도")
		return false;

	MK_CHECK(!m_CursorInfoTable.Exist(key), L"cursor로 이미 존재하는 key 등록 시도 : " + key.GetString())
		return false;

	MK_CHECK(MkFileManager::CheckAvailable(filePath), L"cursor로 존재하지 않는 이미지 등록 시도 : " + key.GetString() + L", " + filePath)
		return false;

	MK_CHECK(MK_TEXTURE_POOL.LoadBitmapTexture(filePath), L"cursor로 적합하지 않은 이미지 등록 시도 : " + key.GetString() + L", " + filePath)
		return false;

	_CursorInfo& ci = m_CursorInfoTable.Create(key);
	ci.filePath = filePath;
	ci.hotspotX = hotspotX;
	ci.hotspotY = hotspotY;

	MK_DEV_PANEL.MsgToLog(L"> cursor 등록 : " + key.GetString(), true);
	return true;
}

bool MkCursorManager::SetCurrentCursor(const MkHashStr& key)
{
	if (m_CursorInfoTable.Exist(key))
	{
		if (m_DelayMode)
		{
			m_CursorOnFrame = key;
			return true;
		}
		else
		{
			if ((key != m_CurrentCursor) && _SetCurrentCursor(key))
			{
				m_CurrentCursor = key;
				return true;
			}
		}
	}
	return false;
}

bool MkCursorManager::CommitCurrentCursor(void)
{
	if (m_DelayMode)
	{
		MkHashStr buffer = m_CursorOnFrame.Empty() ? m_NormalCursor : m_CursorOnFrame;
		m_CursorOnFrame.Clear();

		if (buffer == m_CurrentCursor)
			return true;
		
		if (_SetCurrentCursor(buffer))
		{
			m_CurrentCursor = buffer;
			return true;
		}
	}
	return false;
}

void MkCursorManager::Clear(void)
{
	m_CursorInfoTable.Clear();
	m_CurrentCursor.Clear();
	m_CurrentTexture = NULL;
}

void MkCursorManager::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	if (!m_CurrentCursor.Empty())
	{
		_SetCurrentCursor(m_CurrentCursor);
	}
}

void MkCursorManager::__ConsumeSetCursorMsg(void)
{
	if (!m_CurrentCursor.Empty())
	{
		LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
		if (device != NULL)
		{
			SetCursor(NULL);
			device->ShowCursor(TRUE);
		}
	}
}

MkCursorManager::MkCursorManager() : MkBaseResetableResource(), MkSingletonPattern<MkCursorManager>()
{
	m_DelayMode = true;
}

void MkCursorManager::_LoadCursor(const MkDataNode& node, const MkHashStr& dataKey, MkHashStr& targetName)
{
	MkStr buffer;
	if (node.GetData(dataKey, buffer, 0) && (!buffer.Empty()))
	{
		targetName = buffer;
	}
}

bool MkCursorManager::_SetCurrentCursor(const MkHashStr& key)
{
	LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
	if (device != NULL)
	{
		MkBaseTexturePtr tex;
		const _CursorInfo& ci = m_CursorInfoTable[key];
		MK_TEXTURE_POOL.GetBitmapTexture(ci.filePath, tex);

		if (tex != NULL)
		{
			if (device->SetCursorProperties(ci.hotspotX, ci.hotspotY, tex->GetSurface()) == D3D_OK)
			{
				m_CurrentTexture = NULL;
				m_CurrentTexture = tex;
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
