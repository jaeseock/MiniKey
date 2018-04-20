#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//


class ApResourceType
{
public:

	enum eKind
	{
		eK_Normal = 0 // row(x)-column(y)
	};

	inline void SetUp(eKind kind, unsigned int row, unsigned int column) { m_Value = ((row << 16) | (column << 8) | static_cast<unsigned int>(kind)); }

	inline unsigned int GetValue(void) const { return m_Value; }
	inline eKind GetKind(void) const { return static_cast<eKind>(m_Value & 0xff); }
	inline unsigned int GetRow(void) const { return ((m_Value >> 16) & 0xff); }
	inline unsigned int GetColumn(void) const { return ((m_Value >> 8) & 0xff); }

	ApResourceType() { SetUp(eK_Normal, 0, 0); }
	ApResourceType(const ApResourceType& resType) { m_Value = resType.GetValue(); }
	ApResourceType(eKind kind) { SetUp(kind, 0, 0); }
	ApResourceType(eKind kind, unsigned int row, unsigned int column) { SetUp(kind, row, column); }

protected:

	unsigned int m_Value;
};
