
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkCursorManager.h"


//------------------------------------------------------------------------------------------------//

const static MkHashStr CURSOR_NAME[MkCursorManager::eMaxType] = { L"Normal", L"Excutable", L"Disable", L"Movable", L"Moving", L"TextCursor" };
const static MkHashStr FILE_PATH_KEY = L"FilePath";
const static MkHashStr HOTSPOT_KEY = L"Hotspot";


void MkCursorManager::SetUp(const MkDataNode* node)
{
	if (node == NULL)
		return;

	for (unsigned int i=0; i<eMaxType; ++i)
	{
		const MkDataNode* childNode = node->GetChildNode(CURSOR_NAME[i]);
		if (childNode != NULL)
		{
			MkStr filePath;
			MK_CHECK(childNode->GetData(FILE_PATH_KEY, filePath, 0) && (!filePath.Empty()), L"MkCursorManager에 등록될 " + CURSOR_NAME[i].GetString() + L" 노드의 " + FILE_PATH_KEY.GetString() + L" 항목 오류")
				continue;

			MkInt2 hotspot;
			childNode->GetData(HOTSPOT_KEY, hotspot, 0);
			
			RegisterCursor(static_cast<eCursorType>(i), filePath, static_cast<unsigned int>(hotspot.x), static_cast<unsigned int>(hotspot.y));
		}
	}
}

bool MkCursorManager::RegisterCursor(eCursorType cursorType, const MkPathName& filePath, unsigned int hotspotX, unsigned int hotspotY)
{
	const MkStr& cursorName = CURSOR_NAME[cursorType].GetString();

	MK_CHECK(!m_CursorInfoTable.Exist(cursorType), L"이미 존재하는 cursor type 등록 시도 : " + cursorName)
		return false;

	MK_CHECK(MkFileManager::CheckAvailable(filePath), L"존재하지 않는 cursor type image 등록 시도 : " + cursorName + L", " + filePath)
		return false;

	MK_CHECK(MK_BITMAP_POOL.LoadBitmapTexture(filePath), L"적합하지 않은 cursor type image 등록 시도 : " + cursorName + L", " + filePath)
		return false;

	_CursorInfo& ci = m_CursorInfoTable.Create(cursorType);
	ci.filePath = filePath;
	ci.hotspotX = hotspotX;
	ci.hotspotY = hotspotY;

	MK_DEV_PANEL.MsgToLog(L"> cursor type 등록 : " + cursorName, true);
	return true;
}

bool MkCursorManager::SetCursorType(eCursorType cursorType)
{
	if (cursorType == m_CurrentCursorType)
		return true;

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device != NULL)
	{
		MkBaseTexturePtr texBuf;
		const _CursorInfo& ci = m_CursorInfoTable[cursorType];
		MK_BITMAP_POOL.GetBitmapTexture(ci.filePath, texBuf);

		if (texBuf != NULL)
		{
			if (device->SetCursorProperties(ci.hotspotX, ci.hotspotY, texBuf->GetSurface()) == D3D_OK)
			{
				m_CurrentCursorType = cursorType;

				m_CurrentCursorTexture = NULL;
				m_CurrentCursorTexture = texBuf;
				return true;
			}
		}
	}
	return false;
}

void MkCursorManager::Clear(void)
{
	m_CursorInfoTable.Clear();
	m_CurrentCursorType = eNone;
	m_CurrentCursorTexture = NULL;
}

void MkCursorManager::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	if (m_CurrentCursorType != eNone)
	{
		eCursorType cursorType = m_CurrentCursorType;
		m_CurrentCursorType = eNone;
		SetCursorType(cursorType);
	}
}

void MkCursorManager::__ConsumeSetCursorMsg(void)
{
	if (m_CurrentCursorType != eNone)
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		if (device != NULL)
		{
			SetCursor(NULL);
			device->ShowCursor(TRUE);
		}
	}
}

MkCursorManager::MkCursorManager() : MkBaseResetableResource(), MkSingletonPattern<MkCursorManager>()
{
	m_CurrentCursorType = eNone;
}

//------------------------------------------------------------------------------------------------//
