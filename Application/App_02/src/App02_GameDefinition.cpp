
#include "MkCore_MkCheck.h"
#include "App02_GameDefinition.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr SupplementDef::ResultTag[eR_Max] =
{
	L"S", L"2", L"3", L"H", L"4", L"5",
	L"DS", L"D2", L"D3", L"DH", L"D4", L"D5",
	L"SS", L"S2", L"S3", L"SH", L"S4", L"S5",
	L"GS", L"G2", L"G3", L"GH", L"G4", L"G5"
};

bool SupplementDef::IsNormalUnit(eUnit unit)
{
	return ((unit >= eU_DarkCommand) && (unit < eU_Void));
}

SupplementDef::eColor SupplementDef::GetColor(eUnit unit)
{
	switch (unit)
	{
	case eU_DarkCommand:
	case eU_DarkSupplement:
	case eU_DarkSoldier:
	case eU_DarkTactics: return eC_Dark;

	case eU_SilverCommand:
	case eU_SilverSupplement:
	case eU_SilverSoldier:
	case eU_SilverTactics: return eC_Silver;

	case eU_GoldCommand:
	case eU_GoldSupplement:
	case eU_GoldSoldier:
	case eU_GoldTactics: return eC_Gold;
	}
	return eC_None;
}

SupplementDef::eTech SupplementDef::GetTech(eUnit unit)
{
	switch (unit)
	{
	case eU_DarkCommand:
	case eU_SilverCommand:
	case eU_GoldCommand: return eT_Command;

	case eU_DarkSupplement:
	case eU_SilverSupplement:
	case eU_GoldSupplement: return eT_Supplement;

	case eU_DarkSoldier:
	case eU_SilverSoldier:
	case eU_GoldSoldier: return eT_Soldier;

	case eU_DarkTactics:
	case eU_SilverTactics:
	case eU_GoldTactics: return eT_Tactics;
	}
	return eT_None;
}

SupplementDef::eResult SupplementDef::GetResult(const MkArray<eUnit>& units, MkArray<unsigned int>& targetIndice)
{
	unsigned int unitSize = units.GetSize();

	// ��ȿ�� ����
	MK_CHECK(unitSize <= ADEF_MAX_COMBINATION_SLOT_SIZE, L"�Էµ� unit ������ " + MkStr(ADEF_MAX_COMBINATION_SLOT_SIZE) + L"���� ���� : " + MkStr(unitSize))
		return eR_None;

	MK_INDEXING_LOOP(units, i)
	{
		eUnit unit = units[i];
		MK_CHECK((unit == eU_Void) || IsNormalUnit(unit), L"�Էµ� unit list�� " + MkStr(i) + L"��°�� ������ unit ���� : " + MkStr(unit))
			return eR_None;
	}

	if (unitSize < 2) // 0, 1���� ���� ����
		return eR_None;

	// ���� ���պ��� ���� ���� ������ ����. ���� tech���� �з�
	MkArray<unsigned int> voidIndice(unitSize); // eU_Void�� indice
	MkArray< MkArray<unsigned int> > perTechIndice; // tech�� indice
	for (int i=0; i<eT_Max; ++i)
	{
		MkArray<unsigned int>& indice = perTechIndice.PushBack();
		indice.Reserve(unitSize);
	}

	// eS_Quints
	
/*
	// unit ���� 0 ~ 2���� ���� ���� �˻�
	if (unitSize < 2) // 0, 1���� ���� ����
	{
		return eR_None;
	}
	else if (unitSize == 2) // pair�� ����
	{
		if (units[0] == units[1])
		{
			if (IsNormalUnit(units[0])) // eU_Void ���� pair. ��� eU_Void�� ���� eR_None
			{
				_FillTargetIndice(2, targetIndice);
				return _GetResult(_GetColor(units), eS_Pair);
			}
		}
		else if (units[0] == eU_Void) // �������� units[1]�� normal unit���� ���� ��
		{
			_FillTargetIndice(2, targetIndice);
			return _GetResult(GetColor(units[1]), eS_Pair); // eU_Void�� ���� pair
		}
		else if (units[1] == eU_Void) // �������� units[0]�� normal unit���� ���� ��
		{
			_FillTargetIndice(2, targetIndice);
			return _GetResult(GetColor(units[0]), eS_Pair); // eU_Void�� ���� pair
		}
		return eR_None;
	}

	//  unit ���� 3�� �̻��� ���� �˻縦 ���� tech�� �з�
	MkArray<unsigned int> voidIndice(unitSize); // eU_Void�� index list
	MkArray<unsigned int> perTechIndice[eT_Max]; // tech�� index list
	for (int i=0; i<eT_Max; ++i)
	{
		perTechIndice[i].Reserve(unitSize);
	}

	MK_INDEXING_LOOP(units, i)
	{
		if (units[i] == eU_Void)
		{
			voidIndice.PushBack(i);
		}
		else
		{
			perTechIndice[GetTech(units[i])].PushBack(i);
		}
	}

	// unit ���� ���� �˻�
	if (unitSize == 3)
	{
		if (voidIndice.Empty()) // eU_Void ����
		{
			for (int i=0; i<eT_Max; ++i)
			{
				const MkArray<unsigned int>& currIndice = perTechIndice[i];
				if (currIndice.GetSize() > 1) // 2, 3
				{
					targetIndice = currIndice;
					return _GetResult(_GetColor(units, currIndice), (currIndice.GetSize() == 2) ? eS_Pair : eS_Trips);
				}
			}
		}
		else if (voidIndice.GetSize() < 3) // 1, 2
		{
			for (int i=0; i<eT_Max; ++i)
			{
				const MkArray<unsigned int>& currIndice = perTechIndice[i];
				if (((voidIndice.GetSize() == 1) && (currIndice.GetSize() == 2)) || // eU_Void �� ���̰� normal unit�� pair�� ���
					((voidIndice.GetSize() == 2) && (currIndice.GetSize() == 1))) // eU_Void �� ���̰� normal unit�� single�� ���
				{
					_FillTargetIndice(3, targetIndice);
					return _GetResult(_GetColor(units, currIndice), eS_Trips);
				}
			}
		}
		return eR_None; // ������ ���ų�(3���� tech�� �ϳ���), ��ȣ�ϰų�(eU_Void �ϳ� + 2���� tech�� �ϳ���), eU_Void������ �̷���� ���� ���
	}
	else if (unitSize == 4)
	{
		if (voidIndice.Empty()) // eU_Void ����
		{
			// eS_FullSet �˻�. ��� tech�� ������ 1�̾�� ��
			bool isFullSet = true;
			for (int i=0; i<eT_Max; ++i)
			{
				if (perTechIndice[i].GetSize() != 1)
				{
					isFullSet = false;
					break;
				}
			}
			if (isFullSet)
			{
				_FillTargetIndice(3, targetIndice);
				return _GetResult(_GetColor(units), eS_FullSet);
			}

			// tuple �˻�
			MkArray<int> pairs(2);
			for (int i=0; i<eT_Max; ++i)
			{
				const MkArray<unsigned int>& currIndice = perTechIndice[i];
				if (currIndice.GetSize() == 2)
				{
					pairs.PushBack(i);
				}
				if (currIndice.GetSize() > 2) // 3, 4
				{
					targetIndice = currIndice;
					return _GetResult(_GetColor(units, currIndice), (currIndice.GetSize() == 3) ? eS_Trips : eS_Quads);
				}
			}

			if (pairs.GetSize() == 1) // 2 pair�� ����(��ȣ��)
			{
				const MkArray<unsigned int>& currIndice = perTechIndice[pairs[0]];
				targetIndice = currIndice;
				return _GetResult(_GetColor(units, currIndice), eS_Pair);
			}
		}
		else if (voidIndice.GetSize() < 4) // 1 ~ 3
		{
			for (int i=0; i<eT_Max; ++i)
			{
				const MkArray<unsigned int>& currIndice = perTechIndice[i];
				if (((voidIndice.GetSize() == 1) && (currIndice.GetSize() == 2)) || // eU_Void �� ���̰� normal unit�� pair�� ���
					((voidIndice.GetSize() == 2) && (currIndice.GetSize() == 1))) // eU_Void �� ���̰� normal unit�� single�� ���
				{
					_FillTargetIndice(3, targetIndice);
					return _GetResult(_GetColor(units, currIndice), eS_Trips);
				}
			}
		}
		return eR_None; // ������ ���ų�(3���� tech�� �ϳ���), ��ȣ�ϰų�(eU_Void �ϳ� + 2���� tech�� �ϳ���), eU_Void������ �̷���� ���� ���
	}

	// result ���
	eResult result = eR_None;

	// normal unit���� ���� �� �ִ� �ְ� set�� ����
	for (int i=0; i<eT_Max; ++i)
	{
		if (perTechIndice[i].GetSize() == 5) // quints?
		{
			result = _GetResult(_GetColor(units, perTechIndice[i]), eS_Quints);
			//units.GetDifferenceOfSets(const MkArray<DataType>& targets, MkArray<DataType>& sourceOnly)
			//units.GetIntersectionOfSets
		}
	}
	return result;
	*/

	return eR_None;
}

SupplementDef::eColor SupplementDef::_GetColor(const MkArray<eUnit>& units)
{
	if (units.Empty())
		return eC_None;

	SupplementDef::eColor initColor = GetColor(units[0]);
	if (initColor == eC_None)
		return eC_None;

	for (unsigned int cnt=units.GetSize(), i=1; i<cnt; ++i)
	{
		if (initColor != GetColor(units[i])) // �ϳ��� ���� �ٸ��� eC_None
			return eC_None;
	}
	return initColor;
}

SupplementDef::eColor SupplementDef::_GetColor(const MkArray<eUnit>& units, const MkArray<unsigned int>& indice)
{
	if (units.Empty() || indice.Empty())
		return eC_None;

	SupplementDef::eColor initColor = GetColor(units[indice[0]]);
	if (initColor == eC_None)
		return eC_None;

	for (unsigned int cnt=indice.GetSize(), i=1; i<cnt; ++i)
	{
		if (initColor != GetColor(units[indice[i]])) // �ϳ��� ���� �ٸ��� eC_None
			return eC_None;
	}
	return initColor;
}

SupplementDef::eSet SupplementDef::_GetTupleSet(unsigned int level)
{
	switch (level)
	{
	case 2: return eS_Pair;
	case 3: return eS_Trips;
	case 4: return eS_Quads;
	case 5: return eS_Quints;
	}
	return eS_None;
}

SupplementDef::eResult SupplementDef::_GetResult(eColor color, eSet set)
{
	eResult result = eR_None;
	switch (color)
	{
	case eC_None:
		{
			switch (set)
			{
			case eS_FullSet: result = eR_FullSet; break;
			case eS_Pair: result = eR_Pair; break;
			case eS_Trips: result = eR_Trips; break;
			case eS_FullHouse: result = eR_FullHouse; break;
			case eS_Quads: result = eR_Quads; break;
			case eS_Quints: result = eR_Quints; break;
			}
		}
		break;

	case eC_Dark:
		{
			switch (set)
			{
			case eS_FullSet: result = eR_DarkFullSet; break;
			case eS_Pair: result = eR_DarkPair; break;
			case eS_Trips: result = eR_DarkTrips; break;
			case eS_FullHouse: result = eR_DarkFullHouse; break;
			case eS_Quads: result = eR_DarkQuads; break;
			case eS_Quints: result = eR_DarkQuints; break;
			}
		}
		break;

	case eC_Silver:
		{
			switch (set)
			{
			case eS_FullSet: result = eR_SilverFullSet; break;
			case eS_Pair: result = eR_SilverPair; break;
			case eS_Trips: result = eR_SilverTrips; break;
			case eS_FullHouse: result = eR_SilverFullHouse; break;
			case eS_Quads: result = eR_SilverQuads; break;
			case eS_Quints: result = eR_SilverQuints; break;
			}
		}
		break;

	case eC_Gold:
		{
			switch (set)
			{
			case eS_FullSet: result = eR_GoldFullSet; break;
			case eS_Pair: result = eR_GoldPair; break;
			case eS_Trips: result = eR_GoldTrips; break;
			case eS_FullHouse: result = eR_GoldFullHouse; break;
			case eS_Quads: result = eR_GoldQuads; break;
			case eS_Quints: result = eR_GoldQuints; break;
			}
		}
		break;
	}
	return result;
}

void SupplementDef::_FillTargetIndice(unsigned int count, MkArray<unsigned int>& targetIndice)
{
	if (count > 0)
	{
		targetIndice.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			targetIndice.PushBack(i);
		}
	}
}

SupplementDef::eResult SupplementDef::_FindHighestTuple
(const MkArray<eUnit>& units, const MkArray<unsigned int>& voidIndice, const MkArray< MkArray<unsigned int> >& perTechIndice, MkArray<unsigned int>& targetIndice)
{
	for (unsigned int level=5; level>1; --level) // 5, 4, 3, 2
	{
		eResult result = _FindTargetTuple(level, units, voidIndice, perTechIndice, targetIndice);
		if (result != eR_None)
			return result;
	}
	return eR_None;
}

SupplementDef::eResult SupplementDef::_FindTargetTuple
(unsigned int targetLevel, const MkArray<eUnit>& units, const MkArray<unsigned int>& voidIndice, const MkArray< MkArray<unsigned int> >& perTechIndice, MkArray<unsigned int>& targetIndice)
{
	unsigned int unitSize = units.GetSize();
	if (unitSize >= targetLevel)
	{
		for (unsigned int i=0; i<eT_Max; ++i)
		{
			const MkArray<unsigned int>& currIndice = perTechIndice[i];

			for (unsigned int voidCount=0; voidCount<unitSize; ++voidCount)
			{
				if ((voidIndice.GetSize() == voidCount) && (currIndice.GetSize() == (unitSize - voidCount))) // eU_Void�� �� �ϳ��� tech�� ������ �ϼ��Ǵ� ���
				{
					//_FillTargetIndice(targetLevel, targetIndice);
					return _GetResult(_GetColor(units, currIndice), _GetTupleSet(targetLevel));
				}
			}
		}
	}
	return eR_None;
}