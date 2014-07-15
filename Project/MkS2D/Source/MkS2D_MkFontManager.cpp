
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

	// 기본 font type 생성
	if (!CreateFontType(DSF(), L"맑은고딕", 12, eThin)) // GulimChe
		return false;

	// 기본 font state 생성
	if (!CreateFontState(DSF(), MkColor::White.ConvertToD3DCOLOR(), eShadow, MkColor::DarkGray.ConvertToD3DCOLOR()))
		return false;

	// 기본 font state 생성
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
				fontTypeBuffer.SortInAscendingOrder(); // 정렬

				MK_INDEXING_LOOP(fontTypeBuffer, i)
				{
					const MkHashStr& currTypeName = fontTypeBuffer[i];
					const MkDataNode& typeNode = *fontTypeNode->GetChildNode(currTypeName);

					MkStr faceName;
					unsigned int size;
					int thickness;
					MK_CHECK(typeNode.GetData(L"face", faceName, 0), currTypeName.GetString() + L" font type에 faceName 항목이 없음")
						return false;
					MK_CHECK(typeNode.GetData(L"size", size, 0), currTypeName.GetString() + L" font type에 size 항목이 없음")
						return false;
					MK_CHECK(typeNode.GetData(L"thickness", thickness, 0), currTypeName.GetString() + L" font type에 thickness 항목이 없음")
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
					MK_CHECK(rgb.GetSize() == 3, colorName.GetString() + L" font color의 rgb 값이 제대로 세팅되어 있지 않음")
					{
						rgb.Fill(3, 0); // 기본 값은 0
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
				fontStateBuffer.SortInAscendingOrder(); // 정렬

				MK_INDEXING_LOOP(fontStateBuffer, i)
				{
					const MkHashStr& stateName = fontStateBuffer[i];
					const MkDataNode& stateNode = *fontStateNode->GetChildNode(stateName);

					MkStr textColorBuf, modeColorBuf;
					int modeBuf;
					MK_CHECK(stateNode.GetData(L"textColor", textColorBuf, 0), stateName.GetString() + L" font state에 textColor 항목이 없음")
						return false;
					MK_CHECK(stateNode.GetData(L"mode", modeBuf, 0), stateName.GetString() + L" font state에 mode 항목이 없음")
						return false;
					MK_CHECK(stateNode.GetData(L"modeColor", modeColorBuf, 0), stateName.GetString() + L" font state에 modeColor 항목이 없음")
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
	MK_CHECK(MkFileManager::GetFileData(filePath, buffer), MkStr(filePath) + L" 폰트 파일 열기 실패")
		return false;
	
	DWORD fontCount = 0;
	HANDLE handle = AddFontMemResourceEx(buffer.GetPtr(), buffer.GetSize(), 0, &fontCount);
	MK_CHECK(handle != NULL, MkStr(filePath) + L" 폰트 파일 등록 실패")
		return false;

	m_FontHandleList.Create(filePath, handle);
	return true;
}

bool MkFontManager::CreateFontType(const MkHashStr& fontType, const MkHashStr& faceName, int size, eThickness thickness)
{
	MK_CHECK(!m_TypeList.Exist(fontType), L"이미 존재하는 " + fontType.GetString() + L" font type 생성 시도")
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
	MK_CHECK(!m_ColorList.Exist(colorKey), L"이미 존재하는 " + colorKey.GetString() + L" font color 생성 시도")
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
	MK_CHECK(!m_StateList.Exist(fontState), L"이미 존재하는 " + fontState.GetString() + L" font state 생성 시도")
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
	MK_CHECK(currFontUnit.fontPtr != NULL, currFontKey.GetString() + L" 폰트가 생성되어 있지 않음")
		return false;

	// 제한 크기 확정
	MkInt2 fixedRestriction;
	fixedRestriction.x = restriction.x;
	fixedRestriction.y = (restriction.y == 0) ? 0 : GetMax<int>(restriction.y, currFontUnit.size); // 최소 높이는 폰트 사이즈

	// 높이 제한이 존재할 경우 비율로 예상 페이지 크기를 계산(너비 제한은 단일 페이지만 필요)
	if (fixedRestriction.y > 0)
	{
		MkInt2 nativeSize = _GetTextSize(currFontKey, msg);
		float areaCost;
		if (fixedRestriction.x > 0) // 면적 비율
		{
			areaCost = static_cast<float>(nativeSize.x * nativeSize.y) / static_cast<float>(fixedRestriction.x * fixedRestriction.y);
		}
		else // 높이 비율
		{
			areaCost = static_cast<float>(nativeSize.y) / static_cast<float>(fixedRestriction.y);
		}
		pageBuffer.Reserve(static_cast<unsigned int>(areaCost) + 2);
	}

	unsigned int currPos = 0;
	while (currPos < msg.GetSize())
	{
		// 새 페이지 시작
		MkStr& currPageStr = pageBuffer.PushBack();
		currPageStr.Reserve((msg.GetSize() - currPos) * 2); // 두 배 여유 공간 확보

		int currPageHeight = currFontUnit.size;
		// 진행 할 문자열이 남아있거나 높이 제한이 있을 경우 제한 이하면 페이지 진행
		while ((currPos < msg.GetSize()) && ((fixedRestriction.y == 0) || (currPageHeight <= fixedRestriction.y)))
		{
			// currPos부터 개행문자나 문자열 끝까지의 라인 하나를 얻음
			MkStr currLineBuffer;
			unsigned int linefeedPos = msg.GetFirstLineFeedPosition(currPos);
			bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
			if (endOfMsg)
			{
				msg.GetSubStr(MkArraySection(currPos), currLineBuffer); // 문자열 끝까지의 라인(문자열 종료)
			}
			else if (linefeedPos > currPos)
			{
				msg.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // 개행문자까지의 라인
			}

			// 연달아 개행문자가 올 경우 빈 문자열이 지정 될 수 있음
			if (currLineBuffer.Empty())
			{
				currPageStr += L"\n";
				++currPos; // 개행문자 넘김
			}
			// 문자열이 존재하면
			else
			{
				// 문자열이 두 글자 이상이고 너비 제한이 존재할 때 변환길이가 제한을 넘으면 제한길이만큼 라인을 분할해 앞 부분만 추가
				if ((currLineBuffer.GetSize() > 1) && (fixedRestriction.x > 0) &&
					(_GetTextSize(currFontUnit.fontPtr, DT_SINGLELINE, currLineBuffer).x > fixedRestriction.x))
				{
					// 문자 폭이 제각각이기 때문에 높이처럼 일괄 계산이 불가능. 그나마 적당한 이진탐색으로 판별
					unsigned int cutSize = _FindCutSizeOfLine(currFontUnit.fontPtr, currLineBuffer, fixedRestriction.x);

					MkStr safeLineToken;
					currLineBuffer.GetSubStr(MkArraySection(0, cutSize), safeLineToken);
					safeLineToken.RemoveRearSideBlank();
					currPageStr += safeLineToken;
					currPageStr += L"\n";

					currPos += cutSize; // 잘린 크기만큼만 전진
				}
				// 한 글자거나 제한을 넘지 않으면 현재 라인 그대로 추가
				else
				{
					currLineBuffer.RemoveRearSideBlank();
					currPageStr += currLineBuffer;

					if (endOfMsg)
					{
						currPos = msg.GetSize(); // 문자열 종료
					}
					else
					{
						currPageStr += L"\n";
						currPos = linefeedPos + 1; // 다음 라인 시작점
					}
				}
			}

			currPageHeight += currFontUnit.size;
		}
	}

	// 각 페이지 마지막의 개행문자를 삭제
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

		MK_CHECK(_CreateFont(tmpKey, faceName, size, thickness), tmpKey.GetString() + L" 폰트 생성 실패")
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

	// font 생성. 개당 1436 byte 소모
	LPD3DXFONT font = NULL;
	MK_CHECK(D3DXCreateFontIndirect(m_Device, &desc, &font) == D3D_OK, fontUnit.faceName + L"(" + MkStr(fontUnit.size) + L") 폰트 생성 실패")
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
	MK_CHECK(fontPtr != NULL, fontKey.GetString() + L" 폰트가 생성되어 있지 않음")
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
	// 여기 들어 올 전제 조건은 fontPtr은 보장, targetLine은 두 글자 이상이고 변환 길이가 restrictionX를 초과한 상태임

	// DrawText의 calcrect의 특성상 공문자로 종료된 문자열은 마지막 유효문자 뒤의 공문자들을 무시한다
	// 올바르고 친절한 방식이지만 문제는 이 특성때문에 대상 문자열을 분할해 각각의 크기를 합산하는 방식을 사용 할 수 없다
	// (ex> L"가 나"의 경우 '가'와 '나'의 글자폭이 10, 공문자의 글자폭이 5라 했을 때 총 문자길이는 25이지만 분할해 합산하면 20이 된다)
	// 따라서 비효율적이지만 처음부터 범위까지의 누적된 문자열을 모두 검사한다

	unsigned int lowerBound = 1; // 제한을 넘기지 않는게 확실시 되는 추정상 마지막 크기
	unsigned int upperBound = targetLine.GetSize(); // 제한을 넘기는게 확실시 되는 추정상 최초의 크기
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
		MK_CHECK(fontPtr != NULL, currFontKey.GetString() + L" 폰트가 생성되어 있지 않음")
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
