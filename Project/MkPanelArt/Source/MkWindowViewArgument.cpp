
#include "MkPA_MkBitmapPool.h"
#include "MkWindowViewArgument.h"

//------------------------------------------------------------------------------------------------//

void MkWindowViewArgument::SetContents(ePA_PieceSetType edgeAndClientPST, bool useShadow, bool fillClient, const MkInt2& clientSize)
{
	Clear();

	if (edgeAndClientPST != ePA_PST_None)
	{
		m_PieceSetType = edgeAndClientPST;
		m_UseShadow = useShadow;
		m_FillClient = fillClient;
		m_ClientSize = clientSize;
	}
}

void MkWindowViewArgument::SetContents(ePA_PieceSetType edgePST, bool useShadow, const MkPathName& imagePath, const MkHashStr& subsetOrSequenceName)
{
	Clear();

	m_PieceSetType = edgePST;
	m_UseShadow = useShadow;

	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(imagePath);
	if (texture != NULL)
	{
		const MkImageInfo::Subset* subset = texture->GetImageInfo().GetCurrentSubsetPtr(subsetOrSequenceName);
		if (subset != NULL)
		{
			m_ImagePath = imagePath;
			m_SubsetOrSequenceName = subsetOrSequenceName;
			m_ClientSize = MkInt2(static_cast<int>(subset->rectSize.x), static_cast<int>(subset->rectSize.y));
		}
	}
}

void MkWindowViewArgument::Clear(void)
{
	m_PieceSetType = ePA_PST_None;
	m_UseShadow = false;
	m_FillClient = false;
	m_ImagePath.Clear();
	m_SubsetOrSequenceName.Clear();
	m_WindowSize.Clear();
	m_ClientSize.Clear();
	m_ClientPosition.Clear();
}

//------------------------------------------------------------------------------------------------//
