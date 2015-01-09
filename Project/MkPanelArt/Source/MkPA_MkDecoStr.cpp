
#include "MkCore_MkCheck.h"
#include "MkCore_MkStringTableForParser.h"

#include "MkCore_MkDevPanel.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkDecoStr.h"

const MkDecoStr MkDecoStr::Null(L"");

const static MkStr gTBegin = MKDEF_PA_DECOSTR_TAG_BEGIN;
const static MkStr gTEnd = MKDEF_PA_DECOSTR_TAG_END;
const static MkStr gKHeader = MKDEF_PA_DECOSTR_HEADER_KEYWORD;
const static MkStr gTSeperator = MKDEF_PA_DECOSTR_TAG_SEPERATOR;
const static MkStr gKFontType = MKDEF_PA_DECOSTR_TYPE_KEYWORD;
const static MkStr gKFontState = MKDEF_PA_DECOSTR_STATE_KEYWORD;
const static MkStr gKLineFeed = MKDEF_PA_DECOSTR_LINEFEED_KEYWORD;

const static MkStr gT_Prefix = gTBegin + gKFontType + gTSeperator; // L"<%T="
const static MkStr sS_Prefix = gTBegin + gKFontState + gTSeperator; // L"<%S="
const static MkStr gLF_Prefix = gTBegin + gKLineFeed + gTSeperator; // L"<%LF="


void MkDecoStr::SetUp(const MkStr& source)
{
	unsigned int currPos = _GetDecoStrBodyPosition(source);
	if (currPos == 0)
		return; // 헤더가 없음. 정상적인 deco str format이 아님

	Clear();
	m_Source = source;

	// gTBegin으로 대략적인 태그 수를 파악
	unsigned int tmpTagCnt = source.CountKeyword(MkArraySection(currPos), gTBegin);
	if (tmpTagCnt > 0)
	{
		m_EventQueue.Reserve(tmpTagCnt);

		// 원본 복사
		MkStr srcCopy = source;

		// font type, state가 문자열 태그("~")를 사용하므로 MkStringTableForParser 적용
		MkStringTableForParser strTable;
		strTable.BuildStringTable(srcCopy);

		// 원본의 태그 값(ex> "제목 표시 FT")은 MkStringTableForParser에서 unique keyword로 변함
		// 따라서 source와 srcCopy의 크기가 달라지므로 srcCopy에서의 위치를 source에 맞게 적용하기 위해서는 크기 차이에 대한 offset 누적이 필요
		int posOffsetInSource = 0;

		// 태그를 파싱해 이벤트를 생성
		while (true)
		{
			MkStr tagBuffer;
			unsigned int nextPos = srcCopy.GetFirstBlock(currPos, gTBegin, gTEnd, tagBuffer);

			// 태그 처리
			if (nextPos != MKDEF_ARRAY_ERROR)
			{
				tagBuffer.RemoveBlank();

				bool ok = false;
				do
				{
					// 구분자로 키워드와 값에 해당하는 문자열을 구함
					MkStr keywordBuffer;
					unsigned int valuePos = tagBuffer.GetFirstBlock(0, gTSeperator, keywordBuffer);
					MK_CHECK(valuePos != MKDEF_ARRAY_ERROR, L"구분자 " + gTSeperator + L" 가 없는 태그 존재")
						break;
					MK_CHECK(!keywordBuffer.Empty(), L"키워드가 없는 태그 존재")
						break;

					MkStr valueBuffer;
					tagBuffer.GetSubStr(MkArraySection(valuePos), valueBuffer);
					MK_CHECK(!valueBuffer.Empty(), L"값이 없는 태그 존재")
						break;
					
					// 키워드에 따른 이벤트 등록
					_SetEvent evt;
					if (keywordBuffer == gKFontState)
					{
						evt.eventType = eSetFontState;
						MkHashStr valKey = valueBuffer;
						MK_CHECK(strTable.Exist(valKey), gKFontState + L" 키워드의 값인 " + valueBuffer + L" 는 문자열로 인정되지 않음")
							break;
						
						evt.arg0 = strTable.GetString(valKey);
						evt.arg1 = 0;
						posOffsetInSource += static_cast<int>(evt.arg0.GetSize()) - static_cast<int>(valueBuffer.GetSize()) + 2; // original - table key + sizeof("")
					}
					else if (keywordBuffer == gKFontType)
					{
						evt.eventType = eSetFontType;
						MkHashStr valKey = valueBuffer;
						MK_CHECK(strTable.Exist(valKey), gKFontType + L" 키워드의 값인 " + valueBuffer + L" 는 문자열로 인정되지 않음")
							break;
						
						evt.arg0 = strTable.GetString(valKey);
						evt.arg1 = 0;
						posOffsetInSource += static_cast<int>(evt.arg0.GetSize()) - static_cast<int>(valueBuffer.GetSize()) + 2; // original - table key + sizeof("")
					}
					else if (keywordBuffer == gKLineFeed)
					{
						evt.eventType = eSetLineFeedOffset;
						evt.arg1 = valueBuffer.ToInteger();
					}
					else
					{
						MK_CHECK(false, keywordBuffer + L" 키워드는 인정되지 않음")
							break;
					}

					evt.arg2 = static_cast<unsigned int>(static_cast<int>(nextPos) + posOffsetInSource);

					MkStr ttt;
					m_Source.GetSubStr(MkArraySection(evt.arg2), ttt);


					_RegisterEffectiveEvent(evt); // 유효성을 체크해 등록

					ok = true;
				}
				while (false);

				if (!ok) // 실패 처리
				{
					Clear();
					return;
				}

				currPos = nextPos; // 다음 탐색 진행 위치
			}
			else
				break; // 문자열 탐색 완료
		}
	}

	MK_DEV_PANEL.MsgToLog(L"### 모든 이벤트 : " + MkStr(m_EventQueue.GetSize()));
	if (m_EventQueue.GetSize() == 1)
	{
		const _SetEvent& evt = m_EventQueue[0];

		MkStr msg;
		msg.Reserve(1024);
		msg += L"  > ";
		switch (evt.eventType) // 값 변경
		{
		case eSetFontType:
			msg += gKFontType;
			msg += gTSeperator;
			msg += evt.arg0.GetString();
		case eSetFontState:
			msg += gKFontState;
			msg += gTSeperator;
			msg += evt.arg0.GetString();
			break;
		case eSetLineFeedOffset:
			msg += gKLineFeed;
			msg += gTSeperator;
			msg += evt.arg1;
			break;
		}
		MK_DEV_PANEL.MsgToLog(msg);
	}
	else
	{
		unsigned int logPos = 0;
		for (unsigned int _looping_counter = m_EventQueue.GetSize()-1, i = 0; i < _looping_counter; ++i)
		{
			const _SetEvent& cevt = m_EventQueue[i];
			const _SetEvent& nevt = m_EventQueue[i+1];

			if (logPos < cevt.arg2)
			{
				MkStr subStr;
				m_Source.GetSubStr(MkArraySection(cevt.arg2, nevt.arg2 - cevt.arg2), subStr);
				MK_DEV_PANEL.MsgToLog(subStr);
				logPos = cevt.arg2;
			}

			MkStr msg;
			msg.Reserve(1024);
			msg += L"  (" + MkStr(i) + L") ";
			switch (cevt.eventType) // 값 변경
			{
			case eSetFontType:
				msg += gKFontType;
				msg += gTSeperator;
				msg += cevt.arg0.GetString();
			case eSetFontState:
				msg += gKFontState;
				msg += gTSeperator;
				msg += cevt.arg0.GetString();
				break;
			case eSetLineFeedOffset:
				msg += gKLineFeed;
				msg += gTSeperator;
				msg += cevt.arg1;
				break;
			}
			MK_DEV_PANEL.MsgToLog(msg);
		}

		const _SetEvent& evt = m_EventQueue[m_EventQueue.GetSize()-1];
		if (logPos < evt.arg2)
		{
			MkStr subStr;
			m_Source.GetSubStr(MkArraySection(evt.arg2), subStr);
			MK_DEV_PANEL.MsgToLog(subStr);
		}

		MkStr msg;
		msg.Reserve(1024);
		msg += L"  (" + MkStr(m_EventQueue.GetSize()-1) + L") ";
		switch (evt.eventType) // 값 변경
		{
		case eSetFontType:
			msg += gKFontType;
			msg += gTSeperator;
			msg += evt.arg0.GetString();
		case eSetFontState:
			msg += gKFontState;
			msg += gTSeperator;
			msg += evt.arg0.GetString();
			break;
		case eSetLineFeedOffset:
			msg += gKLineFeed;
			msg += gTSeperator;
			msg += evt.arg1;
			break;
		}
		MK_DEV_PANEL.MsgToLog(msg);
	}
	

	//MkEventQueuePattern<_SetEvent> m_EventQueue;
	/*
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
		unsigned int tagEndPos = source.GetFirstBlock(position, gTBegin, gTEnd, tagBuffer);

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
			unsigned int strSize = tagEndPos - gTBegin.GetSize() - tagBuffer.GetSize() - gTEnd.GetSize() - position;
			source.GetSubStr(MkArraySection(position, strSize), strBuffer);
			nativeBuffer += strBuffer;
			position = tagEndPos;
			unsigned int tagPos = nativeBuffer.GetSize();

			// parse tag
			MkArray<MkStr> tokens;
			if (tagBuffer.Tokenize(tokens, gTSeperator) == 2)
			{
				const MkStr& tagKey = tokens[0];
				const MkStr& tagValue = tokens[1];

				// font type
				if ((tagKey == gKFontType) && (tagValue != lastFontType) && (MK_FONT_MGR.CheckAvailableFontType(tagValue)))
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
				else if ((tagKey == gKFontState) && (tagValue != lastFontState) && (MK_FONT_MGR.CheckAvailableFontState(tagValue)))
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
			_SetSector(sectorInfos, 0, MK_FONT_MGR.DSF(), MK_FONT_MGR.WhiteFS()); // DSF
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
					firstSector.state = MK_FONT_MGR.WhiteFS();
				}
			}
			else
			{
				_SetSector(sectorInfos, 0, MK_FONT_MGR.DSF(), MK_FONT_MGR.WhiteFS()); // DSF
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
	*/
}

MkDecoStr& MkDecoStr::operator = (const MkDecoStr& decoStr)
{
	//m_OutputList = decoStr.__GetOutputList();
	//m_DrawingSize = decoStr.GetDrawingSize();
	return *this;
}

void MkDecoStr::Clear(void)
{
	m_Source.Clear();
	m_EventQueue.Clear();
	//m_OutputList.Clear();
	//m_DrawingSize.Clear();
}

unsigned int MkDecoStr::_GetDecoStrBodyPosition(const MkStr& source) const
{
	if (source.Empty())
		return 0; // 문자열이 비었음. error는 아님

	unsigned int pos = source.GetFirstValidPosition();
	if (pos == MKDEF_ARRAY_ERROR)
		return 0; // 공문자만 존재하는 문자열. error는 아님

	MkStr buffer;
	pos = source.GetFirstBlock(pos, gTBegin, gTEnd, buffer);
	MK_CHECK(pos != MKDEF_ARRAY_ERROR, L"헤더 태그가 없는 문자열로 MkDecoStr 초기화 시도")
		return 0;

	buffer.RemoveBlank();
	MK_CHECK(buffer == gKHeader, L"헤더 태그가 없는 문자열로 MkDecoStr 초기화 시도")
		return 0;

	return pos;
}

void MkDecoStr::_RegisterEffectiveEvent(const _SetEvent& evt)
{
	// 유효성을 점검
	MK_INDEXING_RLOOP(m_EventQueue, i)
	{
		_SetEvent& lastEvt = m_EventQueue[i];
		if (evt.arg2 == lastEvt.arg2) // 같은 위치에
		{
			if (evt.eventType == lastEvt.eventType) // 동일 이벤트 타입이 있으면
			{
				switch (evt.eventType) // 값 변경
				{
				case eSetFontType:
				case eSetFontState:
					lastEvt.arg0 = evt.arg0;
					break;
				case eSetLineFeedOffset:
					lastEvt.arg1 = evt.arg1;
					break;
				}
				return;
			}
			else
				break;
		}
		else
			break;
	}

	m_EventQueue.PushBack(evt); // 유효한 이벤트
}

/*
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

bool MkDecoStr::InsertFontTypeTag(const MkHashStr& fontType, const MkStr& msg, MkStr& buffer)
{
	bool ok = (MK_FONT_MGR.CheckAvailableFontType(fontType) && (!msg.Empty()));
	if (ok)
	{
		buffer.Clear();
		buffer.Reserve(gKFontTypePrefix.GetSize() + fontType.GetSize() + gTEnd.GetSize() + msg.GetSize());

		buffer += gKFontTypePrefix;
		buffer += fontType.GetString();
		buffer += gTEnd;
		buffer += msg;
	}
	return ok;
}

bool MkDecoStr::InsertFontStateTag(const MkHashStr& fontState, const MkStr& msg, MkStr& buffer)
{
	bool ok = (MK_FONT_MGR.CheckAvailableFontState(fontState) && (!msg.Empty()));
	if (ok)
	{
		buffer.Clear();
		buffer.Reserve(sStateKeywordPrefix.GetSize() + fontState.GetSize() + gTEnd.GetSize() + msg.GetSize());

		buffer += sStateKeywordPrefix;
		buffer += fontState.GetString();
		buffer += gTEnd;
		buffer += msg;
	}
	return ok;
}

bool MkDecoStr::Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer)
{
	bool ok = (MK_FONT_MGR.CheckAvailableFontType(fontType) && MK_FONT_MGR.CheckAvailableFontState(fontState) && (!msg.Empty()));
	if (ok)
	{
		unsigned int tagSize = gKFontTypePrefix.GetSize() + sStateKeywordPrefix.GetSize() + gTEnd.GetSize() * 2;
		if (lineFeedSize != 0)
		{
			tagSize += gLineFeedSizeKeywordPrefix.GetSize() + 5 + gTEnd.GetSize();
		}

		buffer.Clear();
		buffer.Reserve(fontType.GetSize() + fontState.GetSize() + msg.GetSize() + tagSize);

		// line feed size tag
		if (lineFeedSize != 0)
		{
			buffer += gLineFeedSizeKeywordPrefix;
			buffer += lineFeedSize;
			buffer += gTEnd;
		}

		// font type tag
		buffer += gKFontTypePrefix;
		buffer += fontType.GetString();
		buffer += gTEnd;

		// font state tag
		buffer += sStateKeywordPrefix;
		buffer += fontState.GetString();
		buffer += gTEnd;

		// msg
		buffer += msg;
	}
	return ok;
}

bool MkDecoStr::Convert
(const MkHashStr& fontType, const MkArray<MkHashStr>& fontState, const MkArray<unsigned int>& statePos, int lineFeedSize, const MkStr& msg, MkStr& buffer)
{
	unsigned int allStateTagSize = 0;
	bool ok = (MK_FONT_MGR.CheckAvailableFontType(fontType) && (!msg.Empty()) && (!fontState.Empty()) && (!statePos.Empty()) && (fontState.GetSize() == statePos.GetSize()));
	if (ok)
	{
		MK_INDEXING_LOOP(fontState, i)
		{
			if ((!MK_FONT_MGR.CheckAvailableFontState(fontState[i])) || (statePos[i] > msg.GetSize()))
			{
				ok = false;
				break;
			}
			else
			{
				allStateTagSize += fontState[i].GetSize();
			}
		}
	}
	if (ok)
	{
		// header set
		MkStr headerSet;
		unsigned int headerSetSize = gKFontTypePrefix.GetSize() + fontType.GetSize() + gTEnd.GetSize();
		if (lineFeedSize != 0)
		{
			headerSetSize += gLineFeedSizeKeywordPrefix.GetSize() + 5 + gTEnd.GetSize();
		}
		headerSet.Reserve(headerSetSize);

		// line feed size tag
		if (lineFeedSize != 0)
		{
			headerSet += gLineFeedSizeKeywordPrefix;
			headerSet += lineFeedSize;
			headerSet += gTEnd;
		}

		// font type tag
		headerSet += gKFontTypePrefix;
		headerSet += fontType.GetString();
		headerSet += gTEnd;

		// body set
		MkStr bodySet;
		bodySet.Reserve((sStateKeywordPrefix.GetSize() + gTEnd.GetSize()) * fontState.GetSize() + allStateTagSize + msg.GetSize());

		// currPos 구간별로 끊음
		MkArray<MkStr> subset;
		subset.Fill(fontState.GetSize() + 1);
		unsigned int currPos = 0;
		unsigned int prevBlankSize = 0;
		MK_INDEXING_LOOP(fontState, i)
		{
			msg.GetSubStr(MkArraySection(currPos, statePos[i] - currPos), subset[i]);

			// 이전 문자열 후반 공문자를 현재 문자열 앞에 삽입함
			if (prevBlankSize > 0)
			{
				MkStr blank;
				blank.AddRepetition(L" ", prevBlankSize);
				subset[i].Insert(0, blank);
			}
			prevBlankSize = subset[i].RemoveRearSideBlank(); // 다음 문자열에 삽입할 공문자 삭제

			currPos = statePos[i];
		}
		msg.GetSubStr(MkArraySection(currPos), subset[fontState.GetSize()]);
		if (prevBlankSize > 0)
		{
			MkStr blank;
			blank.AddRepetition(L" ", prevBlankSize);
			subset[fontState.GetSize()].Insert(0, blank);
		}

		MK_INDEXING_LOOP(subset, i)
		{
			if (i == 0)
			{
				bodySet += subset[i];
			}
			else
			{
				MkStr tokenStr;
				MkDecoStr::InsertFontStateTag(fontState[i-1], subset[i], tokenStr);
				bodySet += tokenStr;
			}
		}

		// result
		buffer.Clear();
		buffer.Reserve(headerSet.GetSize() + bodySet.GetSize());
		buffer += headerSet;
		buffer += bodySet;
	}
	return ok;
}

bool MkDecoStr::Convert(const MkStr& msg, MkStr& buffer)
{
	return Convert(MK_FONT_MGR.DSF(), MK_FONT_MGR.WhiteFS(), 0, msg, buffer);
}
*/
//------------------------------------------------------------------------------------------------//
