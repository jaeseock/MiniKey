#pragma once


//------------------------------------------------------------------------------------------------//
// ex>
//	MkDecoStr buffer;
//	MkDecoTextNode* targetNode = MK_WR_DECO_TEXT.GetChildNode(L"character");
//	if (targetNode != NULL)
//		targetNode = targetNode->GetChildNode(L"status");
//	if (targetNode != NULL)
//		buffer = targetNode->GetDecoText(L"strength desc");
//
// �� �ڵ�� �Ʒ� �ڵ�� ����
//	const MkDecoStr& buffer = MK_WR_DECO_TEXT.GetDecoText(L"character", L"status", L"strength desc");
//
// Ȥ�� �Ʒ� �ڵ�� ����
//	MkArray<MkHashStr> nodeNameAndKey(3);
//	nodeNameAndKey.PushBack(L"character");
//	nodeNameAndKey.PushBack(L"status");
//	nodeNameAndKey.PushBack(L"strength desc");
//	const MkDecoStr& buffer = MK_WR_DECO_TEXT.GetDecoText(nodeNameAndKey);

//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkPathName.h"

#include "MkS2D_MkDecoTextNode.h"
#include "MkS2D_MkWindowTypeImageSet.h"
#include "MkS2D_MkWindowPreset.h"


#define MK_WIN_RES MkWindowResourceManager::Instance()
#define MK_WR_DECO_TEXT MkWindowResourceManager::Instance().GetDecoTextRoot()
#define MK_WR_IMAGE_SET MkWindowResourceManager::Instance().GetWindowTypeImageSet()
#define MK_WR_PRESET MkWindowResourceManager::Instance().GetWindowPreset()


class MkWindowResourceManager : public MkSingletonPattern<MkWindowResourceManager>
{
public:

	// deco text
	bool LoadDecoText(const MkPathName& filePath);
	bool ReloadLoadDecoText(void);
	inline const MkDecoTextNode& GetDecoTextRoot(void) const { return m_DecoTextRootNode; }

	// window type image set
	void LoadWindowTypeImageSet(const MkDataNode& node);
	inline const MkWindowTypeImageSet& GetWindowTypeImageSet(void) const { return m_WindowTypeImageSet; }

	// window preset
	void LoadWindowPreset(const MkDataNode& node);
	inline const MkWindowPreset& GetWindowPreset(void) const { return m_WindowPreset; }

	// ����
	void Clear(void);

	MkWindowResourceManager() : MkSingletonPattern<MkWindowResourceManager>() {};
	virtual ~MkWindowResourceManager() { Clear(); }

protected:

	// deco text
	MkPathName m_DecoTextFilePath;
	MkDecoTextNode m_DecoTextRootNode;

	// window type image set
	MkWindowTypeImageSet m_WindowTypeImageSet;

	// window preset
	MkWindowPreset m_WindowPreset;
};
