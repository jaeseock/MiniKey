#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - D3D font manager
//
// * 내부용
// - font unit : dx 객체와 정보를 저장하는 공간
// - font key : unit과 1:1 대응하는 구분자. face + size + thickness 형태
//
// * 유저 정의 구분자
// - font type : 폰트 형태를 정의. face + size + thickness
//   등록시(CreateFontType) unit 객체 생성
//   별도의 type을 정의하지 않고도 직접 face, size, thickness를 주어 사용할 수 있으며 만약 동일한 설정이
//   기존 등록되어 있던 type에 존재 할 경우 같은 unit을 공유함
//   다른 이름의 type이 같은 설정을 공유할 경우에도 마찬가지로 unit은 공유됨
// - font color : 컬러값 정의
// - font style : 어떤 식으로 그려질지를 정의. text color + output mode + mode color
//
// 기본 정의 type keyword
// - DefaultT
//   (NOTE) 초기화용 data node에 반드시 해당 이름의 font type node가 정의되어 있어야 함
//
// 기본 정의 color
// - BlackC, WhiteC, LightGrayC, DarkGrayC, RedC, GreenC, BlueC, YellowC, CianC, PinkC, VioletC, OrangeC
//   (NOTE) 초기화용 data node에 해당 이름의 font color가 정의되어 있으면 그걸 사용하고 없으면 자체 생성
//
// 기본 정의 style
// - DefaultS
//   (NOTE) 초기화용 data node에 반드시 해당 이름의 font style node가 정의되어 있어야 함
// - BlackS, WhiteS, LightGrayS, DarkGrayS, RedS, GreenS, BlueS, YellowS, CianS, PinkS, VioletS, OrangeS
//   (NOTE) 초기화용 data node에 해당 이름의 font style가 정의되어 있으면 그걸 사용하고 없으면 자체 생성
//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkPA_MkD3DDefinition.h"
#include "MkPA_MkBaseResetableResource.h"


#define MK_FONT_MGR MkFontManager::Instance()


class MkColor;
class MkDataNode;
//class MkDecoStr;

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
		eThin = 0, // 40%
		eMedium, // 60%
		eThick, // 80%
		eFull // 100%
	};

public:

	// 초기화
	// (NOTE) 장치 초기화 이후 실행
	void SetUp(const MkDataNode* dataNode, const MkHashStr& fontTypeNodeKey);

	// 해당 설정으로 폰트 교체
	// (NOTE) font type만 리셋되고 color와 style는 그대로 유지됨
	void ChangeFontType(const MkDataNode* dataNode, const MkHashStr& fontTypeNodeKey);

	// 별도로 등록할 폰트 파일이 있을 경우 사용
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

	// 해당 font type의 pixel height 반환. 존재하지 않는 type이면 0 반환
	inline int GetFontHeight(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].size : 0; }

	// 해당 font type의 space size 반환. 존재하지 않는 type이면 0 반환
	inline int GetFontSpaceSize(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].spaceSize : 0; }

	// 해당 font type의 id를 반환. 존재하지 않으면 -1 반환
	inline int GetFontTypeID(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].id : -1; }

	// 해당 id의 font type을 반환. 존재하지 않으면 EMPTY 반환
	inline const MkHashStr& GetFontTypeName(int id) const { unsigned int i = static_cast<unsigned int>(id); return m_TypeID.IsValidIndex(i) ? m_TypeID[i] : MkHashStr::EMPTY; }

	// 폰트 color 생성
	// colorKey : 해당 색의 key로 사용할 문자열
	// r, g, b : 0 ~ 255 범위
	bool CreateFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b);

	// 폰트 style 생성
	// fontStyle : 해당 style의 key로 사용할 문자열
	// textColor : 문자열 색상
	// mode : eOutputMode
	// modeColor : outline이나 shadow 모드의 색상
	bool CreateFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor);

	// font style 존재여부 반환
	inline bool CheckAvailableFontStyle(const MkHashStr& fontStyle) const { return m_StyleList.Exist(fontStyle); }

	// 해당 font style의 id를 반환. 존재하지 않으면 -1 반환
	inline int GetFontStyleID(const MkHashStr& fontStyle) const { return m_StyleList.Exist(fontStyle) ? m_StyleList[fontStyle].id : -1; }

	// 해당 id의 font style을 반환. 존재하지 않으면 EMPTY 반환
	inline const MkHashStr& GetFontStyleName(int id) const { unsigned int i = static_cast<unsigned int>(id); return m_StyleID.IsValidIndex(i) ? m_StyleID[i] : MkHashStr::EMPTY; }

	// 해제
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// 폰트 사용
	//------------------------------------------------------------------------------------------------//

	// 텍스트 크기 계산
	// (NOTE) 탭(L'\t')과 리턴(L'\r') 문자 무시
	// (NOTE) 문장 마지막에 추가된 공문자나 개행문자의(ex> L"가 \n\n     ") 해석에 따라 크기가 달라질 수 있음
	// - ignoreBacksideBlank가 true일 경우 출력되지 않는 무의미한 부분이라 보고 적당히 무시(DX 방식 그대로)
	//   문자열이 비었으면 MkInt2(0, 0) 반환
	// - ignoreBacksideBlank가 false일 경우 출력되지 않더라도 작성자의 의도라 생각하고 모두 반영
	//   위의 예로 보자면 L"가 ", L"", L"     " 라는 세 줄의 문자열이라 보고 모두 계산
	//   문자열이 비었으면 이것 조차 의도라 생각하고 MkInt2(0, 폰트의 높이) 반환
	MkInt2 GetTextSize(const MkHashStr& fontType, const MkStr& msg, bool ignoreBacksideBlank);

	// 한정된 크기 안에서 주어진 텍스트가 범위를 넘지 않도록 개행하거나 페이지 단위로 분할해 반환
	// 출력 영역 크기가 고정되어 있을 시 여러 페이지로 나누어 출력하거나 범위를 넘어서는 문자열을 잘라낼 때 사용
	// - 가로 제한 : 길이를 넘어가면 개행이 이루어짐. 제한하지 않으려면 0 입력
	// - 세로 제한 : 라인 수가 범위를 넘을 경우 이후의 문자열은 다음 페이지로 이동. 제한하지 않으려면 0 입력
	// (NOTE) 제한 영역의 크기가 만약 한 글자도 담지 못할 정도로 작더라도 0보다 크면 최소한 글자 하나는 포함 됨
	//
	// ex>
	//	MkStr testStr = L"가나다\n라마바사아자차카타\n\n카타파하 으헤으   헤허허\n";
	//	// 맑은고딕 20 size에서 testStr 크기는 (170, 80). MkInt2(120, 50)로 제한하면 가로는 끝부분에서 개행이 이루어지고 한 페이지에 두 줄까지 들어감
	//
	//	MkArray<MkStr> pageBuf;
	//	if (MK_FONT_MGR.RestrictSize(L"맑은고딕20", testStr, MkInt2(120, 50), pageBuf))
	//	{
	//		// pageBuf[0] == L"가나다\n라마바사아자차카";
	//		// pageBuf[1] == L"타";
	//		// pageBuf[2] == L"카타파하 으헤으\n헤허허";
	//	}
	bool RestrictSize(const MkHashStr& fontType, const MkStr& msg, const MkInt2& restriction, MkArray<MkStr>& pageBuffer);

	// DefaultT/DefaultS 로 텍스트 출력
	bool DrawMessage(const MkInt2& position, const MkStr& msg);

	// 세부 설정으로 텍스트 출력
	bool DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg);

	// type과 style로 텍스트 출력
	bool DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontStyle, const MkStr& msg);

	// MkDecoStr로 텍스트 출력
	//bool DrawMessage(const MkInt2& position, const MkDecoStr& msg);

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
		int height;
		int spaceSize;
		unsigned int weight;

		LPD3DXFONT fontPtr;
	}
	_FontUnit;

	typedef struct __FontType
	{
		MkHashStr faceName;
		int size;
		eThickness thickness;
		int spaceSize;
		int id;
	}
	_FontType;

	typedef struct __FontStyle
	{
		D3DCOLOR textColor;
		eOutputMode mode;
		D3DCOLOR modeColor;
		int id;
	}
	_FontStyle;

	void _LoadFontTypes(const MkDataNode* dataNode);
	void _LoadFontColors(const MkDataNode* dataNode);
	void _LoadFontStyles(const MkDataNode* dataNode);

	void _CreateDefaultFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b);
	void _CreateDefaultFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor);

	// 설정에 맞는 font key를 반환하고 없으면 생성해 반환
	bool _GetFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey);

	// 주어진 설정대로 unit 생성
	bool _CreateFont(const MkHashStr& fontKey, const MkHashStr& faceName, int size, eThickness thickness);
	LPD3DXFONT _CreateFont(const _FontUnit& fontUnit);

	MkInt2 _GetTextSize(const _FontUnit& funtUnit, const MkStr& msg, bool ignoreBacksideBlank) const;
	MkInt2 _GetDXStyleTextSize(LPD3DXFONT fontPtr, DWORD flag, const MkStr& msg) const;

	unsigned int _FindCutSizeOfLine(LPD3DXFONT fontPtr, const MkStr& targetLine, int restrictionX) const;

	bool _DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		D3DCOLOR textColor, eOutputMode outputMode, D3DCOLOR modeColor, const MkStr& msg);

	void _ClearFontType(void);

protected:

	LPDIRECT3DDEVICE9 m_Device;

	// face, size, thickness 순서로 저장된 font key 테이블
	MkHashMap<MkHashStr, MkMap<int, MkMap<eThickness, MkHashStr> > > m_FontKeyTable;

	// font key : unit. 실제 리소스와 DX 객체 저장
	MkMap<MkPathName, HANDLE> m_FontHandleList;
	MkHashMap<MkHashStr, _FontUnit> m_AvailableUnitList; // font key, unit

	// type
	MkHashMap<MkHashStr, _FontType> m_TypeList;
	MkArray<MkHashStr> m_TypeID;
	
	// color
	MkHashMap<MkHashStr, D3DCOLOR> m_ColorList;
	D3DCOLOR m_ErrorColor;

	// style
	MkHashMap<MkHashStr, _FontStyle> m_StyleList;
	MkArray<MkHashStr> m_StyleID;

public:

	// default font type
	static const MkHashStr DefaultT;

	// default font color
	static const MkHashStr BlackC;
	static const MkHashStr WhiteC;
	static const MkHashStr LightGrayC;
	static const MkHashStr DarkGrayC;
	static const MkHashStr RedC;
	static const MkHashStr GreenC;
	static const MkHashStr BlueC;
	static const MkHashStr YellowC;
	static const MkHashStr CianC;
	static const MkHashStr PinkC;
	static const MkHashStr VioletC;
	static const MkHashStr OrangeC;

	// default font style
	static const MkHashStr DefaultS;
	static const MkHashStr BlackS;
	static const MkHashStr WhiteS;
	static const MkHashStr LightGrayS;
	static const MkHashStr DarkGrayS;
	static const MkHashStr RedS;
	static const MkHashStr GreenS;
	static const MkHashStr BlueS;
	static const MkHashStr YellowS;
	static const MkHashStr CianS;
	static const MkHashStr PinkS;
	static const MkHashStr VioletS;
	static const MkHashStr OrangeS;
};
