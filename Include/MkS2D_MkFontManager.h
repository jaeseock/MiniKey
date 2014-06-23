#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - D3D font manager
//
// * ���ο�
// - font unit : dx ��ü�� ������ �����ϴ� ����
// - font key : unit�� 1:1 �����ϴ� ������. face + size + thickness
//
// * ���� ���� ������
// - font type : ��Ʈ ���¸� ����. face + size + thickness
//   ��Ͻ�(CreateFontType) unit ��ü ����
//   ������ type�� �������� �ʰ� ���� face, size, thickness�� �־� ����� �� ������ ���� ������ ������
//   ���� ��ϵǾ� �ִ� type�� ���� �� ��� ���� unit�� ������
//   �ٸ� �̸��� type�� ���� ������ ������ ��쿡�� ���������� unit�� ������
// - font color : �÷��� ����
// - font state : � ������ �׷������� ����. text color + output mode + mode color
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

	// �ʱ�ȭ. ���� ���� �ʱ�ȭ ������ �ʿ�
	bool SetUp(LPDIRECT3DDEVICE9 device);

	// MkDataNode�� �������� �ʱ�ȭ
	bool SetUp(LPDIRECT3DDEVICE9 device, const MkDataNode* dataNode);

	// ������ ����� �ý��� ��Ʈ�� ���� ��� ���
	// Clear() ȣ�� �� ������
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

	// font height ��ȯ. �������� �ʴ� type�̸� 0 ��ȯ
	inline int GetFontHeight(const MkHashStr& fontType) const { return m_TypeList.Exist(fontType) ? m_TypeList[fontType].size : 0; }

	// ��Ʈ color ����
	// colorKey : �ش� ���� key�� ����� ���ڿ�
	// r, g, b : 0 ~ 255 ����
	bool CreateFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b);

	// ��Ʈ state ����
	// fontState : �ش� state�� key�� ����� ���ڿ�
	// textColor : ���ڿ� ����
	// mode : eOutputMode
	// modeColor : outline�̳� shadow ����� ����
	bool CreateFontState(const MkHashStr& fontState, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor);
	bool CreateFontState(const MkHashStr& fontState, D3DCOLOR textColor, eOutputMode mode, D3DCOLOR modeColor);

	// font state ���翩�� ��ȯ
	inline bool CheckAvailableFontState(const MkHashStr& fontState) const { return m_StateList.Exist(fontState); }

	// ����
	void Clear(void);

	// default system font key ��ȯ. type�� state�� ���
	const MkHashStr& DSF(void) const;

	// default highlight font key ��ȯ. state�� ���
	const MkHashStr& DHF(void) const;

	//------------------------------------------------------------------------------------------------//
	// �ؽ�Ʈ ���� ���
	//------------------------------------------------------------------------------------------------//

	// �ؽ�Ʈ ũ�� ���
	// �����ϸ� MkInt2(0, 0) ��ȯ
	// (NOTE) ���� �߰��� ���Ե� ������(ex> L"\n �� \n\n ��")�� �ۼ����� �ǵ��� �ݿ��Ǿ��ٰ� �ؼ� �� �� ������
	// �������� �߰��� ������(ex> L"�� \n")�� �ǹ̵� ���� �Ӵ���(��µ��� ����) �ۼ����� �Ǽ��� ���ɼ��� �����Ƿ� ���õ�
	MkInt2 GetTextSize(const MkHashStr& fontType, const MkStr& msg);

	// ������ ũ�� �ȿ��� �־��� �ؽ�Ʈ�� ������ ���� �ʵ��� ������ ������ ������ ��ȯ
	// ��� ���� ũ�Ⱑ �����Ǿ� ���� �� ���� �������� ������ ����ϰų� ������ �Ѿ�� ���ڿ��� �߶� �� ���
	// - ���� ���� ���� : ���̸� �Ѿ�� ������ �̷����. �������� �������� 0 �Է�
	// - ���� ���� ���� : ���� ���� ������ ���� ��� ������ ���ڿ��� ���� �������� �̵�. �������� �������� 0 �Է�
	// (NOTE) ��ȯ�� ������ �߰��ǰų� ��� ���¿� �°� �����ϱ� ������(ex> ����, ���������� ��ȿ���� ������ �ǹ� ���� �����ڴ� ���� ��)
	// pageBuffer�� ���� msg�� �������� ���� �� ����
	// (NOTE) restriction�� �ʺ�(x)�� ����(y) ���Ѱ��� 0�̸� �ش� �κ��� ������ ���� ����
	// (NOTE) restriction�� �ʺ�(x)�� ����(y) ������ ���� �� ���ڵ� ���� ���� ������ �۴��� �ּ��� ���� �ϳ��� ���� ��
	//
	// ex>
	//	MkStr testStr = L"������\n�󸶹ٻ������īŸ\n\nīŸ���� ������   ������\n";
	//	// ������� 20 size���� testStr ũ��� (170, 80). MkInt2(120, 50)�� �����ϸ� ���δ� ���κп��� ������ �̷������ �� �������� �� �ٱ��� ��
	//
	//	MkArray<MkStr> pageBuf;
	//	if (MK_FONT_MGR.ConvertToPageUnderRestriction(L"�������20", testStr, MkInt2(120, 50), pageBuf))
	//	{
	//		// pageBuf[0] == L"������\n�󸶹ٻ������ī";
	//		// pageBuf[1] == L"Ÿ";
	//		// pageBuf[2] == L"īŸ���� ������\n������";
	//	}
	bool ConvertToPageUnderRestriction(const MkHashStr& fontType, const MkStr& msg, const MkInt2& restriction, MkArray<MkStr>& pageBuffer);

	// DSF�� �ؽ�Ʈ ���
	bool DrawMessage(const MkInt2& position, const MkStr& msg);

	// ���� �������� �ؽ�Ʈ ���
	bool DrawMessage
		(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
		const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg);

	// type�� state�� �ؽ�Ʈ ���
	bool DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontState, const MkStr& msg);

	// MkDecoStr�� �ؽ�Ʈ ���
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

	// font key ����
	void _MakeFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey) const;

	// ������ �´� font key�� ��ȯ�ϰ� ������ ������ ��ȯ
	bool _GetFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey);

	// �־��� ������� unit ����
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

	// face, size, thickness ������ ����� font key ���̺�
	MkHashMap<MkHashStr, MkMap<int, MkMap<eThickness, MkHashStr> > > m_FontKeyTable;

	// font key : unit. ���� ���ҽ��� DX ��ü ����
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
