#pragma once


//------------------------------------------------------------------------------------------------//
// theme set data
// MkWindowThemeData의 집합
// theme의 texture는 유지하지 않음
//
// (NOTE) theme와 component는 오직 시각적인 구성에만 관계되어 있고 기능적 측면과는 연관되지 않음
//
// MkWindowThemeSet : theme 전체의 data
//    L MkWindowThemeData : theme 각각의 data
//       L MkWindowThemeFormData : componet 각각의 data
//          L MkWindowThemeUnitData : componet 구성 요소에 대한 panel 설정 data
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkPA_MkWindowThemeData.h"


class MkWindowThemeSet
{
public:

	// 초기화. 성공은 최소 default theme는 이상없이 존재함을 의미
	bool SetUp(const MkDataNode& node);

	// 현재 theme 사용 지정
	// 반환값은 해당 theme 사용 가능 여부
	// 사용 지정이 된다고 해당 theme만 사용 할 수 있다는 것은 아니고 이전 theme도 사용되고 있는 곳이 있으면 계속 유지됨
	// (이전 theme들은 사용되고 있는 곳이 없으면 자동으로 자원 해제됨)
	bool SetCurrentTheme(const MkHashStr& themeName);

	// 사용 지정된 theme 반환
	inline const MkHashStr& GetCurrentTheme(void) const { return m_CurrentTheme; }

	// 사용 지정된 theme의 정보 반환
	// 만약 주어진 theme나 component가 존재하지 않을 경우 default theme의 것을 대신 반환
	// (초기화가 성공했다면 모든 component가 갖추어진 default theme가 존재한다는 의미이므로 MkWindowThemeData::eCT_None만 아니면 항상 반환 성공)
	const MkHashStr& GetImageFilePath(void) const;
	const MkWindowThemeFormData* GetFormData(MkWindowThemeData::eComponentType compType) const;

	// 사용되지 않는 theme를 체크해 이미지를 삭제
	void UnloadUnusedThemeImage(void);

	// 해제
	void Clear(void);

	~MkWindowThemeSet() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkWindowThemeData> m_Themes;

	MkHashStr m_CurrentTheme;
	MkArray<MkHashStr> m_UsedThemes;
};
