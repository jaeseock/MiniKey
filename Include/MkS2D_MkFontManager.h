#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - D3D font manager
//
// * 내부용
// - font unit : dx 객체와 정보를 저장하는 공간
// - font key : unit과 1:1 대응하는 구분자. face + size + thickness
//
// * 유저 정의 구분자
// - font type : 폰트 형태를 정의. face + size + thickness
//   등록시(CreateFontType) unit 객체 생성
//   별도의 type을 정의하지 않고도 직접 face, size, thickness를 주어 사용할 수 있으며 만약 동일한 설정이
//   기존 등록되어 있던 type에 존재 할 경우 같은 unit을 공유함
//   다른 이름의 type이 같은 설정을 공유할 경우에도 마찬가지로 unit은 공유됨
// - font color : 컬러값 정의
// - font state : 어떤 식으로 그려질지를 정의. text color + output mode + mode color
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkS2D_MkD3DDefinition.h"
#include "MkS2D_MkBaseResetableResource.h"


#define MK_FONT_MGR MkFontManager::Instance()


class MkColor;
class MkDataNode;
class MkDecoStr;

class MkFontManager : public MkBaseResetableResource, public MkSingletonPattern<MkFontManager>
{
public:

	enum eOutputMode
	{
		eNormal = 0,
		eOutline,
		eShadow
	};

	enum eThickness
	{
		eThin = 0, // 30%
		eMedium, // 50%
		eThick, // 70%
		eFull // 100%
	};

public:

	// 초기화. 이후 수동 초기화 과정이 필요
	bool SetUp(LPDIRECT3DDEVICE9 device);

	// MkDataNode의 설정으로 초기화
	bool SetUp(LPDIRECT3DDEVICE9 device, const MkDataNode* dataNode);

	// 별도로 등록할 시스템 폰트가 있을 경우 사용
	// Clear() 호출 시 해제됨
	// filePath : root directory 기준 상대 혹은 절대 폰트 파일 경로
	// ex> LoadSystemFont(L"Font\\08SeoulNamsanM.ttf");
	bool LoadFontResource(const MkPathName& filePath);

	// font type 생성
	// fontType : 해당 type의 key로 사용할 문자열
	// faceName : 폰트 리소스에 등록된 face 이름
	// size : 크기 point
	// thickness : 글자 두께. eThickness
	bool CreateFontType(const MkHashStr& fontType, const MkHashStr& faceName, int size, eThickness thickness);

	// font type 존재여부 반환
	inline bool CheckAvailableFontType(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType); }

	// font height 반환. 존재하지 않는 type이면 0 반환
	inline int GetFontHeight(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].size : 0; }

	// 폰트 color 생성
	// colorKey : 해당 색의 key로 사용할 문자열
	// r, g, b : 0 ~ 255 범위
	bool CreateFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b);

	// 폰트 state 생성
	// fontState : 해당 state의 key로 사용할 문자열
	// textColor : 문자열 색상
	// mode : eOutputMode
	// modeColor : outline이나 shadow 모드의 색상
	bool CreateFontState(const MkHashStr& fontState, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor);
	bool CreateFontState(const MkHashStr& fontState, D3DCOLOR textColor, eOutputMode mode, D3DCOLOR modeColor);

	// font state 존재여부 반환
	inline bool CheckAvailableFontState(const MkHashStr& fontState) const { return m_StateList.Exist(fontState); }

	// 해제
	void Clear(void);

	// default system font key 반환. type과 state에 사용
	const MkHashStr& DSF(void) const;

	// default highlight font key 반환. state에 사용
	const MkHashStr& DHF(void) const;

	//------------------------------------------------------------------------------------------------//
	// 텍스트 실제 사용
	//------------------------------------------------------------------------------------------------//

	// 텍스트 크기 계산
	// 실패하면 MkInt2(0, 0) 반환
	// (NOTE) 문장 중간에 삽입된 공문자(ex> L"\n 가 \n\n 나")는 작성자의 의도가 반영되었다고 해석 할 수 있지만
	// 마지막에 추가된 공문자(ex> L"가 \n")는 의미도 없을 뿐더러(출력되지 않음) 작성자의 실수일 가능성이 높으므로 무시됨
	MkInt2 GetTextSize(const MkHashStr& fontType, const MkStr& msg);

	// 한정된 크기 안에서 주어진 텍스트가 범위를 넘지 않도록 페이지 단위로 분할해 반환
	// 출력 영역 크기가 고정되어 있을 시 여러 페이지로 나누어 출력하거나 범위를 넘어서는 문자열을 잘라낼 때 사용
	// - 가로 길이 제한 : 길이를 넘어가면 개행이 이루어짐. 제한하지 않으려면 0 입력
	// - 세로 길이 제한 : 라인 수가 범위를 넘을 경우 이후의 문자열은 다음 페이지로 이동. 제한하지 않으려면 0 입력
	// (NOTE) 변환시 개행이 추가되거나 출력 형태에 맞게 변조하기 때문에(ex> 라인, 페이지에서 유효문자 이후의 의미 없는 공문자는 삭제 등)
	// pageBuffer의 합이 msg와 동일하지 않을 수 있음
	// (NOTE) restriction은 너비(x)와 높이(y) 제한값이 0이면 해당 부분의 제한은 하지 않음
	// (NOTE) restriction은 너비(x)와 높이(y) 제한이 만약 한 글자도 담지 못할 정도로 작더라도 최소한 글자 하나는 포함 됨
	//
	// ex>
	//	MkStr testStr = L"가나다\n라마바사아자차카타\n\n카타파하 으헤으   헤허허\n";
	//	// 맑은고딕 20 size에서 testStr 크기는 (170, 80). MkInt2(120, 50)로 제한하면 가로는 끝부분에서 개행이 이루어지고 한 페이지에 두 줄까지 들어감
	//
	//	MkArray<MkStr> pageBuf;
	//	if (MK_FONT_MGR.ConvertToPageUnderRestriction(L"맑은고딕20", testStr, MkInt2(120, 50), pageBuf))
	//	{
	//		// pageBuf[0] == L"가나다\n라마바사아자차카";
	//		// pageBuf[1] == L"타";
	//		// pageBuf[2] == L"카타파하 으헤으\n헤허허";
	//	}
	bool ConvertToPageUnderRestriction(const MkHashStr& fontType, const MkStr& msg, const MkInt2& restriction, MkArray<MkStr>& pageBuffer);

	// DSF로 텍스트 출력
	bool DrawMessage(const MkInt2& position, const MkStr& msg);

	// 세부 설정으로 텍스트 출력
	bool DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg);

	// type과 state로 텍스트 출력
	bool DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontState, const MkStr& msg);

	// MkDecoStr로 텍스트 출력
	bool DrawMessage(const MkInt2& position, const MkDecoStr& msg);

	//------------------------------------------------------------------------------------------------//
	// MkBaseResetableResource
	//------------------------------------------------------------------------------------------------//

	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

public:

	MkFontManager();
	virtual ~MkFontManager() { Clear(); }

protected:

	typedef struct __FontUnit
	{
		MkStr faceName;
		int size;
		unsigned int weight;

		LPD3DXFONT fontPtr;
	}
	_FontUnit;

	typedef struct __FontType
	{
		MkHashStr faceName;
		int size;
		eThickness thickness;
	}
	_FontType;

	typedef struct __FontState
	{
		D3DCOLOR textColor;
		eOutputMode mode;
		D3DCOLOR modeColor;
	}
	_FontState;

	// font key 생성
	void _MakeFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey) const;

	// 설정에 맞는 font key를 반환하고 없으면 생성해 반환
	bool _GetFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey);

	// 주어진 설정대로 unit 생성
	bool _CreateFont(const MkHashStr& fontKey, const MkHashStr& faceName, int size, eThickness thickness);
	LPD3DXFONT _CreateFont(const _FontUnit& fontUnit);

	MkInt2 _GetTextSize(const MkHashStr& fontKey, const MkStr& msg) const;
	MkInt2 _GetTextSize(const MkHashStr& fontKey, DWORD flag, const MkStr& msg) const;
	MkInt2 _GetTextSize(LPD3DXFONT fontPtr, DWORD flag, const MkStr& msg) const;

	unsigned int _FindCutSizeOfLine(LPD3DXFONT fontPtr, const MkStr& targetLine, int restrictionX) const;

	bool _DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		D3DCOLOR textColor, eOutputMode outputMode, D3DCOLOR modeColor, const MkStr& msg);

protected:

	LPDIRECT3DDEVICE9 m_Device;

	// face, size, thickness 순서로 저장된 font key 테이블
	MkHashMap<MkHashStr, MkMap<int, MkMap<eThickness, MkHashStr> > > m_FontKeyTable;

	// font key : unit. 실제 리소스와 DX 객체 저장
	MkMap<MkPathName, HANDLE> m_FontHandleList;
	MkHashMap<MkHashStr, _FontUnit> m_AvailableUnitList; // font key, unit

	// type
	MkHashMap<MkHashStr, _FontType> m_TypeList;
	
	// color
	MkHashMap<MkHashStr, D3DCOLOR> m_ColorList;
	D3DCOLOR m_ErrorColor;

	// state
	MkHashMap<MkHashStr, _FontState> m_StateList;
};
