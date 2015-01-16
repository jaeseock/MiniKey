
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
		MK_CHECK(fontTypeNode != NULL, L"폰트 타입 설정 data node에 FontType 노드가 없음")
			return;

		const MkDataNode* targetNode = fontTypeNode->GetChildNode(fontTypeNodeKey);
		MK_CHECK(targetNode != NULL, L"FontType data node에 " + fontTypeNodeKey.GetString() + L" 노드가 없음")
			return;

		_LoadFontTypes(targetNode);
	}
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
	ft.spaceSize = m_AvailableUnitList[currFontKey].spaceSize;
	ft.id = static_cast<int>(m_TypeID.GetSize());

	m_TypeID.PushBack(fontType);

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

bool MkFontManager::CreateFontStyle(const MkHashStr& fontStyle, const MkHashStr& textColor, eOutputMode mode, const MkHashStr& modeColor)
{
	MK_CHECK(!m_StyleList.Exist(fontStyle), L"이미 존재하는 " + fontStyle.GetString() + L" font style 생성 시도")
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
	MK_CHECK(currFontUnit.fontPtr != NULL, currFontKey.GetString() + L" 폰트가 생성되어 있지 않음")
		return false;

	// 제한 크기 확정
	MkInt2 fixedRestriction;
	fixedRestriction.x = (restriction.y <= 0) ? 0 : restriction.x;
	fixedRestriction.y = (restriction.y <= 0) ? 0 : GetMax<int>(restriction.y, currFontUnit.height); // 최소 높이는 폰트 사이즈
	bool widthRestriction = (fixedRestriction.x > 0);
	bool heightRestriction = (fixedRestriction.y > 0);

	// 높이 제한이 존재할 경우 페이지 분할이 발생할 수 있으므로 대략적인 크기를 계산
	if (heightRestriction)
	{
		MkInt2 nativeSize = _GetTextSize(currFontUnit, msg, true); // 정확할 필요는 없으므로 속도를 위해 DX style로 계산

		// 가로 제한이 있을 경우 면적 비율, 없을 경우 높이 비율로 판단
		int txtSize = (widthRestriction) ? (nativeSize.x * nativeSize.y) : nativeSize.y;
		int restrictionSize = (widthRestriction) ? (fixedRestriction.x * fixedRestriction.y) : fixedRestriction.y;
		float pageCount = static_cast<float>(txtSize) / static_cast<float>(restrictionSize);

		pageBuffer.Reserve(static_cast<unsigned int>(pageCount) + 2);
	}

	unsigned int currPos = 0;
	while (currPos < msg.GetSize())
	{
		// 새 페이지 시작
		MkStr& currPageBuffer = pageBuffer.PushBack();

		// 가로 제한이 있을 경우만 개행으로 인한 크기 변동 발생
		if (widthRestriction)
		{
		}
		currPageBuffer.Reserve((msg.GetSize() - currPos) * 2); // 두 배 여유 공간 확보

		int currPageHeight = currFontUnit.height;

		// 진행 할 문자열이 남아있고 높이 제한이 없거나 제한 이하면 진행
		while ((currPos < msg.GetSize()) && ((!heightRestriction) || (currPageHeight <= fixedRestriction.y)))
		{
			// currPos부터 개행문자나 문자열 끝까지의 라인 하나를 얻음
			MkStr currLineBuffer;
			unsigned int linefeedPos = msg.GetFirstLineFeedPosition(currPos);
			bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
			if (endOfMsg) // 문자열 종료
			{
				msg.GetSubStr(MkArraySection(currPos), currLineBuffer); // 남은 문자열을 버퍼에 복사
			}
			else if (linefeedPos > currPos)
			{
				msg.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // 개행문자까지의 라인을 버퍼에 복사
			}

			// 연달아 개행문자가 올 경우(linefeedPos == currPos) 빈 문자열이 지정 될 수 있음
			if (currLineBuffer.Empty())
			{
				currPageBuffer += L"\n";
				++currPos;
			}
			// 문자열이 존재하면
			else
			{
				// 문자열이 두 글자 이상이고 너비 제한이 존재할 때 변환길이가 제한을 넘으면 제한길이만큼 라인을 분할해 앞 부분만 추가
				if ((currLineBuffer.GetSize() > 1) && (fixedRestriction.x > 0) &&
					(_GetTextSize(currFontUnit, currLineBuffer, true).x > fixedRestriction.x))
				{
					// 문자 폭이 제각각이기 때문에 높이처럼 일괄 계산이 불가능. 그나마 적당한 이진탐색으로 판별
					unsigned int cutSize = _FindCutSizeOfLine(currFontUnit.fontPtr, currLineBuffer, fixedRestriction.x);

					MkStr safeLineToken;
					currLineBuffer.GetSubStr(MkArraySection(0, cutSize), safeLineToken);
					safeLineToken.RemoveRearSideBlank();
					currPageBuffer += safeLineToken;
					currPageBuffer += L"\n";

					currPos += cutSize; // 잘린 크기만큼만 전진
				}
				// 한 글자거나 제한을 넘지 않으면 현재 라인 그대로 추가
				else
				{
					currLineBuffer.RemoveRearSideBlank();
					currPageBuffer += currLineBuffer;

					if (endOfMsg)
					{
						currPos = msg.GetSize(); // 문자열 종료
					}
					else
					{
						currPageBuffer += L"\n";
						currPos = linefeedPos + 1; // 다음 라인 시작점
					}
				}
			}

			currPageHeight += currFontUnit.height;
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

			fontTypeBuffer.SortInAscendingOrder(); // 정렬

			MK_INDEXING_LOOP(fontTypeBuffer, i)
			{
				const MkHashStr& currTypeName = fontTypeBuffer[i];
				const MkDataNode& typeNode = *dataNode->GetChildNode(currTypeName);

				MkStr faceName;
				unsigned int size;
				int thickness;
				MK_CHECK(typeNode.GetData(L"face", faceName, 0), currTypeName.GetString() + L" font type에 faceName 항목이 없음")
					continue;
				MK_CHECK(typeNode.GetData(L"size", size, 0), currTypeName.GetString() + L" font type에 size 항목이 없음")
					continue;
				MK_CHECK(typeNode.GetData(L"thickness", thickness, 0), currTypeName.GetString() + L" font type에 thickness 항목이 없음")
					continue;

				CreateFontType(currTypeName, faceName, size, static_cast<eThickness>(thickness));
			}

			m_TypeList.Rehash();
		}

		MK_CHECK(CheckAvailableFontType(DefaultT), DefaultT.GetString() + L" font type이 등록되어 있지 않음") {}
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
				MK_CHECK(rgb.GetSize() == 3, colorName.GetString() + L" font color의 rgb 값이 제대로 세팅되어 있지 않음")
				{
					rgb.Fill(3, 0); // 기본 값은 0
				}

				CreateFontColor(colorName, rgb[0], rgb[1], rgb[2]);
			}
		}
		m_ColorList.Rehash();
	}

	// 기본 font color 생성
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

			fontStyleBuffer.SortInAscendingOrder(); // 정렬

			MK_INDEXING_LOOP(fontStyleBuffer, i)
			{
				const MkHashStr& styleName = fontStyleBuffer[i];
				const MkDataNode& styleNode = *dataNode->GetChildNode(styleName);

				MkStr textColorBuf, modeColorBuf;
				int modeBuf;
				MK_CHECK(styleNode.GetData(L"textColor", textColorBuf, 0), styleName.GetString() + L" font style에 textColor 항목이 없음")
					continue;
				MK_CHECK(styleNode.GetData(L"mode", modeBuf, 0), styleName.GetString() + L" font style에 mode 항목이 없음")
					continue;
				MK_CHECK(styleNode.GetData(L"modeColor", modeColorBuf, 0), styleName.GetString() + L" font style에 modeColor 항목이 없음")
					continue;

				CreateFontStyle(styleName, textColorBuf, static_cast<eOutputMode>(modeBuf), modeColorBuf);
			}

			m_StyleList.Rehash();
		}

		MK_CHECK(CheckAvailableFontStyle(DefaultS), DefaultS.GetString() + L" font style이 등록되어 있지 않음") {}
	}

	// 기본 font style 생성
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
		// key 문자열 생성
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

		// 폰트 생성
		MK_CHECK(_CreateFont(newKey, faceName, size, thickness), newKey.GetString() + L" 폰트 생성 실패")
			return false;

		// 테이블에 등록
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

	// font 생성. 개당 1436 byte 소모
	LPD3DXFONT font = NULL;
	MK_CHECK(D3DXCreateFontIndirect(device, &desc, &font) == D3D_OK, fontUnit.faceName + L"(" + MkStr(fontUnit.height) + L") 폰트 생성 실패")
		return NULL;

	return font;
}

MkInt2 MkFontManager::_GetTextSize(const _FontUnit& funtUnit, const MkStr& msg, bool ignoreBacksideBlank) const
{
	if (msg.Empty() || (funtUnit.fontPtr == NULL))
		return MkInt2(0, (ignoreBacksideBlank) ? 0 : funtUnit.height);

	// DX style. 의미 없는 공문자(문자열 뒷부분의 공/개행문자 무시)
	if (ignoreBacksideBlank)
		return _GetDXStyleTextSize(funtUnit.fontPtr, msg.Exist(L"\n") ? DT_WORDBREAK : DT_SINGLELINE, msg);

	// msg를 분해해 직접 size를 결정. 문자열 뒷부분의 공(L' ')/개행(L'\n') 문자를 크기에 반영
	// (NOTE) 단 DX와 마찬가지로 탭(L'\t')과 리턴(L'\r')은 무시 함
	MkArray<MkStr> lines;
	msg.Tokenize(lines, MkStr::LF, false); // 공문자를 살린 라인들로 분할

	int longestX = 0;
	MK_INDEXING_LOOP(lines, i)
	{
		MkStr& currLine = lines[i];
		currLine.RemoveKeyword(MkStr::TAB); // tab 제거
		currLine.RemoveKeyword(MkStr::CR); // carriage return 제거
		if (!currLine.Empty())
		{
			int currWidth = _GetDXStyleTextSize(funtUnit.fontPtr, DT_SINGLELINE, currLine).x;
			currWidth += currLine.CountBacksideBlank() * funtUnit.spaceSize; // 뒷부분의 공문자 크기를 추가
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
		MK_CHECK(fontPtr != NULL, currFontKey.GetString() + L" 폰트가 생성되어 있지 않음")
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
