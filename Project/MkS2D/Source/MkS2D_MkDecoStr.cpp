
#include "MkCore_MkCheck.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkDecoStr.h"

const MkDecoStr MkDecoStr::Null(L"");

const static MkStr gTagBegin = MKDEF_S2D_DECOSTR_TAG_BEGIN;
const static MkStr gTagEnd = MKDEF_S2D_DECOSTR_TAG_END;
const static MkStr gTagSeperator = MKDEF_S2D_DECOSTR_TAG_SEPERATOR;
const static MkStr gTypeKeyword = MKDEF_S2D_DECOSTR_TYPE_KEYWORD;
const static MkStr gStateKeyword = MKDEF_S2D_DECOSTR_STATE_KEYWORD;
const static MkStr gLineFeedSizeKeyword = MKDEF_S2D_DECOSTR_LFS_KEYWORD;

void MkDecoStr::SetUp(const MkStr& source)
{
	Clear();

	if (source.Empty())
		return;
	
	MkStr nativeBuffer;
	nativeBuffer.Reserve(source.GetSize());

	MkMap<unsigned int, SectorInfo> sectorInfos;
	
	MkStr lastFontType;
	unsigned int lastTypePos = 0;

	MkStr lastFontState;

	int lineFeedSize = 0;

	// 원본(source)에서 tag와 문자열을 분리해 저장
	unsigned int position = 0;
	while (position < source.GetSize())
	{
		MkStr tagBuffer, strBuffer;
		unsigned int tagEndPos = source.GetFirstBlock(position, gTagBegin, gTagEnd, tagBuffer);

		// end of source
		if (tagEndPos == MKDEF_ARRAY_ERROR)
		{
			source.GetSubStr(MkArraySection(position), strBuffer);
			nativeBuffer += strBuffer;
			position = source.GetSize();
		}
		// tag exist
		else
		{
			unsigned int strSize = tagEndPos - gTagBegin.GetSize() - tagBuffer.GetSize() - gTagEnd.GetSize() - position;
			source.GetSubStr(MkArraySection(position, strSize), strBuffer);
			nativeBuffer += strBuffer;
			position = tagEndPos;
			unsigned int tagPos = nativeBuffer.GetSize();

			// parse tag
			MkArray<MkStr> tokens;
			if (tagBuffer.Tokenize(tokens, gTagSeperator) == 2)
			{
				const MkStr& tagKey = tokens[0];
				const MkStr& tagValue = tokens[1];

				// font type
				if ((tagKey == gTypeKeyword) && (tagValue != lastFontType) && (MK_FONT_MGR.CheckAvailableFontType(tagValue)))
				{
					if (tagPos > lastTypePos)
					{
						if (nativeBuffer[tagPos - 1] != L'\n') // 바로 직전 글자가 line feed가 아니면 강제 개행
						{
							nativeBuffer += L"\n";
							++tagPos;
						}
						lastTypePos = tagPos;
					}

					_SetSector(sectorInfos, tagPos, tagValue, L"");

					lastFontType = tagValue;
				}
				// font state
				else if ((tagKey == gStateKeyword) && (tagValue != lastFontState) && (MK_FONT_MGR.CheckAvailableFontState(tagValue)))
				{
					_SetSector(sectorInfos, tagPos, L"", tagValue);

					lastFontState = tagValue;
				}
				// line feed size
				else if ((tagKey == gLineFeedSizeKeyword) && tagValue.IsDigit())
				{
					lineFeedSize = tagValue.ToInteger();
				}
			}
		}
	}

	if (!nativeBuffer.Empty())
	{
		// tag별 대상 문자열 추출
		if (sectorInfos.Empty())
		{
			_SetSector(sectorInfos, 0, MK_FONT_MGR.DSF(), MK_FONT_MGR.DSF()); // DSF
			sectorInfos[0].text = nativeBuffer;
		}
		else
		{
			unsigned int firstPos = sectorInfos.GetFirstKey();
			if (firstPos == 0)
			{
				SectorInfo& firstSector = sectorInfos[0];
				if (firstSector.type.Empty())
				{
					firstSector.type = MK_FONT_MGR.DSF();
				}
				if (firstSector.state.Empty())
				{
					firstSector.state = MK_FONT_MGR.DSF();
				}
			}
			else
			{
				_SetSector(sectorInfos, 0, MK_FONT_MGR.DSF(), MK_FONT_MGR.DSF()); // DSF
			}

			unsigned int lastPos = sectorInfos.GetLastKey();
			MkMapLooper<unsigned int, SectorInfo> looper(sectorInfos);
			MK_STL_LOOP(looper)
			{
				const unsigned int& currPos = looper.GetCurrentKey();
				if (currPos == lastPos)
				{
					nativeBuffer.GetSubStr(MkArraySection(currPos), looper.GetCurrentField().text);
				}
				else
				{
					std::map<unsigned int, SectorInfo>::const_iterator nextItr = looper.GetCurrentIterator();
					++nextItr;
					nativeBuffer.GetSubStr(MkArraySection(currPos, nextItr->first - currPos), looper.GetCurrentField().text);
				}
			}
		}

		// line feed 기준으로 문자열 분리 및 좌표 생성
		unsigned int linefeedCount = nativeBuffer.CountKeyword(L"\n");
		nativeBuffer.Clear();

		m_OutputList.Reserve(GetMax<unsigned int>(linefeedCount, sectorInfos.GetSize()));

		MkHashStr currentType, currentState;
		MkInt2 currentTextPos;
		int currentFontHeight = 0;

		MkMapLooper<unsigned int, SectorInfo> looper(sectorInfos);
		MK_STL_LOOP(looper)
		{
			const SectorInfo& currInfo = looper.GetCurrentField();
			bool typeChanged = (!currInfo.type.Empty()) && (currentType != currInfo.type);
			if (typeChanged)
			{
				currentType = currInfo.type;
				currentFontHeight = MK_FONT_MGR.GetFontHeight(currentType) + lineFeedSize;
			}
			bool stateChanged = (!currInfo.state.Empty()) && (currentState != currInfo.state);
			if (stateChanged)
			{
				currentState = currInfo.state;
			}
			
			unsigned int currPos = 0;
			while (currPos < currInfo.text.GetSize())
			{
				// currPos부터 개행문자나 문자열 끝까지의 라인 하나를 얻음
				MkStr currLineBuffer;
				unsigned int linefeedPos = currInfo.text.GetFirstLineFeedPosition(currPos);
				bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
				if (endOfMsg)
				{
					currInfo.text.GetSubStr(MkArraySection(currPos), currLineBuffer); // 문자열 끝까지의 라인(문자열 종료)
				}
				else if (linefeedPos > currPos)
				{
					currInfo.text.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // 개행문자까지의 라인
				}

				currLineBuffer.RemoveRearSideBlank();

				// 공문자로만 이루어진 라인이거나 연달아 개행문자가 올 경우 빈 문자열이 지정 될 수 있음
				if (!currLineBuffer.Empty())
				{
					SectorInfo& si = m_OutputList.PushBack();
					if (typeChanged)
					{
						si.type = currentType;
						typeChanged = false;
					}
					if (stateChanged)
					{
						si.state = currentState;
						stateChanged = false;
					}
					si.text = currLineBuffer;
					si.position = currentTextPos;

					m_DrawingSize.y = si.position.y + currentFontHeight;
				}

				if (!currLineBuffer.Empty())
				{
					currentTextPos.x += MK_FONT_MGR.GetTextSize(currentType, currLineBuffer).x;

					m_DrawingSize.x = GetMax<int>(m_DrawingSize.x, currentTextPos.x);
				}

				if (endOfMsg)
				{
					currPos = currInfo.text.GetSize(); // 다음 sector info
				}
				else
				{
					currPos = linefeedPos + 1;

					currentTextPos.x = 0;
					currentTextPos.y += currentFontHeight; // 다음 라인
				}
			}
		}
	}
}

MkDecoStr& MkDecoStr::operator = (const MkDecoStr& decoStr)
{
	m_OutputList = decoStr.__GetOutputList();
	m_DrawingSize = decoStr.GetDrawingSize();
	return *this;
}

bool MkDecoStr::ChangeState(const MkHashStr& state)
{
	bool ok = MK_FONT_MGR.CheckAvailableFontState(state);
	if (ok)
	{
		MK_INDEXING_LOOP(m_OutputList, i)
		{
			m_OutputList[i].state = state;
		}
	}
	return ok;
}

void MkDecoStr::Clear(void)
{
	m_OutputList.Clear();
	m_DrawingSize.Clear();
}

void MkDecoStr::_SetSector(MkMap<unsigned int, SectorInfo>& sectorInfos, unsigned int position, const MkHashStr& type, const MkHashStr& state)
{
	SectorInfo& currSector = sectorInfos.Exist(position) ? sectorInfos[position] : sectorInfos.Create(position);
	if (!type.Empty())
	{
		currSector.type = type;
	}
	if (!state.Empty())
	{
		currSector.state = state;
	}
}

bool MkDecoStr::Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer)
{
	bool ok = (MK_FONT_MGR.CheckAvailableFontType(fontType) && MK_FONT_MGR.CheckAvailableFontState(fontState));
	if (ok && (!msg.Empty()))
	{
		const static MkStr LineFeedSizeKeywordPrefix = gTagBegin + gLineFeedSizeKeyword + gTagSeperator; // L"<%LFS:"
		const static MkStr TypeKeywordPrefix = gTagBegin + gTypeKeyword + gTagSeperator; // L"<%T:"
		const static MkStr StateKeywordPrefix = gTagBegin + gStateKeyword + gTagSeperator; // L"<%S:"

		const static unsigned int TagSize =
			LineFeedSizeKeywordPrefix.GetSize() + TypeKeywordPrefix.GetSize() + StateKeywordPrefix.GetSize() + gTagEnd.GetSize() * 3 + ((lineFeedSize == 0) ? 0 : 5);

		buffer.Clear();
		buffer.Reserve(fontType.GetSize() + fontState.GetSize() + msg.GetSize() + TagSize);

		// line feed size tag
		if (lineFeedSize != 0)
		{
			buffer += LineFeedSizeKeywordPrefix;
			buffer += lineFeedSize;
			buffer += gTagEnd;
		}

		// font type tag
		buffer += TypeKeywordPrefix;
		buffer += fontType.GetString();
		buffer += gTagEnd;

		// font state tag
		buffer += StateKeywordPrefix;
		buffer += fontState.GetString();
		buffer += gTagEnd;

		// msg
		buffer += msg;
	}
	return ok;
}

bool MkDecoStr::Convert(const MkStr& msg, MkStr& buffer)
{
	return Convert(MK_FONT_MGR.DSF(), MK_FONT_MGR.DSF(), 0, msg, buffer);
}

//------------------------------------------------------------------------------------------------//
