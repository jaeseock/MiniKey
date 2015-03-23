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
#include "MkPA_MkWindowThemeData.h"


class MkWindowThemeSet
{
public:

	// �ʱ�ȭ. ������ �ּ� default theme�� �̻���� �������� �ǹ�
	bool SetUp(const MkDataNode& node);

	// theme�� form data ��ȯ
	// ���� �־��� theme�� component�� �������� ���� ��� default theme�� ���� ��� ��ȯ
	// (�ʱ�ȭ�� �����ߴٸ� ��� regular component�� ���߾��� default theme�� �����Ѵٴ� �ǹ��̹Ƿ� eCT_None, eCT_CustomForm�� �ƴϸ� �׻� ��ȯ ����)
	const MkWindowThemeFormData* GetFormData(const MkHashStr& themeName, MkWindowThemeData::eComponentType compType, const MkHashStr& formName);

	// theme setting
	float GetFrameSize(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType) const;
	const MkArray<MkHashStr>& GetCaptionTextNode(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType) const;

	// ������ �ʴ� theme�� üũ�� �̹����� ����
	void UnloadUnusedThemeImage(void);

	// ����
	void Clear(void);

	~MkWindowThemeSet() { Clear(); }

protected:

	const MkWindowThemeFormData* _GetFormData(const MkHashStr& themeName, MkWindowThemeData::eComponentType compType, const MkHashStr& formName);

protected:

	MkHashMap<MkHashStr, MkWindowThemeData> m_Themes;

	MkHashStr m_CurrentTheme;
	MkArray<MkHashStr> m_UsedThemes;
};
