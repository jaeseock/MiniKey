#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"


class MkDataNode;

class MkWindowTypeImageSet
{
public:

	enum eSetType
	{
		eNull = -1,
		eSingleType = 0, // SN(subset name)
		e3And1Type, // (3, 1). 1 by 3 :	L(left), M(middle), R(right)
		e1And3Type, // (1, 3). 3 by 1 :	T(top), C(center), B(bottom)
		e3And3Type // (3, 3). 3 by 3 :	LT(left-top), MT(middle-top), RT(right-top),
									//	LC(left-center), MC(middle-center), RC(right-center),
									//	LB(left-bottom), MB(middle-bottom), RB(right-bottom)
	};

	enum eTypeIndex
	{
		eL = 0, // at e3And1Type
		eT = eL, // at e1And3Type
		eLT = eL, // at e3And3Type

		eM, // at e3And1Type
		eC = eM, // at e1And3Type
		eMT = eM, // at e3And3Type

		eR, // at e3And1Type
		eB = eR, // at e1And3Type
		eRT = eR, // at e3And3Type

		eLC, // at e3And3Type
		eMC, // at e3And3Type
		eRC, // at e3And3Type
		eLB, // at e3And3Type
		eMB, // at e3And3Type
		eRB // at e3And3Type
	};

	typedef struct _Pack
	{
		eSetType type;
		MkPathName filePath;
		MkArray<MkHashStr> subsetNames;
	}
	Pack;

	// 초기화
	void SetUp(const MkDataNode& node);

	// Pack 존재 여부 반환
	inline bool Exist(const MkHashStr& name) const { return m_Sets.Exist(name); }

	// Pack 참조 반환
	inline const Pack& GetPack(const MkHashStr& name) const { return Exist(name) ? m_Sets[name] : m_Null; }

	// 해제
	inline void Clear(void) { m_Sets.Clear(); }

	// set type, index에 해당하는 keyword 반환
	static const MkHashStr& GetImageSetKeyword(eSetType type, eTypeIndex index);

	MkWindowTypeImageSet();
	~MkWindowTypeImageSet() { Clear(); }

protected:

	MkHashMap<MkHashStr, Pack> m_Sets;
	Pack m_Null;
};
