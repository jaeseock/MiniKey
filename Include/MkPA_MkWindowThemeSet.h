#pragma once


//------------------------------------------------------------------------------------------------//
// theme set data
// MkWindowThemeData�� ����
// theme�� texture�� �������� ����
//
// (NOTE) theme�� component�� ���� �ð����� �������� ����Ǿ� �ְ� ����� ������� �������� ����
//
// MkWindowThemeSet : theme ��ü�� data
//    L MkWindowThemeData : theme ������ data
//       L MkWindowThemeFormData : componet ������ data
//          L MkWindowThemeUnitData : componet ���� ��ҿ� ���� panel ���� data
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkDeque.h"
#include "MkPA_MkWindowThemeData.h"


class MkWindowThemeSet
{
public:

	// �ʱ�ȭ. ������ �ּ� default theme�� �̻���� �������� �ǹ�
	bool SetUp(const MkDataNode& node);

	// ���� theme ��� ����
	// ��ȯ���� �ش� theme ��� ���� ����
	bool SetCurrentTheme(const MkHashStr& themeName);

	// ��� ������ theme ��ȯ
	inline const MkHashStr& GetCurrentTheme(void) const { return m_CurrentTheme; }

	// ��� ������ theme�� ���� ��ȯ
	// ���� �־��� theme�� component�� �������� ���� ��� default theme�� ���� ��� ��ȯ
	// (�ʱ�ȭ�� �����ߴٸ� ��� component�� ���߾��� default theme�� �����Ѵٴ� �ǹ��̹Ƿ� �׻� ��ȯ ����)
	const MkHashStr& GetImageFilePath(void) const;
	const MkWindowThemeFormData* GetFormData(MkWindowThemeData::eComponentType compType) const;

	// ������ �ʴ� theme�� üũ�� �̹����� ����
	void UnloadUnusedThemeImage(void);

	// ����
	void Clear(void);

	~MkWindowThemeSet() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkWindowThemeData> m_Themes;

	MkHashStr m_CurrentTheme;
	MkDeque<MkHashStr> m_UsedThemes;
};
