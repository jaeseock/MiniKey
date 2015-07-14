#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - D3D font manager
//
// * ���ο�
// - font unit : dx ��ü�� ������ �����ϴ� ����
// - font key : unit�� 1:1 �����ϴ� ������. face + size + thickness ����
//
// * ���� ���� ������
// - font type : ��Ʈ ���¸� ����. face + size + thickness
//   ��Ͻ�(CreateFontType) unit ��ü ����
//   ������ type�� �������� �ʰ� ���� face, size, thickness�� �־� ����� �� ������ ���� ������ ������
//   ���� ��ϵǾ� �ִ� type�� ���� �� ��� ���� unit�� ������
//   �ٸ� �̸��� type�� ���� ������ ������ ��쿡�� ���������� unit�� ������
// - font color : �÷��� ����
// - font style : � ������ �׷������� ����. text color + output mode + mode color
//
// �⺻ ���� type keyword
// - DefaultT
//   (NOTE) �ʱ�ȭ�� data node�� �ݵ�� �ش� �̸��� font type node�� ���ǵǾ� �־�� ��
//
// �⺻ ���� color
// - BlackC, WhiteC, LightGrayC, DarkGrayC, RedC, GreenC, BlueC, YellowC, CianC, PinkC, VioletC, OrangeC
//   (NOTE) �ʱ�ȭ�� data node�� �ش� �̸��� font color�� ���ǵǾ� ������ �װ� ����ϰ� ������ ��ü ����
//
// �⺻ ���� style
// - DefaultS
//   (NOTE) �ʱ�ȭ�� data node�� �ݵ�� �ش� �̸��� font style node�� ���ǵǾ� �־�� ��
// - BlackS, WhiteS, LightGrayS, DarkGrayS, RedS, GreenS, BlueS, YellowS, CianS, PinkS, VioletS, OrangeS
//   (NOTE) �ʱ�ȭ�� data node�� �ش� �̸��� font style�� ���ǵǾ� ������ �װ� ����ϰ� ������ ��ü ����
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

	// �ʱ�ȭ
	// L"FontType"->L"DefaultType" ���� ���� font type���� ����
	// (NOTE) ��ġ �ʱ�ȭ ���� ����
	void SetUp(const MkDataNode* dataNode);

	// �ش� �������� ��Ʈ ��ü
	// empty�� L"FontType"->L"DefaultType" ���� ����
	// (NOTE) font type�� ���µǰ� color�� style�� �״�� ������
	// ex>
	//	MkDataNode dataNode;
	//	if (dataNode.Load(L"MkRenderConfig.txt"))
	//	{
	//		MK_FONT_MGR.ChangeFontType(dataNode.GetChildNode(L"FontResource"), L"�������");
	//	}
	bool ChangeFontType(const MkDataNode* dataNode, const MkHashStr& fontTypeNodeKey);

	// ������ ����� ��Ʈ ������ ���� ��� ���
	// filePath : root directory ���� ��� Ȥ�� ���� ��Ʈ ���� ���
	// ex> LoadSystemFont(L"Font\\08SeoulNamsanM.ttf");
	bool LoadFontResource(const MkPathName& filePath);

	// font type ����
	// fontType : �ش� type�� key�� ����� ���ڿ�
	// faceName : ��Ʈ ���ҽ��� ��ϵ� face �̸�
	// size : ũ�� point
	// thickness : ���� �β�. eThickness
	bool CreateFontType(const MkHashStr& fontType, const MkHashStr& faceName, int size, eThickness thickness);

	// font type ���翩�� ��ȯ
	inline bool CheckAvailableFontType(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType); }

	// �ش� font type�� pixel height ��ȯ. �������� �ʴ� type�̸� 0 ��ȯ
	inline int GetFontHeight(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].size : 0; }

	// �ش� font type�� space size ��ȯ. �������� �ʴ� type�̸� 0 ��ȯ
	inline int GetFontSpaceSize(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].spaceSize : 0; }

	// �ش� font type�� id�� ��ȯ. �������� ������ -1 ��ȯ
	inline int GetFontTypeID(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].id : -1; }

	// �ش� id�� font type�� ��ȯ. �������� ������ EMPTY ��ȯ
	inline const MkHashStr& GetFontTypeName(int id) const { unsigned int i = static_cast<unsigned int>(id); return m_TypeID.IsValidIndex(i) ? m_TypeID[i] : MkHashStr::EMPTY; }

	// ��Ʈ color ����
	// colorKey : �ش� ���� key�� ����� ���ڿ�
	// r, g, b : 0 ~ 255 ����
	bool CreateFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b);

	// ��Ʈ style ����
	// fontStyle : �ش� style�� key�� ����� ���ڿ�
	// textColor : ���ڿ� ����
	// mode : eOutputMode
	// modeColor : outline�̳� shadow ����� ����
	bool CreateFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor);

	// font style ���翩�� ��ȯ
	inline bool CheckAvailableFontStyle(const MkHashStr& fontStyle) const { return m_StyleList.Exist(fontStyle); }

	// �ش� font style�� id�� ��ȯ. �������� ������ -1 ��ȯ
	inline int GetFontStyleID(const MkHashStr& fontStyle) const { return m_StyleList.Exist(fontStyle) ? m_StyleList[fontStyle].id : -1; }

	// �ش� id�� font style�� ��ȯ. �������� ������ EMPTY ��ȯ
	inline const MkHashStr& GetFontStyleName(int id) const { unsigned int i = static_cast<unsigned int>(id); return m_StyleID.IsValidIndex(i) ? m_StyleID[i] : MkHashStr::EMPTY; }

	// ����
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// ��Ʈ ���
	//------------------------------------------------------------------------------------------------//

	// �ؽ�Ʈ ũ�� ���
	// (NOTE) ��(L'\t')�� ����(L'\r') ���� ����
	// (NOTE) ���� �������� �߰��� �����ڳ� ���๮����(ex> L"�� \n\n     ") �ؼ��� ���� ũ�Ⱑ �޶��� �� ����
	// - ignoreBacksideBlank�� true�� ��� ��µ��� �ʴ� ���ǹ��� �κ��̶� ���� ������ ����(DX ��� �״��)
	//   ���ڿ��� ������� MkInt2::Zero ��ȯ
	// - ignoreBacksideBlank�� false�� ��� ��µ��� �ʴ��� �ۼ����� �ǵ��� �����ϰ� ��� �ݿ�
	//   ���� ���� ���ڸ� L"�� ", L"", L"     " ��� �� ���� ���ڿ��̶� ���� ��� ���
	//   ���ڿ��� ������� �̰� ���� �ǵ��� �����ϰ� MkInt2(0, ��Ʈ�� ����) ��ȯ
	MkInt2 GetTextSize(const MkHashStr& fontType, const MkStr& msg, bool ignoreBacksideBlank);

	// DefaultT/DefaultS �� �ؽ�Ʈ ���
	bool DrawMessage(const MkInt2& position, const MkStr& msg);

	// ���� �������� �ؽ�Ʈ ���
	bool DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg);

	// type�� style�� �ؽ�Ʈ ���
	bool DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontStyle, const MkStr& msg);

	//------------------------------------------------------------------------------------------------//
	// MkBaseResetableResource
	//------------------------------------------------------------------------------------------------//

	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	//------------------------------------------------------------------------------------------------//
	// MkTextNode�� Ư�� �Լ�
	//------------------------------------------------------------------------------------------------//

	unsigned int __FindSplitPosition(const MkHashStr& fontType, const MkStr& singleLine, int limitSize);

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

	// ������ �´� font key�� ��ȯ�ϰ� ������ ������ ��ȯ
	bool _GetFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey);

	// �־��� ������� unit ����
	bool _CreateFont(const MkHashStr& fontKey, const MkHashStr& faceName, int size, eThickness thickness);
	LPD3DXFONT _CreateFont(const _FontUnit& fontUnit);

	MkInt2 _GetTextSize(const _FontUnit& funtUnit, const MkStr& msg, bool ignoreBacksideBlank) const;
	MkInt2 _GetDXStyleTextSize(LPD3DXFONT fontPtr, DWORD flag, const wchar_t* ptr, int count) const;
	inline MkInt2 _GetDXStyleTextSize(LPD3DXFONT fontPtr, DWORD flag, const MkStr& msg) const { return _GetDXStyleTextSize(fontPtr, flag, msg.GetPtr(), msg.GetSize()); }

	unsigned int _FindCutSizeOfLine(LPD3DXFONT fontPtr, const MkStr& targetLine, int restrictionX) const;

	bool _DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		D3DCOLOR textColor, eOutputMode outputMode, D3DCOLOR modeColor, const MkStr& msg);

	void _ClearFontType(void);

protected:

	// face, size, thickness ������ ����� font key ���̺�
	MkHashMap<MkHashStr, MkMap<int, MkMap<eThickness, MkHashStr> > > m_FontKeyTable;

	// font key : unit. ���� ���ҽ��� DX ��ü ����
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

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkFontManager::_FontStyle)