#pragma once

#include "MkCore_MkHashStr.h"


//------------------------------------------------------------------------------------------------//
// supplement definition
//------------------------------------------------------------------------------------------------//

class SupplementDef
{
public:

	// color
	enum eColor
	{
		eC_None = -1,

		eC_Dark = 0,
		eC_Silver,
		eC_Gold
	};

	// tech
	enum eTech
	{
		eT_None = -1,

		eT_Command = 0,
		eT_Supplement,
		eT_Soldier,
		eT_Tactics,

		eT_Max
	};

	// unit(color + tech)
	enum eUnit
	{
		eU_None = -1,

		eU_DarkCommand = 0,
		eU_DarkSupplement,
		eU_DarkSoldier,
		eU_DarkTactics,

		eU_SilverCommand,
		eU_SilverSupplement,
		eU_SilverSoldier,
		eU_SilverTactics,

		eU_GoldCommand,
		eU_GoldSupplement,
		eU_GoldSoldier,
		eU_GoldTactics,

		eU_Void, // eC_None & eT_None

		eU_Max
	};

	// set
	enum eSet
	{
		eS_None = 0,
		eS_FullSet,
		eS_Pair,
		eS_Trips,
		eS_FullHouse,
		eS_Quads,
		eS_Quints
	};

	// result(color + set)
	enum eResult
	{
		eR_None = -1,

		eR_FullSet = 0, // no color
		eR_Pair,
		eR_Trips,
		eR_FullHouse,
		eR_Quads,
		eR_Quints,

		eR_DarkFullSet,
		eR_DarkPair,
		eR_DarkTrips,
		eR_DarkFullHouse,
		eR_DarkQuads,
		eR_DarkQuints,

		eR_SilverFullSet,
		eR_SilverPair,
		eR_SilverTrips,
		eR_SilverFullHouse,
		eR_SilverQuads,
		eR_SilverQuints,

		eR_GoldFullSet,
		eR_GoldPair,
		eR_GoldTrips,
		eR_GoldFullHouse,
		eR_GoldQuads,
		eR_GoldQuints,

		eR_Max
	};

	static const MkHashStr ResultTag[eR_Max];

public:

	static bool IsNormalUnit(eUnit unit);

	static eColor GetColor(eUnit unit);

	static eTech GetTech(eUnit unit);

	static eResult GetResult(const MkArray<eUnit>& units, MkArray<unsigned int>& targetIndice);

protected:

	static eColor _GetColor(const MkArray<eUnit>& units);
	static eColor _GetColor(const MkArray<eUnit>& units, const MkArray<unsigned int>& indice);
	static eSet _GetTupleSet(unsigned int level);
	static eResult _GetResult(eColor color, eSet set);
	static void _FillTargetIndice(unsigned int count, MkArray<unsigned int>& targetIndice);

	static eResult _FindHighestTuple
		(const MkArray<eUnit>& units, const MkArray<unsigned int>& voidIndice, const MkArray< MkArray<unsigned int> >& perTechIndice,
		MkArray<unsigned int>& targetIndice);

	static eResult _FindTargetTuple
		(unsigned int targetLevel, const MkArray<eUnit>& units, const MkArray<unsigned int>& voidIndice, const MkArray< MkArray<unsigned int> >& perTechIndice,
		MkArray<unsigned int>& targetIndice);

};

#define ADEF_MAX_COMBINATION_SLOT_SIZE 5

MKDEF_DECLARE_FIXED_SIZE_TYPE(SupplementDef::eColor)
MKDEF_DECLARE_FIXED_SIZE_TYPE(SupplementDef::eTech)
MKDEF_DECLARE_FIXED_SIZE_TYPE(SupplementDef::eUnit)
MKDEF_DECLARE_FIXED_SIZE_TYPE(SupplementDef::eSet)
MKDEF_DECLARE_FIXED_SIZE_TYPE(SupplementDef::eResult)


//------------------------------------------------------------------------------------------------//
// object definition
//------------------------------------------------------------------------------------------------//

class GameObjectDef
{
public:

	enum eGrade
	{
		eG_Basic = 0,
		eG_Advanced,
		eG_Excellent,
		eG_Super,

		eG_Max
	};

	enum eLevel
	{
		eL_Noob = 0,
		eL_Standard,
		eL_Expert,
		eL_Master,

		eL_Max
	};
};



