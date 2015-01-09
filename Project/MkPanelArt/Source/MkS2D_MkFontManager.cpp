
#include "MkCore_MkCheck.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkDecoStr.h"
#include "MkS2D_MkFontManager.h"


#define MKDEF_S2D_GET_COLOR(name) (m_ColorList.Exist(name) ? m_ColorList[name] : m_ErrorColor)


//------------------------------------------------------------------------------------------------//

bool MkFontManager::SetUp(LPDIRECT3DDEVICE9 device)
{
	MK_CHECK(device != NULL, L"NULL device")
		return false;

	m_Device = device;

	// �⺻ font type ����
	if (!CreateFontType(DSF(), L"�������", 12, eThin)) // GulimChe
		return false;

	// �⺻ font state ����
	if (!CreateFontState(DSF(), MkColor::White.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;

	// �⺻ font state ����
	if (!CreateFontState(BlackFS(), MkColor::Black.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(WhiteFS(), MkColor::White.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(LightGrayFS(), MkColor::LightGray.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(DarkGrayFS(), MkColor::DarkGray.ConvertToD3DCOLOR(), eShadow, MkColor::Black.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(RedFS(), MkColor::Red.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(GreenFS(), MkColor::Green.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(BlueFS(), MkColor::Blue.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(YellowFS(), MkColor::Yellow.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(CianFS(), MkColor::Cian.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(PinkFS(), MkColor::Pink.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(VioletFS(), MkColor::Violet.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;
	if (!CreateFontState(OrangeFS(), MkColor::Orange.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;

	return true;
}

bool MkFontManager::SetUp(LPDIRECT3DDEVICE9 device, const MkDataNode* dataNode)
{
	if (!SetUp(device))
		return false;

	if (dataNode != NULL)
	{
		// font type
		const MkDataNode* fontTypeNode = dataNode->GetChildNode(MKDEF_S2D_FONT_TYPE_NODE_NAME);
		if (fontTypeNode != NULL)
		{
			MkArray<MkStr> fontFilePathBuffer;
			if (fontTypeNode->GetData(L"FontFilePath", fontFilePathBuffer))
			{
				MK_INDEXING_LOOP(fontFilePathBuffer, i)
				{
					if (!LoadFontResource(fontFilePathBuffer[i]))
						return false;
				}
			}

			MkArray<MkHashStr> fontTypeBuffer;
			if (fontTypeNode->GetChildNodeList(fontTypeBuffer) > 0)
			{
				fontTypeBuffer.SortInAscendingOrder(); // ����

				MK_INDEXING_LOOP(fontTypeBuffer, i)
				{
					const MkHashStr& currTypeName = fontTypeBuffer[i];
					const MkDataNode& typeNode = *fontTypeNode->GetChildNode(currTypeName);

					MkStr faceName;
					unsigned int size;
					int thickness;
					MK_CHECK(typeNode.GetData(L"face", faceName, 0), currTypeName.GetString() + L" font type�� faceName �׸��� ����")
						return false;
					MK_CHECK(typeNode.GetData(L"size", size, 0), currTypeName.GetString() + L" font type�� size �׸��� ����")
						return false;
					MK_CHECK(typeNode.GetData(L"thickness", thickness, 0), currTypeName.GetString() + L" font type�� thickness �׸��� ����")
						return false;

					if (!CreateFontType(currTypeName, faceName, size, static_cast<eThickness>(thickness)))
						return false;
				}

				m_TypeList.Rehash();
			}
		}

		// font color
		const MkDataNode* fontColorNode = dataNode->GetChildNode(MKDEF_S2D_FONT_COLOR_NODE_NAME);
		if (fontColorNode != NULL)
		{
			MkArray<MkHashStr> colorKeyList;
			fontColorNode->GetUnitKeyList(colorKeyList);
			MK_INDEXING_LOOP(colorKeyList, i)
			{
				const MkHashStr& colorName = colorKeyList[i];
				MkArray<unsigned int> rgb;
				if (fontColorNode->GetData(colorName, rgb))
				{
					MK_CHECK(rgb.GetSize() == 3, colorName.GetString() + L" font color�� rgb ���� ����� ���õǾ� ���� ����")
					{
						rgb.Fill(3, 0); // �⺻ ���� 0
					}

					if (!CreateFontColor(colorName, rgb[0], rgb[1], rgb[2]))
						return false;
				}
			}
			m_ColorList.Rehash();
		}

		// font state
		const MkDataNode* fontStateNode = dataNode->GetChildNode(MKDEF_S2D_FONT_STATE_NODE_NAME);
		if (fontStateNode != NULL)
		{
			MkArray<MkHashStr> fontStateBuffer;
			if (fontStateNode->GetChildNodeList(fontStateBuffer) > 0)
			{
				fontStateBuffer.SortInAscendingOrder(); // ����

				MK_INDEXING_LOOP(fontStateBuffer, i)
				{
					const MkHashStr& stateName = fontStateBuffer[i];
					const MkDataNode& stateNode = *fontStateNode->GetChildNode(stateName);

					MkStr textColorBuf, modeColorBuf;
					int modeBuf;
					MK_CHECK(stateNode.GetData(L"textColor", textColorBuf, 0), stateName.GetString() + L" font state�� textColor �׸��� ����")
						return false;
					MK_CHECK(stateNode.GetData(L"mode", modeBuf, 0), stateName.GetString() + L" font state�� mode �׸��� ����")
						return false;
					MK_CHECK(stateNode.GetData(L"modeColor", modeColorBuf, 0), stateName.GetString() + L" font state�� modeColor �׸��� ����")
						return false;

					MkHashStr tcName = textColorBuf;
					MkHashStr mcName = modeColorBuf;

					if (!CreateFontState(stateName, textColorBuf, static_cast<eOutputMode>(modeBuf), modeColorBuf))
						return false;
				}

				m_StateList.Rehash();
			}
		}
	}

	return true;
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

bool MkFontManager::CreateFontState(const MkHashStr& fontState, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor)
{
	return CreateFontState(fontState, MKDEF_S2D_GET_COLOR(textColor), mode, MKDEF_S2D_GET_COLOR(modeColor));
}

bool MkFontManager::CreateFontState(const MkHashStr& fontState, D3DCOLOR textColor, eOutputMode mode, D3DCOLOR modeColor)
{
	MK_CHECK(!m_StateList.Exist(fontState), L"�̹� �����ϴ� " + fontState.GetString() + L" font state ���� �õ�")
		return false;

	_FontState& fs = m_StateList.Create(fontState);
	fs.textColor = textColor;
	fs.mode = mode;
	fs.modeColor = modeColor;

	MK_DEV_PANEL.MsgToLog(L"> font state : " + fontState.GetString(), true);

	return true;
}

const MkHashStr& MkFontManager::DSF(void) const
{
	const static MkHashStr defaultSystemFontKey = L"DSF";
	return defaultSystemFontKey;
}

const MkHashStr& MkFontManager::BlackFS(void) const
{
	const static MkHashStr colorState = L"BlackFS";
	return colorState;
}
const MkHashStr& MkFontManager::WhiteFS(void) const
{
	const static MkHashStr colorState = L"WhiteFS";
	return colorState;
}
const MkHashStr& MkFontManager::LightGrayFS(void) const
{
	const static MkHashStr colorState = L"LightGrayFS";
	return colorState;
}
const MkHashStr& MkFontManager::DarkGrayFS(void) const
{
	const static MkHashStr colorState = L"DarkGrayFS";
	return colorState;
}
const MkHashStr& MkFontManager::RedFS(void) const
{
	const static MkHashStr colorState = L"RedFS";
	return colorState;
}
const MkHashStr& MkFontManager::GreenFS(void) const
{
	const static MkHashStr colorState = L"GreenFS";
	return colorState;
}
const MkHashStr& MkFontManager::BlueFS(void) const
{
	const static MkHashStr colorState = L"BlueFS";
	return colorState;
}
const MkHashStr& MkFontManager::YellowFS(void) const
{
	const static MkHashStr colorState = L"YellowFS";
	return colorState;
}
const MkHashStr& MkFontManager::CianFS(void) const
{
	const static MkHashStr colorState = L"CianFS";
	return colorState;
}
const MkHashStr& MkFontManager::PinkFS(void) const
{
	const static MkHashStr colorState = L"PinkFS";
	return colorState;
}
const MkHashStr& MkFontManager::VioletFS(void) const
{
	const static MkHashStr colorState = L"VioletFS";
	return colorState;
}
const MkHashStr& MkFontManager::OrangeFS(void) const
{
	const static MkHashStr colorState = L"OrangeFS";
	return colorState;
}

void MkFontManager::Clear(void)
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
	m_ColorList.Clear();
	m_StateList.Clear();
}

MkInt2 MkFontManager::GetTextSize(const MkHashStr& fontType, const MkStr& msg)
{
	if (m_TypeList.Exist(fontType))
	{
		const _FontType& currFontType = m_TypeList[fontType];
		
		MkHashStr currFontKey;
		if (_GetFontKey(currFontType.faceName, currFontType.size, currFontType.thickness, currFontKey))
		{
			return _GetTextSize(currFontKey, msg);
		}
	}
	return MkInt2(0, 0);
}

bool MkFontManager::ConvertToPageUnderRestriction(const MkHashStr& fontType, const MkStr& msg, const MkInt2& restriction, MkArray<MkStr>& pageBuffer)
{
	if (!m_TypeList.Exist(fontType))
		return false;

	if (msg.Empty())
	{
		pageBuffer.PushBack(); // empty page
		return true; // not error
	}

	if ((restriction.x == 0) && (restriction.y == 0))
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
	fixedRestriction.x = restriction.x;
	fixedRestriction.y = (restriction.y == 0) ? 0 : GetMax<int>(restriction.y, currFontUnit.size); // �ּ� ���̴� ��Ʈ ������

	// ���� ������ ������ ��� ������ ���� ������ ũ�⸦ ���(�ʺ� ������ ���� �������� �ʿ�)
	if (fixedRestriction.y > 0)
	{
		MkInt2 nativeSize = _GetTextSize(currFontKey, msg);
		float areaCost;
		if (fixedRestriction.x > 0) // ���� ����
		{
			areaCost = static_cast<float>(nativeSize.x * nativeSize.y) / static_cast<float>(fixedRestriction.x * fixedRestriction.y);
		}
		else // ���� ����
		{
			areaCost = static_cast<float>(nativeSize.y) / static_cast<float>(fixedRestriction.y);
		}
		pageBuffer.Reserve(static_cast<unsigned int>(areaCost) + 2);
	}

	unsigned int currPos = 0;
	while (currPos < msg.GetSize())
	{
		// �� ������ ����
		MkStr& currPageStr = pageBuffer.PushBack();
		currPageStr.Reserve((msg.GetSize() - currPos) * 2); // �� �� ���� ���� Ȯ��

		int currPageHeight = currFontUnit.size;
		// ���� �� ���ڿ��� �����ְų� ���� ������ ���� ��� ���� ���ϸ� ������ ����
		while ((currPos < msg.GetSize()) && ((fixedRestriction.y == 0) || (currPageHeight <= fixedRestriction.y)))
		{
			// currPos���� ���๮�ڳ� ���ڿ� �������� ���� �ϳ��� ����
			MkStr currLineBuffer;
			unsigned int linefeedPos = msg.GetFirstLineFeedPosition(currPos);
			bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
			if (endOfMsg)
			{
				msg.GetSubStr(MkArraySection(currPos), currLineBuffer); // ���ڿ� �������� ����(���ڿ� ����)
			}
			else if (linefeedPos > currPos)
			{
				msg.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // ���๮�ڱ����� ����
			}

			// ���޾� ���๮�ڰ� �� ��� �� ���ڿ��� ���� �� �� ����
			if (currLineBuffer.Empty())
			{
				currPageStr += L"\n";
				++currPos; // ���๮�� �ѱ�
			}
			// ���ڿ��� �����ϸ�
			else
			{
				// ���ڿ��� �� ���� �̻��̰� �ʺ� ������ ������ �� ��ȯ���̰� ������ ������ ���ѱ��̸�ŭ ������ ������ �� �κи� �߰�
				if ((currLineBuffer.GetSize() > 1) && (fixedRestriction.x > 0) &&
					(_GetTextSize(currFontUnit.fontPtr, DT_SINGLELINE, currLineBuffer).x > fixedRestriction.x))
				{
					// ���� ���� �������̱� ������ ����ó�� �ϰ� ����� �Ұ���. �׳��� ������ ����Ž������ �Ǻ�
					unsigned int cutSize = _FindCutSizeOfLine(currFontUnit.fontPtr, currLineBuffer, fixedRestriction.x);

					MkStr safeLineToken;
					currLineBuffer.GetSubStr(MkArraySection(0, cutSize), safeLineToken);
					safeLineToken.RemoveRearSideBlank();
					currPageStr += safeLineToken;
					currPageStr += L"\n";

					currPos += cutSize; // �߸� ũ�⸸ŭ�� ����
				}
				// �� ���ڰų� ������ ���� ������ ���� ���� �״�� �߰�
				else
				{
					currLineBuffer.RemoveRearSideBlank();
					currPageStr += currLineBuffer;

					if (endOfMsg)
					{
						currPos = msg.GetSize(); // ���ڿ� ����
					}
					else
					{
						currPageStr += L"\n";
						currPos = linefeedPos + 1; // ���� ���� ������
					}
				}
			}

			currPageHeight += currFontUnit.size;
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
	return DrawMessage(position, DSF(), WhiteFS(), msg);
}

bool MkFontManager::DrawMessage
(const MkInt2& position, const MkHashStr& faceName, int size, eThickness thickness,
 const MkHashStr& textColor, eOutputMode outputMode, const MkHashStr& modeColor, const MkStr& msg)
{
	return _DrawMessage(position, faceName, size, thickness, MKDEF_S2D_GET_COLOR(textColor), outputMode, MKDEF_S2D_GET_COLOR(modeColor), msg);
}

bool MkFontManager::DrawMessage(const MkInt2& position, const MkHashStr& fontType, const MkHashStr& fontState, const MkStr& msg)
{
	if (m_TypeList.Exist(fontType) && m_StateList.Exist(fontState))
	{
		const _FontType& currFontType = m_TypeList[fontType];
		const _FontState& currFontState = m_StateList[fontState];
		return _DrawMessage
			(position, currFontType.faceName, currFontType.size, currFontType.thickness, currFontState.textColor, currFontState.mode, currFontState.modeColor, msg);
	}
	return false;
}

bool MkFontManager::DrawMessage(const MkInt2& position, const MkDecoStr& msg)
{
	MkHashStr currentType, currentState;

	const MkArray<MkDecoStr::SectorInfo>& outputList = msg.__GetOutputList();
	MK_INDEXING_LOOP(outputList, i)
	{
		const MkDecoStr::SectorInfo& si = outputList[i];
		if (!si.type.Empty())
		{
			currentType = si.type;
		}
		if (!si.state.Empty())
		{
			currentState = si.state;
		}

		if (!DrawMessage(position + si.position, currentType, currentState, si.text))
			return false;
	}
	return true;
}

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
	m_Device = device;

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
	m_Device = NULL;
	m_ErrorColor = MkColor::Pink.ConvertToD3DCOLOR();
}

//------------------------------------------------------------------------------------------------//

void MkFontManager::_MakeFontKey(const MkHashStr& faceName, int size, eThickness thickness, MkHashStr& fontKey) const
{
	MkStr buffer;
	buffer.Reserve(faceName.GetSize() + 4);
	buffer = faceName;
	buffer += size;
	switch (thickness)
	{
	case eThin: buffer += L"A"; break;
	case eMedium: buffer += L"B"; break;
	case eThick: buffer += L"C"; break;
	case eFull: buffer += L"D"; break;
	}
	fontKey = buffer;
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
				fontKey = thicknessTable[thickness];
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
		MkHashStr tmpKey;
		_MakeFontKey(faceName, size, thickness, tmpKey);

		MK_CHECK(_CreateFont(tmpKey, faceName, size, thickness), tmpKey.GetString() + L" ��Ʈ ���� ����")
			return false;

		fontKey = tmpKey;

		MkMap<int, MkMap<eThickness, MkHashStr> >& sizeTable = (generateNewFont == 1) ? m_FontKeyTable.Create(faceName) : m_FontKeyTable[faceName];
		MkMap<eThickness, MkHashStr>& thicknessTable = (generateNewFont <= 2) ? sizeTable.Create(size) : sizeTable[size];
		thicknessTable.Create(thickness, tmpKey);
	}
	return true;
}

bool MkFontManager::_CreateFont(const MkHashStr& fontKey, const MkHashStr& faceName, int size, eThickness thickness)
{
	_FontUnit fu;
	fu.faceName = faceName;
	fu.size = size;
	switch (thickness)
	{
	case eThin: fu.weight = 300; break;
	case eMedium: fu.weight = 500; break;
	case eThick: fu.weight = 700; break;
	case eFull: fu.weight = 1000; break;
	}
	fu.fontPtr = _CreateFont(fu);
	bool ok = (fu.fontPtr != NULL);
	if (ok)
	{
		m_AvailableUnitList.Create(fontKey, fu);

		MK_DEV_PANEL.MsgToLog(L"> font unit : " + fontKey.GetString(), true);
	}
	return ok;
}

LPD3DXFONT MkFontManager::_CreateFont(const _FontUnit& fontUnit)
{
	// description
	D3DXFONT_DESC desc;
	desc.Height = fontUnit.size;
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
	MK_CHECK(D3DXCreateFontIndirect(m_Device, &desc, &font) == D3D_OK, fontUnit.faceName + L"(" + MkStr(fontUnit.size) + L") ��Ʈ ���� ����")
		return NULL;

	return font;
}

MkInt2 MkFontManager::_GetTextSize(const MkHashStr& fontKey, const MkStr& msg) const
{
	return _GetTextSize(fontKey, msg.Exist(L"\n") ? DT_WORDBREAK : DT_SINGLELINE, msg);
}

MkInt2 MkFontManager::_GetTextSize(const MkHashStr& fontKey, DWORD flag, const MkStr& msg) const
{
	LPD3DXFONT fontPtr = m_AvailableUnitList[fontKey].fontPtr;
	MK_CHECK(fontPtr != NULL, fontKey.GetString() + L" ��Ʈ�� �����Ǿ� ���� ����")
		return MkInt2(0, 0);

	return _GetTextSize(fontPtr, flag, msg);
}

MkInt2 MkFontManager::_GetTextSize(LPD3DXFONT fontPtr, DWORD flag, const MkStr& msg) const
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
		int conversion = _GetTextSize(fontPtr, DT_SINGLELINE, buffer).x;
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

		DWORD flag = msg.Exist(L"\n") ? DT_WORDBREAK : DT_SINGLELINE;
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

//------------------------------------------------------------------------------------------------//
