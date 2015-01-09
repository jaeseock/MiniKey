
#include "MkS2D_MkWindowResourceManager.h"


//------------------------------------------------------------------------------------------------//

bool MkWindowResourceManager::LoadDecoText(const MkPathName& filePath)
{
	bool ok = m_DecoTextRootNode.SetUp(filePath);
	if (ok)
	{
		m_DecoTextFilePath = filePath;
	}
	return ok;
}

bool MkWindowResourceManager::ReloadLoadDecoText(void)
{
	return m_DecoTextFilePath.Empty() ? false : m_DecoTextRootNode.SetUp(m_DecoTextFilePath);
}

void MkWindowResourceManager::LoadWindowTypeImageSet(const MkDataNode& node)
{
	m_WindowTypeImageSet.SetUp(node);
}

void MkWindowResourceManager::LoadWindowPreset(const MkDataNode& node)
{
	m_WindowPreset.SetUp(node);
}

void MkWindowResourceManager::Clear(void)
{
	m_DecoTextRootNode.Clear();
	m_WindowTypeImageSet.Clear();
	m_WindowPreset.Clear();
}

//------------------------------------------------------------------------------------------------//
