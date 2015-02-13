#pragma once


//------------------------------------------------------------------------------------------------//
// overlay 구성을 위한 설정 class
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"
#include "MkCore_MkHashStr.h"
#include "MkPA_MkGlobalDefinition.h"


class MkWindowViewArgument
{
public:

	// 설정
	void SetContents(ePA_PieceSetType edgeAndClientPST, bool useShadow, bool fillClient, const MkInt2& clientSize);
	void SetContents(ePA_PieceSetType edgePST, bool useShadow, const MkPathName& imagePath, const MkHashStr& subsetOrSequenceName);

	// 설정값 반환
	inline ePA_PieceSetType GetPieceSetType(void) const { return m_PieceSetType; }
	inline bool GetUseShadow(void) const { return m_UseShadow; }
	inline bool GetFillClient(void) const { return m_FillClient; }
	const MkPathName& GetImagePath(void) const { return m_ImagePath; }
	const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }

	const MkInt2& GetWindowSize(void) const { return m_WindowSize; }
	const MkInt2& GetClientSize(void) const { return m_ClientSize; }
	const MkInt2& GetClientPosition(void) const { return m_ClientPosition; }

	// 초기화
	void Clear(void);

	MkWindowViewArgument() { Clear(); }

protected:

	ePA_PieceSetType m_PieceSetType;
	bool m_UseShadow;
	bool m_FillClient;
	MkPathName m_ImagePath;
	MkHashStr m_SubsetOrSequenceName;

	MkInt2 m_WindowSize;
	MkInt2 m_ClientSize;
	MkInt2 m_ClientPosition;
};
