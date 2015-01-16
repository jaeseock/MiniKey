
#include "MkCore_MkCheck.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
//#include "MkS2D_MkDecoStr.h"
#include "MkPA_MkFontManager.h"


#define MKDEF_PA_GET_COLOR(name) (m_ColorList.Exist(name) ? m_ColorList[name] : m_ErrorColor)

const MkHashStr MkFontManager::DefaultT(L"DefaultT");

const MkHashStr MkFontManager::BlackC(L"BlackC");
const MkHashStr MkFontManager::WhiteC(L"WhiteC");
const MkHashStr MkFontManager::LightGrayC(L"LightGrayC");
const MkHashStr MkFontManager::DarkGrayC(L"DarkGrayC");
const MkHashStr MkFontManager::RedC(L"RedC");
const MkHashStr MkFontManager::GreenC(L"GreenC");
const MkHashStr MkFontManager::BlueC(L"BlueC");
const MkHashStr MkFontManager::YellowC(L"YellowC");
const MkHashStr MkFontManager::CianC(L"CianC");
const MkHashStr MkFontManager::PinkC(L"PinkC");
const MkHashStr MkFontManager::VioletC(L"VioletC");
const MkHashStr MkFontManager::OrangeC(L"OrangeC");

const MkHashStr MkFontManager::DefaultS(L"DefaultS");
const MkHashStr MkFontManager::BlackS(L"BlackS");
const MkHashStr MkFontManager::WhiteS(L"WhiteS");
const MkHashStr MkFontManager::LightGrayS(L"LightGrayS");
const MkHashStr MkFontManager::DarkGrayS(L"DarkGrayS");
const MkHashStr MkFontManager::RedS(L"RedS");
const MkHashStr MkFontManager::GreenS(L"GreenS");
const MkHashStr MkFontManager::BlueS(L"BlueS");
const MkHashStr MkFontManager::YellowS(L"YellowS");
const MkHashStr MkFontManager::CianS(L"CianS");
const MkHashStr MkFontManager::PinkS(L"PinkS");
const MkHashStr MkFontManager::VioletS(L"VioletS");
const MkHashStr MkFontManager::OrangeS(L"OrangeS");

//------------------------------------------------------------------------------------------------//

void MkFontManager::SetUp(const MkDataNode* dataNode, const MkHashStr& fontTypeNodeKey)
{
	if (dataNode != NULL)
	{
		ChangeFontType(dataNode, fontTypeNodeKey);

		_LoadFontColors(dataNode->GetChildNode(L"FontColor"));
		_LoadFontStyles(dataNode->GetChildNode(L"FontStyle"));
	}
}

void MkFontManager::ChangeFontType(const MkDataNode* dataNode, const MkHashStr& fontTypeNodeKey)
{
	if (dataNode != NULL)
	{
		const MkDataNode* fontTypeNode = dataNode->GetChildNode(L"FontType");
		MK_CHECK(fontTypeNode != NULL, L"��Ʈ Ÿ�� ���� data node�� FontType ��尡 ����")
			return;

		const MkDataNode* targetNode = fontTypeNode->GetChildNode(fontTypeNodeKey);
		MK_CHECK(targetNode != NULL, L"FontType data node�� " + fontTypeNodeKey.GetString() + L" ��尡 ����")
			return;

		_LoadFontTypes(targetNode);
	}
}

bool MkFontManager::LoadFontResource(const MkPathName& filePath)
{
	if (m_FontHandleList.Exist(filePath))
		return true;

	MkByteArray buffer;
	MK_CHECK(MkFileManager::GetFileData(filePath, buffer), MkStr(filePath) + L" ��Ʈ ���� ���� ����")
		return false;
	
	DWORD fontCount = 0;
	HANDLE handle = AddFontMemResourceEx(buffer.GetPtr(), buffer.GetSize(), 0, &fontCount);
	MK_CHECK(handle != NULL, MkStr(filePath) + L" ��Ʈ ���� ��� ����")
		return false;

	m_FontHandleList.Create(filePath, handle);
	return true;
}

bool MkFontManager::CreateFontType(const MkHashStr& fontType, const MkHashStr& faceName, int size, eThickness thickness)
{
	MK_CHECK(!m_TypeList.Exist(fontType), L"�̹� �����ϴ� " + fontType.GetString() + L" font type ���� �õ�")
		return false;

	MkHashStr currFontKey;
	if (!_GetFontKey(faceName, size, thickness, currFontKey))
		return false;

	_FontType& ft = m_TypeList.Create(fontType);
	ft.faceName = faceName;
	ft.size = size;
	ft.thickness = thickness;
	ft.spaceSize = m_AvailableUnitList[currFontKey].spaceSize;
	ft.id = static_cast<int>(m_TypeID.GetSize());

	m_TypeID.PushBack(fontType);

	MK_DEV_PANEL.MsgToLog(L"> font type : " + fontType.GetString(), true);
	return true;
}

bool MkFontManager::CreateFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b)
{
	MK_CHECK(!m_ColorList.Exist(colorKey), L"�̹� �����ϴ� " + colorKey.GetString() + L" font color ���� �õ�")
		return false;

	unsigned int cr = Clamp<unsigned int>(r, 0, 255);
	unsigned int cg = Clamp<unsigned int>(g, 0, 255);
	unsigned int cb = Clamp<unsigned int>(b, 0, 255);
	m_ColorList.Create(colorKey, D3DCOLOR_XRGB(cr, cg, cb));

	MK_DEV_PANEL.MsgToLog(L"> font color : " + colorKey.GetString(), true);
	return true;
}

bool MkFontManager::CreateFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor)
{
	MK_CHECK(!m_StyleList.Exist(fontStyle), L"�̹� �����ϴ� " + fontStyle.GetString() + L" font style ���� �õ�")
		return false;

	_FontStyle& fs = m_StyleList.Create(fontStyle);
	fs.textColor = MKDEF_PA_GET_COLOR(textColor);
	fs.mode = mode;
	fs.modeColor = MKDEF_PA_GET_COLOR(modeColor);
	fs.id = static_cast<int>(m_StyleID.GetSize());

	m_StyleID.PushBack(fontStyle);

	MK_DEV_PANEL.MsgToLog(L"> font style : " + fontStyle.GetString(), true);
	return true;
}

void MkFontManager::Clear(void)
{
	_ClearFontType();

	m_ColorList.Clear();
	m_StyleList.Clear();
	m_StyleID.Clear();
}

MkInt2 MkFontManager::GetTextSize(const MkHashStr& fontType, const MkStr& msg, bool ignoreBacksideBlank)
{
	if (m_TypeList.Exist(fontType))
	{
		const _FontType& currFontType = m_TypeList[fontType];
		MkHashStr currFontKey;
		if (_GetFontKey(currFontType.faceName, currFontType.size, currFontType.thickness, currFontKey))
		{
			return _GetTextSize(m_AvailableUnitList[currFontKey], msg, ignoreBacksideBlank);
		}
	}
	return MkInt2(0, 0);
}

bool MkFontManager::RestrictSize(const MkHashStr& fontType, const MkStr& msg, const MkInt2& restriction, MkArray<MkStr>& pageBuffer)
{
	if (!m_TypeList.Exist(fontType))
		return false;

	if (msg.Empty())
	{
		pageBuffer.PushBack(); // empty page
		return true; // not error
	}

	if ((restriction.x <= 0) && (restriction.y <= 0))
	{
		pageBuffer.PushBack() = msg; // just copy
		return true; // not error
	}

	const _FontType& currFontType = m_TypeList[fontType];

	MkHashStr currFontKey;
	if (!_GetFontKey(currFontType.faceName, currFontType.size, currFontType.thickness, currFontKey))
		return false;

	const _FontUnit& currFontUnit = m_AvailableUnitList[currFontKey];
	MK_CHECK(currFontUnit.fontPtr != NULL, currFontKey.GetString() + L" ��Ʈ�� �����Ǿ� ���� ����")
		return false;

	// ���� ũ�� Ȯ��
	MkInt2 fixedRestriction;
	fixedRestriction.x = (restriction.y <= 0) ? 0 : restriction.x;
	fixedRestriction.y = (restriction.y <= 0) ? 0 : GetMax<int>(restriction.y, currFontUnit.height); // �ּ� ���̴� ��Ʈ ������
	bool widthRestriction = (fixedRestriction.x > 0);
	bool heightRestriction = (fixedRestriction.y > 0);

	// ���� ������ ������ ��� ������ ������ �߻��� �� �����Ƿ� �뷫���� ũ�⸦ ���
	if (heightRestriction)
	{
		MkInt2 nativeSize = _GetTextSize(currFontUnit, msg, true); // ��Ȯ�� �ʿ�� �����Ƿ� �ӵ��� ���� DX style�� ���

		// ���� ������ ���� ��� ���� ����, ���� ��� ���� ������ �Ǵ�
		int txtSize = (widthRestriction) ? (nativeSize.x * nativeSize.y) : nativeSize.y;
		int restrictionSize = (widthRestriction) ? (fixedRestriction.x * fixedRestriction.y) : fixedRestriction.y;
		float pageCount = static_cast<float>(txtSize) / static_cast<float>(restrictionSize);

		pageBuffer.Reserve(static_cast<unsigned int>(pageCount) + 2);
	}

	unsigned int currPos = 0;
	while (currPos < msg.GetSize())
	{
		// �� ������ ����
		MkStr& currPageBuffer = pageBuffer.PushBack();

		// ���� ������ ���� ��츸 �������� ���� ũ�� ���� �߻�
		if (widthRestriction)
		{
		}
		currPageBuffer.Reserve((msg.GetSize() - currPos) * 2); // �� �� ���� ���� Ȯ��

		int currPageHeight = currFontUnit.height;

		// ���� �� ���ڿ��� �����ְ� ���� ������ ���ų� ���� ���ϸ� ����
		while ((currPos < msg.GetSize()) && ((!heightRestriction) || (currPageHeight <= fixedRestriction.y)))
		{
			// currPos���� ���๮�ڳ� ���ڿ� �������� ���� �ϳ��� ����
			MkStr currLineBuffer;
			unsigned int linefeedPos = msg.GetFirstLineFeedPosition(currPos);
			bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
			if (endOfMsg) // ���ڿ� ����
			{
				msg.GetSubStr(MkArraySection(currPos), currLineBuffer); // ���� ���ڿ��� ���ۿ� ����
			}
			else if (linefeedPos > currPos)
			{
				msg.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // ���๮�ڱ����� ������ ���ۿ� ����
			}

			// ���޾� ���๮�ڰ� �� ���(linefeedPos == currPos) �� ���ڿ��� ���� �� �� ����
			if (currLineBuffer.Empty())
			{
				currPageBuffer += L"\n";
				++currPos;
			}
			// ���ڿ��� �����ϸ�
			else
			{
				// ���ڿ��� �� ���� �̻��̰� �ʺ� ������ ������ �� ��ȯ���̰� ������ ������ ���ѱ��̸�ŭ ������ ������ �� �κи� �߰�
				if ((currLineBuffer.GetSize() > 1) && (fixedRestriction.x > 0) &&
					(_GetTextSize(currFontUnit, currLineBuffer, true).x > fixedRestriction.x))
				{
					// ���� ���� �������̱� ������ ����ó�� �ϰ� ����� �Ұ���. �׳��� ������ ����Ž������ �Ǻ�
					unsigned int cutSize = _FindCutSizeOfLine(currFontUnit.fontPtr, currLineBuffer, fixedRestriction.x);

					MkStr safeLineToken;
					currLineBuffer.GetSubStr(MkArraySection(0, cutSize), safeLineToken);
					safeLineToken.RemoveRearSideBlank();
					currPageBuffer += safeLineToken;
					currPageBuffer += L"\n";

					currPos += cutSize; // �߸� ũ�⸸ŭ�� ����
				}
				// �� ���ڰų� ������ ���� ������ ���� ���� �״�� �߰�
				else
				{
					currLineBuffer.RemoveRearSideBlank();
					currPageBuffer += currLineBuffer;

					if (endOfMsg)
					{
						currPos = msg.GetSize(); // ���ڿ� ����
					}
					else
					{
						currPageBuffer += L"\n";
						currPos = linefeedPos + 1; // ���� ���� ������
					}
				}
			}

			currPageHeight += currFontUnit.height;
		}
	}

	// �� ������ �������� ���๮�ڸ� ����
	MK_INDEXING_LOOP(pageBuffer, i)
	{
		pageBuffer[i].RemoveRearSideBlank();
	}

	return true;
}

bool MkFontManager::DrawMessage(const MkInt2& position, const MkStr& msg)
{
	return DrawMessage(position, DefaultT, DefaultS, msg);
}

bool MkFontManager::DrawMessage
(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
 const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg)
{
	return _DrawMessage(position, faceName, size, thickness, MKDEF_PA_GET_COLOR(textColor), outputMode, MKDEF_PA_GET_COLOR(modeColor), msg);
}

bool MkFontManager::DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontStyle, const MkStr& msg)
{
	if (m_TypeList.Exist(fontType) && m_StyleList.Exist(fontStyle))
	{
		const _FontType& currFontType = m_TypeList[fontType];
		const _FontStyle& currFontStyle = m_StyleList[fontStyle];
		return _DrawMessage
			(position, currFontType.faceName, currFontType.size, currFontType.thickness, currFontStyle.textColor, currFontStyle.mode, currFontStyle.modeColor, msg);
	}
	return false;
}
/*
bool MkFontManager::DrawMessage(const MkInt2& position, const MkDecoStr& msg)
{
	MkHashStr currentType, currentStyle;

	const MkArray<MkDecoStr::SectorInfo>& outputList = msg.__GetOutputList();
	MK_INDEXING_LOOP(outputList, i)
	{
		const MkDecoStr::SectorInfo& si = outputList[i];
		if (!si.type.Empty())
		{
			currentType = si.type;
		}
		if (!si.style.Empty())
		{
			currentStyle = si.style;
		}

		if (!DrawMessage(position + si.position, currentType, currentStyle, si.text))
			return false;
	}
	return true;
}
*/
void MkFontManager::UnloadResource(void)
{
	MkHashMapLooper<MkHashStr, _FontUnit> looper(m_AvailableUnitList);
	MK_STL_LOOP(looper)
	{
		_FontUnit& fontUnit = looper.GetCurrentField();
		MK_RELEASE(fontUnit.fontPtr);
	}
}

void MkFontManager::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	MkHashMapLooper<MkHashStr, _FontUnit> looper(m_AvailableUnitList);
	MK_STL_LOOP(looper)
	{
		_FontUnit& fontUnit = looper.GetCurrentField();
		if (fontUnit.fontPtr == NULL)
		{
			fontUnit.fontPtr = _CreateFont(fontUnit);
		}
	}
}

MkFontManager::MkFontManager() : MkBaseResetableResource(), MkSingletonPattern<MkFontManager>()
{
	m_ErrorColor = MkColor::Pink.ConvertToD3DCOLOR();
}

//------------------------------------------------------------------------------------------------//

void MkFontManager::_LoadFontTypes(const MkDataNode* dataNode)
{
	_ClearFontType();

	if (dataNode != NULL)
	{
		MkArray<MkStr> fontFilePathBuffer;
		if (dataNode->GetData(L"FontFilePath", fontFilePathBuffer))
		{
			MK_INDEXING_LOOP(fontFilePathBuffer, i)
			{
				if (!LoadFontResource(fontFilePathBuffer[i]))
					return;
			}
		}

		MkArray<MkHashStr> fontTypeBuffer;
		if (dataNode->GetChildNodeList(fontTypeBuffer) > 0)
		{
			m_TypeID.Reserve(fontTypeBuffer.GetSize());

			fontTypeBuffer.SortInAscendingOrder(); // ����

			MK_INDEXING_LOOP(fontTypeBuffer, i)
			{
				const MkHashStr& currTypeName = fontTypeBuffer[i];
				const MkDataNode& typeNode = *dataNode->GetChildNode(currTypeName);

				MkStr faceName;
				unsigned int size;
				int thickness;
				MK_CHECK(typeNode.GetData(L"face", faceName, 0), currTypeName.GetString() + L" font type�� faceName �׸��� ����")
					continue;
				MK_CHECK(typeNode.GetData(L"size", size, 0), currTypeName.GetString() + L" font type�� size �׸��� ����")
					continue;
				MK_CHECK(typeNode.GetData(L"thickness", thickness, 0), currTypeName.GetString() + L" font type�� thickness �׸��� ����")
					continue;

				CreateFontType(currTypeName, faceName, size, static_cast<eThickness>(thickness));
			}

			m_TypeList.Rehash();
		}

		MK_CHECK(CheckAvailableFontType(DefaultT), DefaultT.GetString() + L" font type�� ��ϵǾ� ���� ����") {}
	}
}

void MkFontManager::_LoadFontColors(const MkDataNode* dataNode)
{
	m_ColorList.Clear();

	if (dataNode != NULL)
	{
		MkArray<MkHashStr> colorKeyList;
		dataNode->GetUnitKeyList(colorKeyList);
		MK_INDEXING_LOOP(colorKeyList, i)
		{
			const MkHashStr& colorName = colorKeyList[i];
			MkArray<unsigned int> rgb;
			if (dataNode->GetData(colorName, rgb))
			{
				MK_CHECK(rgb.GetSize() == 3, colorName.GetString() + L" font color�� rgb ���� ����� ���õǾ� ���� ����")
				{
					rgb.Fill(3, 0); // �⺻ ���� 0
				}

				CreateFontColor(colorName, rgb[0], rgb[1], rgb[2]);
			}
		}
		m_ColorList.Rehash();
	}

	// �⺻ font color ����
	_CreateDefaultFontColor(BlackC, 0, 0, 0);
	_CreateDefaultFontColor(WhiteC, 255, 255, 255);
	_CreateDefaultFontColor(LightGrayC, 170, 170, 170);
	_CreateDefaultFontColor(DarkGrayC, 85, 85, 85);
	_CreateDefaultFontColor(RedC, 255, 0, 0);
	_CreateDefaultFontColor(GreenC, 0, 255, 0);
	_CreateDefaultFontColor(BlueC, 0, 0, 255);
	_CreateDefaultFontColor(YellowC, 255, 255, 0);
	_CreateDefaultFontColor(CianC, 0, 255, 255);
	_CreateDefaultFontColor(PinkC, 255, 0, 255);
	_CreateDefaultFontColor(VioletC, 255, 102, 204);
	_CreateDefaultFontColor(OrangeC, 255, 153, 52);
}

void MkFontManager::_LoadFontStyles(const MkDataNode* dataNode)
{
	m_StyleList.Clear();
	m_StyleID.Clear();

	if (dataNode != NULL)
	{
		MkArray<MkHashStr> fontStyleBuffer;
		if (dataNode->GetChildNodeList(fontStyleBuffer) > 0)
		{
			m_StyleID.Reserve(fontStyleBuffer.GetSize());

			fontStyleBuffer.SortInAscendingOrder(); // ����

			MK_INDEXING_LOOP(fontStyleBuffer, i)
			{
				const MkHashStr& styleName = fontStyleBuffer[i];
				const MkDataNode& styleNode = *dataNode->GetChildNode(styleName);

				MkStr textColorBuf, modeColorBuf;
				int modeBuf;
				MK_CHECK(styleNode.GetData(L"textColor", textColorBuf, 0), styleName.GetString() + L" font style�� textColor �׸��� ����")
					continue;
				MK_CHECK(styleNode.GetData(L"mode", modeBuf, 0), styleName.GetString() + L" font style�� mode �׸��� ����")
					continue;
				MK_CHECK(styleNode.GetData(L"modeColor", modeColorBuf, 0), styleName.GetString() + L" font style�� modeColor �׸��� ����")
					continue;

				CreateFontStyle(styleName, textColorBuf, static_cast<eOutputMode>(modeBuf), modeColorBuf);
			}

			m_StyleList.Rehash();
		}

		MK_CHECK(CheckAvailableFontStyle(DefaultS), DefaultS.GetString() + L" font style�� ��ϵǾ� ���� ����") {}
	}

	// �⺻ font style ����
	_CreateDefaultFontStyle(BlackS, BlackC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(WhiteS, WhiteC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(LightGrayS, LightGrayC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(DarkGrayS, DarkGrayC, eShadow, BlackC);
	_CreateDefaultFontStyle(RedS, RedC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(GreenS, GreenC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(BlueS, BlueC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(YellowS, YellowC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(CianS, CianC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(PinkS, PinkC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(VioletS, VioletC, eShadow, DarkGrayC);
	_CreateDefaultFontStyle(OrangeS, OrangeC, eShadow, DarkGrayC);
}

void MkFontManager::_CreateDefaultFontColor(const MkHashStr& colorKey, unsigned int r, unsigned int g, unsigned int b)
{
	if (!m_ColorList.Exist(colorKey))
	{
		CreateFontColor(colorKey, r, g, b);
	}
}

void MkFontManager::_CreateDefaultFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor)
{
	if (!m_StyleList.Exist(fontStyle))
	{
		CreateFontStyle(fontStyle, textColor, mode, modeColor);
	}
}

bool MkFontManager::_GetFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey)
{
	int generateNewFont = 0;

	if (m_FontKeyTable.Exist(faceName))
	{
		MkMap<int, MkMap<eThickness, MkHashStr> >& sizeTable = m_FontKeyTable[faceName];
		if (sizeTable.Exist(size))
		{
			MkMap<eThickness, MkHashStr>& thicknessTable = sizeTable[size];
			if (thicknessTable.Exist(thickness))
			{
				fontKey = thicknessTable[thickness]; // assign
			}
			else
				generateNewFont = 3;
		}
		else
			generateNewFont = 2;
	}
	else
		generateNewFont = 1;

	if (generateNewFont > 0)
	{
		// key ���ڿ� ����
		MkStr buffer;
		buffer.Reserve(faceName.GetSize() + 4);
		buffer += faceName;
		buffer += size;
		switch (thickness)
		{
		case eThin: buffer += L"A"; break;
		case eMedium: buffer += L"B"; break;
		case eThick: buffer += L"C"; break;
		case eFull: buffer += L"D"; break;
		}
		MkHashStr newKey = buffer;

		// ��Ʈ ����
		MK_CHECK(_CreateFont(newKey, faceName, size, thickness), newKey.GetString() + L" ��Ʈ ���� ����")
			return false;

		// ���̺� ���
		MkMap<int, MkMap<eThickness, MkHashStr> >& sizeTable = (generateNewFont == 1) ? m_FontKeyTable.Create(faceName) : m_FontKeyTable[faceName];
		MkMap<eThickness, MkHashStr>& thicknessTable = (generateNewFont <= 2) ? sizeTable.Create(size) : sizeTable[size];
		thicknessTable.Create(thickness, newKey);

		// assign
		fontKey = newKey;
	}
	return true;
}

bool MkFontManager::_CreateFont(const MkHashStr& fontKey, const MkHashStr& faceName, int size, eThickness thickness)
{
	_FontUnit fu;
	fu.faceName = faceName;
	fu.height = size;
	switch (thickness)
	{
	case eThin: fu.weight = 400; break;
	case eMedium: fu.weight = 600; break;
	case eThick: fu.weight = 800; break;
	case eFull: fu.weight = 1000; break;
	}
	fu.fontPtr = _CreateFont(fu);
	bool ok = (fu.fontPtr != NULL);
	if (ok)
	{
		int onlyDotSize = _GetDXStyleTextSize(fu.fontPtr, DT_SINGLELINE, L".").x;
		int spaceAndDotSize = _GetDXStyleTextSize(fu.fontPtr, DT_SINGLELINE, L" .").x;
		fu.spaceSize = spaceAndDotSize - onlyDotSize;

		m_AvailableUnitList.Create(fontKey, fu);

		MK_DEV_PANEL.MsgToLog(L"> font unit : " + fontKey.GetString(), true);
	}
	return ok;
}

LPD3DXFONT MkFontManager::_CreateFont(const _FontUnit& fontUnit)
{
	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	MK_CHECK(device != NULL, L"NULL device. _CreateFont() failed")
		return NULL;

	// description
	D3DXFONT_DESC desc;
	desc.Height = fontUnit.height;
	desc.Width = 0; // auto
	desc.Weight = fontUnit.weight;
	desc.MipLevels = 1;
	desc.Italic = FALSE;
	desc.CharSet = DEFAULT_CHARSET;
	desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	desc.Quality = DEFAULT_QUALITY;
	desc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy_s(desc.FaceName, LF_FACESIZE, fontUnit.faceName.GetPtr());

	// font ����. ���� 1436 byte �Ҹ�
	LPD3DXFONT font = NULL;
	MK_CHECK(D3DXCreateFontIndirect(device, &desc, &font) == D3D_OK, fontUnit.faceName + L"(" + MkStr(fontUnit.height) + L") ��Ʈ ���� ����")
		return NULL;

	return font;
}

MkInt2 MkFontManager::_GetTextSize(const _FontUnit& funtUnit, const MkStr& msg, bool ignoreBacksideBlank) const
{
	if (msg.Empty() || (funtUnit.fontPtr == NULL))
		return MkInt2(0, (ignoreBacksideBlank) ? 0 : funtUnit.height);

	// DX style. �ǹ� ���� ������(���ڿ� �޺κ��� ��/���๮�� ����)
	if (ignoreBacksideBlank)
		return _GetDXStyleTextSize(funtUnit.fontPtr, msg.Exist(L"\n") ? DT_WORDBREAK : DT_SINGLELINE, msg);

	// msg�� ������ ���� size�� ����. ���ڿ� �޺κ��� ��(L' ')/����(L'\n') ���ڸ� ũ�⿡ �ݿ�
	// (NOTE) �� DX�� ���������� ��(L'\t')�� ����(L'\r')�� ���� ��
	MkArray<MkStr> lines;
	msg.Tokenize(lines, MkStr::LF, false); // �����ڸ� �츰 ���ε�� ����

	int longestX = 0;
	MK_INDEXING_LOOP(lines, i)
	{
		MkStr& currLine = lines[i];
		currLine.RemoveKeyword(MkStr::TAB); // tab ����
		currLine.RemoveKeyword(MkStr::CR); // carriage return ����
		if (!currLine.Empty())
		{
			int currWidth = _GetDXStyleTextSize(funtUnit.fontPtr, DT_SINGLELINE, currLine).x;
			currWidth += currLine.CountBacksideBlank() * funtUnit.spaceSize; // �޺κ��� ������ ũ�⸦ �߰�
			if (currWidth > longestX)
			{
				longestX = currWidth;
			}
		}
	}
	return MkInt2(longestX, lines.GetSize() * funtUnit.height);
}

MkInt2 MkFontManager::_GetDXStyleTextSize(LPD3DXFONT fontPtr, DWORD flag, const MkStr& msg) const
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	
	fontPtr->DrawTextW(NULL, msg.GetPtr(), msg.GetSize(), &rect, flag | DT_LEFT | DT_CALCRECT, 0);
	return MkInt2(rect.right, rect.bottom);
}

unsigned int MkFontManager::_FindCutSizeOfLine(LPD3DXFONT fontPtr, const MkStr& targetLine, int restrictionX) const
{
	// ���� ��� �� ���� ������ fontPtr�� ����, targetLine�� �� ���� �̻��̰� ��ȯ ���̰� restrictionX�� �ʰ��� ������

	// DrawText�� calcrect�� Ư���� �����ڷ� ����� ���ڿ��� ������ ��ȿ���� ���� �����ڵ��� �����Ѵ�
	// �ùٸ��� ģ���� ��������� ������ �� Ư�������� ��� ���ڿ��� ������ ������ ũ�⸦ �ջ��ϴ� ����� ��� �� �� ����
	// (ex> L"�� ��"�� ��� '��'�� '��'�� �������� 10, �������� �������� 5�� ���� �� �� ���ڱ��̴� 25������ ������ �ջ��ϸ� 20�� �ȴ�)
	// ���� ��ȿ���������� ó������ ���������� ������ ���ڿ��� ��� �˻��Ѵ�

	unsigned int lowerBound = 1; // ������ �ѱ��� �ʴ°� Ȯ�ǽ� �Ǵ� ������ ������ ũ��
	unsigned int upperBound = targetLine.GetSize(); // ������ �ѱ�°� Ȯ�ǽ� �Ǵ� ������ ������ ũ��
	while (true)
	{
		if (upperBound == (lowerBound + 1))
			break;

		unsigned int pivot = (lowerBound + upperBound) / 2;
		MkStr buffer;
		targetLine.GetSubStr(MkArraySection(0, pivot), buffer);
		int conversion = _GetDXStyleTextSize(fontPtr, DT_SINGLELINE, buffer).x;
		if (conversion < restrictionX)
		{
			lowerBound = pivot;
		}
		else if (conversion > restrictionX)
		{
			upperBound = pivot;
		}
		else
		{
			lowerBound = pivot;
			break;
		}
	}
	return lowerBound;
}

bool MkFontManager::_DrawMessage
(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness, D3DCOLOR textColor, eOutputMode outputMode, D3DCOLOR modeColor, const MkStr& msg)
{
	MkHashStr currFontKey;
	if (_GetFontKey(faceName, size, thickness, currFontKey))
	{
		LPD3DXFONT fontPtr = m_AvailableUnitList[currFontKey].fontPtr;
		MK_CHECK(fontPtr != NULL, currFontKey.GetString() + L" ��Ʈ�� �����Ǿ� ���� ����")
			return false;

		RECT rect;
		rect.left = position.x;
		rect.top = position.y;
		const wchar_t* msgPtr = msg.GetPtr();
		unsigned int size = msg.GetSize();

		DWORD flag = msg.Exist(MkStr::LF) ? DT_WORDBREAK : DT_SINGLELINE;
		flag |= DT_LEFT;
		fontPtr->DrawTextW(NULL, msgPtr, size, &rect, flag | DT_CALCRECT, textColor);

		if (outputMode == eOutline)
		{
			RECT tRect;

			tRect.left = rect.left;
			tRect.right = rect.right;
			tRect.top = rect.top - 1;
			tRect.bottom = rect.bottom - 1;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);

			tRect.top = rect.top + 1;
			tRect.bottom = rect.bottom + 1;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);

			tRect.left = rect.left - 1;
			tRect.right = rect.right - 1;
			tRect.top = rect.top;
			tRect.bottom = rect.bottom;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);

			tRect.left = rect.left + 1;
			tRect.right = rect.right + 1;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);
		}
		else if (outputMode == eShadow)
		{
			RECT tRect;
			tRect.left = rect.left + 1;
			tRect.right = rect.right + 1;
			tRect.top = rect.top + 1;
			tRect.bottom = rect.bottom + 1;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);

			++tRect.top;
			++tRect.bottom;
			fontPtr->DrawTextW(NULL, msgPtr, size, &tRect, flag, modeColor);
		}
		
		return (fontPtr->DrawTextW(NULL, msgPtr, size, &rect, flag, textColor) != 0);
	}
	return false;
}

void MkFontManager::_ClearFontType(void)
{
	UnloadResource();

	m_FontKeyTable.Clear();
	m_AvailableUnitList.Clear();

	MkMapLooper<MkPathName, HANDLE> looper(m_FontHandleList);
	MK_STL_LOOP(looper)
	{
		RemoveFontMemResourceEx(looper.GetCurrentField());
	}
	m_FontHandleList.Clear();
	
	m_TypeList.Clear();
	m_TypeID.Clear();
}

//------------------------------------------------------------------------------------------------//
