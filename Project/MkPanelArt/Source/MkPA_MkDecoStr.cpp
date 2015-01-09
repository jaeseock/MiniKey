
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
		return; // ����� ����. �������� deco str format�� �ƴ�

	Clear();
	m_Source = source;

	// gTBegin���� �뷫���� �±� ���� �ľ�
	unsigned int tmpTagCnt = source.CountKeyword(MkArraySection(currPos), gTBegin);
	if (tmpTagCnt > 0)
	{
		m_EventQueue.Reserve(tmpTagCnt);

		// ���� ����
		MkStr srcCopy = source;

		// font type, state�� ���ڿ� �±�("~")�� ����ϹǷ� MkStringTableForParser ����
		MkStringTableForParser strTable;
		strTable.BuildStringTable(srcCopy);

		// ������ �±� ��(ex> "���� ǥ�� FT")�� MkStringTableForParser���� unique keyword�� ����
		// ���� source�� srcCopy�� ũ�Ⱑ �޶����Ƿ� srcCopy������ ��ġ�� source�� �°� �����ϱ� ���ؼ��� ũ�� ���̿� ���� offset ������ �ʿ�
		int posOffsetInSource = 0;

		// �±׸� �Ľ��� �̺�Ʈ�� ����
		while (true)
		{
			MkStr tagBuffer;
			unsigned int nextPos = srcCopy.GetFirstBlock(currPos, gTBegin, gTEnd, tagBuffer);

			// �±� ó��
			if (nextPos != MKDEF_ARRAY_ERROR)
			{
				tagBuffer.RemoveBlank();

				bool ok = false;
				do
				{
					// �����ڷ� Ű����� ���� �ش��ϴ� ���ڿ��� ����
					MkStr keywordBuffer;
					unsigned int valuePos = tagBuffer.GetFirstBlock(0, gTSeperator, keywordBuffer);
					MK_CHECK(valuePos != MKDEF_ARRAY_ERROR, L"������ " + gTSeperator + L" �� ���� �±� ����")
						break;
					MK_CHECK(!keywordBuffer.Empty(), L"Ű���尡 ���� �±� ����")
						break;

					MkStr valueBuffer;
					tagBuffer.GetSubStr(MkArraySection(valuePos), valueBuffer);
					MK_CHECK(!valueBuffer.Empty(), L"���� ���� �±� ����")
						break;
					
					// Ű���忡 ���� �̺�Ʈ ���
					_SetEvent evt;
					if (keywordBuffer == gKFontState)
					{
						evt.eventType = eSetFontState;
						MkHashStr valKey = valueBuffer;
						MK_CHECK(strTable.Exist(valKey), gKFontState + L" Ű������ ���� " + valueBuffer + L" �� ���ڿ��� �������� ����")
							break;
						
						evt.arg0 = strTable.GetString(valKey);
						evt.arg1 = 0;
						posOffsetInSource += static_cast<int>(evt.arg0.GetSize()) - static_cast<int>(valueBuffer.GetSize()) + 2; // original - table key + sizeof("")
					}
					else if (keywordBuffer == gKFontType)
					{
						evt.eventType = eSetFontType;
						MkHashStr valKey = valueBuffer;
						MK_CHECK(strTable.Exist(valKey), gKFontType + L" Ű������ ���� " + valueBuffer + L" �� ���ڿ��� �������� ����")
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
						MK_CHECK(false, keywordBuffer + L" Ű����� �������� ����")
							break;
					}

					evt.arg2 = static_cast<unsigned int>(static_cast<int>(nextPos) + posOffsetInSource);

					MkStr ttt;
					m_Source.GetSubStr(MkArraySection(evt.arg2), ttt);


					_RegisterEffectiveEvent(evt); // ��ȿ���� üũ�� ���

					ok = true;
				}
				while (false);

				if (!ok) // ���� ó��
				{
					Clear();
					return;
				}

				currPos = nextPos; // ���� Ž�� ���� ��ġ
			}
			else
				break; // ���ڿ� Ž�� �Ϸ�
		}
	}

	MK_DEV_PANEL.MsgToLog(L"### ��� �̺�Ʈ : " + MkStr(m_EventQueue.GetSize()));
	if (m_EventQueue.GetSize() == 1)
	{
		const _SetEvent& evt = m_EventQueue[0];

		MkStr msg;
		msg.Reserve(1024);
		msg += L"  > ";
		switch (evt.eventType) // �� ����
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
			switch (cevt.eventType) // �� ����
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
		switch (evt.eventType) // �� ����
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

	// ����(source)���� tag�� ���ڿ��� �и��� ����
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
						if (nativeBuffer[tagPos - 1] != L'\n') // �ٷ� ���� ���ڰ� line feed�� �ƴϸ� ���� ����
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
		// tag�� ��� ���ڿ� ����
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

		// line feed �������� ���ڿ� �и� �� ��ǥ ����
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
				// currPos���� ���๮�ڳ� ���ڿ� �������� ���� �ϳ��� ����
				MkStr currLineBuffer;
				unsigned int linefeedPos = currInfo.text.GetFirstLineFeedPosition(currPos);
				bool endOfMsg = (linefeedPos == MKDEF_ARRAY_ERROR);
				if (endOfMsg)
				{
					currInfo.text.GetSubStr(MkArraySection(currPos), currLineBuffer); // ���ڿ� �������� ����(���ڿ� ����)
				}
				else if (linefeedPos > currPos)
				{
					currInfo.text.GetSubStr(MkArraySection(currPos, linefeedPos - currPos), currLineBuffer); // ���๮�ڱ����� ����
				}

				currLineBuffer.RemoveRearSideBlank();

				// �����ڷθ� �̷���� �����̰ų� ���޾� ���๮�ڰ� �� ��� �� ���ڿ��� ���� �� �� ����
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
					currPos = currInfo.text.GetSize(); // ���� sector info
				}
				else
				{
					currPos = linefeedPos + 1;

					currentTextPos.x = 0;
					currentTextPos.y += currentFontHeight; // ���� ����
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
		return 0; // ���ڿ��� �����. error�� �ƴ�

	unsigned int pos = source.GetFirstValidPosition();
	if (pos == MKDEF_ARRAY_ERROR)
		return 0; // �����ڸ� �����ϴ� ���ڿ�. error�� �ƴ�

	MkStr buffer;
	pos = source.GetFirstBlock(pos, gTBegin, gTEnd, buffer);
	MK_CHECK(pos != MKDEF_ARRAY_ERROR, L"��� �±װ� ���� ���ڿ��� MkDecoStr �ʱ�ȭ �õ�")
		return 0;

	buffer.RemoveBlank();
	MK_CHECK(buffer == gKHeader, L"��� �±װ� ���� ���ڿ��� MkDecoStr �ʱ�ȭ �õ�")
		return 0;

	return pos;
}

void MkDecoStr::_RegisterEffectiveEvent(const _SetEvent& evt)
{
	// ��ȿ���� ����
	MK_INDEXING_RLOOP(m_EventQueue, i)
	{
		_SetEvent& lastEvt = m_EventQueue[i];
		if (evt.arg2 == lastEvt.arg2) // ���� ��ġ��
		{
			if (evt.eventType == lastEvt.eventType) // ���� �̺�Ʈ Ÿ���� ������
			{
				switch (evt.eventType) // �� ����
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

	m_EventQueue.PushBack(evt); // ��ȿ�� �̺�Ʈ
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

		// currPos �������� ����
		MkArray<MkStr> subset;
		subset.Fill(fontState.GetSize() + 1);
		unsigned int currPos = 0;
		unsigned int prevBlankSize = 0;
		MK_INDEXING_LOOP(fontState, i)
		{
			msg.GetSubStr(MkArraySection(currPos, statePos[i] - currPos), subset[i]);

			// ���� ���ڿ� �Ĺ� �����ڸ� ���� ���ڿ� �տ� ������
			if (prevBlankSize > 0)
			{
				MkStr blank;
				blank.AddRepetition(L" ", prevBlankSize);
				subset[i].Insert(0, blank);
			}
			prevBlankSize = subset[i].RemoveRearSideBlank(); // ���� ���ڿ��� ������ ������ ����

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
