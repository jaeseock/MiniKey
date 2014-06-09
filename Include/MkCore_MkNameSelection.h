#ifndef __MINIKEY_CORE_MKNAMESELECTION_H__
#define __MINIKEY_CORE_MKNAMESELECTION_H__

//------------------------------------------------------------------------------------------------//
// 이름에 의한 선택 규칙 정의
// - MkSelectionByName : 동일 이름 여부
// - MkSelectionByPrefix : prefix 일치 여부
// - MkSelectionByPostfix : postfix 일치 여부
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"
#include "MkCore_MkHashStr.h"


class MkNameSelection
{
public:

	enum eSeletionType
	{
		eEquals = 0,
		ePrefix,
		ePostfix
	};

	inline eSeletionType GetSelectionType(void) const { return m_SelectionType; }

	inline const MkHashStr& GetArgument(void) const { return m_Argument; }

	inline bool Check(const MkHashStr& targetName) const
	{
		switch (m_SelectionType)
		{
		case eEquals: return (targetName == m_Argument);
		case ePrefix: return (targetName.GetString().CheckPrefix(m_Argument.GetString()));
		case ePostfix: return (targetName.GetString().CheckPostfix(m_Argument.GetString()));
		}
		return false;
	}

	inline bool Check(const MkStr& targetName) const
	{
		switch (m_SelectionType)
		{
		case eEquals: return (targetName == m_Argument.GetString());
		case ePrefix: return (targetName.CheckPrefix(m_Argument.GetString()));
		case ePostfix: return (targetName.CheckPostfix(m_Argument.GetString()));
		}
		return false;
	}

	MkNameSelection(const MkHashStr& argument, eSeletionType selectionType)
	{
		m_SelectionType = selectionType;
		m_Argument = argument;
	}

	virtual ~MkNameSelection() {}
	
protected:

	eSeletionType m_SelectionType;
	MkHashStr m_Argument;
};

//------------------------------------------------------------------------------------------------//

class MkSelectionByName : public MkNameSelection
{
public:
	MkSelectionByName(const MkHashStr& argument) : MkNameSelection(argument, eEquals) {}
	virtual ~MkSelectionByName() {}
};

//------------------------------------------------------------------------------------------------//

class MkSelectionByPrefix : public MkNameSelection
{
public:
	MkSelectionByPrefix(const MkHashStr& argument) : MkNameSelection(argument, ePrefix) {}
	virtual ~MkSelectionByPrefix() {}
};

//------------------------------------------------------------------------------------------------//

class MkSelectionByPostfix : public MkNameSelection
{
public:
	MkSelectionByPostfix(const MkHashStr& argument) : MkNameSelection(argument, ePostfix) {}
	virtual ~MkSelectionByPostfix() {}
};

//------------------------------------------------------------------------------------------------//

#endif
