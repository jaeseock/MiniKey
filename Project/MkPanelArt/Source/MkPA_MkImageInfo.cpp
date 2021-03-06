#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkFloatOp.h"
#include "MkPA_MkImageInfo.h"


const static MkHashStr GROUP_KEY = L"__#Group";

const static MkHashStr POSITION_KEY = L"Position";
const static MkHashStr SIZE_KEY = L"Size";
const static MkHashStr TABLE_KEY = L"Table";
const static MkHashStr OFFSET_KEY = L"Offset";

const static MkHashStr TOTAL_RANGE_KEY = L"TotalRange";
const static MkHashStr SUBSET_LIST_KEY = L"SubsetList";
const static MkHashStr TIME_LIST_KEY = L"TimeList";
const static MkHashStr PIVOT_KEY = L"Pivot";
const static MkHashStr LOOP_KEY = L"Loop";

//------------------------------------------------------------------------------------------------//

void MkImageInfo::SetUp(const MkInt2& imageSize, const MkDataNode* node)
{
	Clear();

	// ready to uv converting
	MkFloat2 fImageSize(static_cast<float>(imageSize.x), static_cast<float>(imageSize.y));

	// empty subset
	_CreateEmptySubset(fImageSize);

	if (node == NULL)
		return;

	// group
	node->GetDataEx(GROUP_KEY, m_Group, 0);

	// parse nodes
	MkArray<MkHashStr> keyList;
	node->GetChildNodeList(keyList);
	MK_INDEXING_LOOP(keyList, i)
	{
		const MkHashStr& childName = keyList[i];
		const MkDataNode& childNode = *node->GetChildNode(childName);

		do
		{
			// 필수 key로 subset/sequence 종류 판단
			if (childNode.IsValidKey(SIZE_KEY)) // subset
			{
				MkInt2 position;
				childNode.GetData(POSITION_KEY, position, 0);

				MkInt2 size;
				childNode.GetData(SIZE_KEY, size, 0);
				MK_CHECK(size.IsPositive(), childName.GetString() + L" 노드의 " + SIZE_KEY.GetString() + L" 값 오류")
					break;

				MkInt2 table(1, 1);
				childNode.GetData(TABLE_KEY, table, 0);
				MK_CHECK(table.IsPositive(), childName.GetString() + L" 노드의 " + TABLE_KEY.GetString() + L" 값 오류")
					break;

				MkFloat2 fSubsetSize(static_cast<float>(size.x), static_cast<float>(size.y));
				MkFloat2 fUVSize(fSubsetSize.x / fImageSize.x, fSubsetSize.y / fImageSize.y);

				// single subset
				if ((table.x == 1) && (table.y == 1))
				{
					_UpdateSubset(childName, fSubsetSize, position, fImageSize, fUVSize);
				}
				// multi subset
				else
				{
					MkInt2 offset = size;
					childNode.GetData(OFFSET_KEY, offset, 0);

					MkInt2 currPivot = position;
					for (int y = 0; y < table.y; ++y)
					{
						for (int x = 0; x < table.x; ++x)
						{
							_UpdateSubset(childName.GetString() + MkStr(y * table.x + x), fSubsetSize, currPivot, fImageSize, fUVSize);
							currPivot.x += offset.x;
						}
						currPivot.x = position.x;
						currPivot.y += offset.y;
					}
				}
			}
			else if (childNode.IsValidKey(TOTAL_RANGE_KEY) && childNode.IsValidKey(SUBSET_LIST_KEY)) // sequence
			{
				float totalRange = 0.f;
				childNode.GetData(TOTAL_RANGE_KEY, totalRange, 0);
				MK_CHECK(totalRange > 0.f, childName.GetString() + L" 노드의 " + TOTAL_RANGE_KEY.GetString() + L" 값 오류")
					break;

				MkArray<MkHashStr> subsetList;
				childNode.GetDataEx(SUBSET_LIST_KEY, subsetList);
				MK_CHECK(!subsetList.Empty(), childName.GetString() + L" 노드의 " + SUBSET_LIST_KEY.GetString() + L" 값 오류")
					break;

				MkArray<float> timeList;
				childNode.GetData(TIME_LIST_KEY, timeList);

				eRectAlignmentPosition pivot = eRAP_LeftBottom;
				MkStr pivotBuffer;
				if (childNode.GetData(PIVOT_KEY, pivotBuffer, 0) && (pivotBuffer.GetSize() == 2))
				{
					pivotBuffer.ToUpper();
					const wchar_t& frontChar = pivotBuffer.GetAt(0);
					const wchar_t& rearChar = pivotBuffer.GetAt(1);

					if (frontChar == L'L')
					{
						if (rearChar == L'T') { pivot = eRAP_LeftTop; }
						else if (rearChar == L'C') { pivot = eRAP_LeftCenter; }
						else if (rearChar == L'B') { pivot = eRAP_LeftBottom; }
					}
					else if (frontChar == L'M')
					{
						if (rearChar == L'T') { pivot = eRAP_MiddleTop; }
						else if (rearChar == L'C') { pivot = eRAP_MiddleCenter; }
						else if (rearChar == L'B') { pivot = eRAP_MiddleBottom; }
					}
					else if (frontChar == L'R')
					{
						if (rearChar == L'T') { pivot = eRAP_RightTop; }
						else if (rearChar == L'C') { pivot = eRAP_RightCenter; }
						else if (rearChar == L'B') { pivot = eRAP_RightBottom; }
					}
				}

				bool loop = true;
				childNode.GetData(LOOP_KEY, loop, 0);

				// time list 검증 및 자동생성
				if (timeList.Empty())
				{
					timeList.Reserve(subsetList.GetSize());
					float oneFrameTime = totalRange / static_cast<float>(subsetList.GetSize());
					float timeStamp = 0.f;
					MK_INDEXING_LOOP(subsetList, j)
					{
						timeList.PushBack(timeStamp);
						timeStamp += oneFrameTime;
					}
				}
				else
				{
					MK_CHECK(timeList.GetSize() == subsetList.GetSize(), childName.GetString() + L" 노드의 " + TIME_LIST_KEY.GetString() + L" 값 갯수가 subset list와 다름")
						break;

					int invalidFrame = 0;
					for (unsigned int cnt=timeList.GetSize()-1, j=0; j<cnt; ++j)
					{
						if (timeList[j] >= timeList[j+1])
						{
							invalidFrame = j+1;
							break;
						}
					}

					MK_CHECK(invalidFrame == 0, childName.GetString() + L" 노드의 " + TIME_LIST_KEY.GetString() + L" 값에 후퇴 프레임 존재 : " + MkStr(invalidFrame))
						break;

					timeList[0] = 0.f; // 최초 프레임은 0.부터 시작해야 함
				}

				// 등록
				Sequence& seq = m_Sequences.Create(childName);
				seq.totalRange = totalRange;
				seq.pivot = pivot;
				seq.loop = loop;

				MK_INDEXING_LOOP(timeList, j)
				{
					seq.tracks.Create(static_cast<double>(timeList[j]), subsetList[j]);
				}
			}
			else
			{
				MK_CHECK(false, childName.GetString() + L" 노드 구성을 위한 필수 key가 모두 갖추어져 있지 않음") {}
			}
		}
		while (false);
	}

	m_Subsets.Rehash();

	// 모든 노드들이 subset 혹은 sequence로 등록되었으므로 track 유효성 점검
	if (!m_Sequences.Empty())
	{
		MkArray<MkHashStr> killList(m_Sequences.GetSize());
		MkHashMapLooper<MkHashStr, Sequence> looper(m_Sequences);
		MK_STL_LOOP(looper)
		{
			MkMap<double, MkHashStr>& tracks = looper.GetCurrentField().tracks;
			MkMapLooper<double, MkHashStr> trackLooper(tracks);
			MK_STL_LOOP(trackLooper)
			{
				MK_CHECK(m_Subsets.Exist(trackLooper.GetCurrentField()), looper.GetCurrentKey().GetString() + L" sequence에 등록되어 있지 않은 " + trackLooper.GetCurrentField().GetString() + L" subset이 존재")
				{
					killList.PushBack(looper.GetCurrentKey());
					break;
				}
			}
		}

		m_Sequences.Erase(killList);
		m_Sequences.Rehash();
	}
}

bool MkImageInfo::SaveToFile(const MkPathName& filePath, bool textFormat) const
{
	MkDataNode node;

	// group
	if (!m_Group.Empty())
	{
		node.CreateUnit(GROUP_KEY, m_Group.GetString());
	}

	// subset
	{
		MkConstHashMapLooper<MkHashStr, Subset> looper(m_Subsets);
		MK_STL_LOOP(looper)
		{
			if (!looper.GetCurrentKey().Empty())
			{
				const Subset& ss = looper.GetCurrentField();

				MkDataNode* childNode = node.CreateChildNode(looper.GetCurrentKey());
				if (!ss.position.IsZero())
				{
					childNode->CreateUnit(POSITION_KEY, ss.position);
				}
				childNode->CreateUnit(SIZE_KEY, MkInt2(static_cast<int>(ss.rectSize.x), static_cast<int>(ss.rectSize.y)));
			}
		}
	}

	// sequence
	{
		MkConstHashMapLooper<MkHashStr, Sequence> looper(m_Sequences);
		MK_STL_LOOP(looper)
		{
			const Sequence& sq = looper.GetCurrentField();

			MkDataNode* childNode = node.CreateChildNode(looper.GetCurrentKey());

			childNode->CreateUnit(TOTAL_RANGE_KEY, static_cast<float>(sq.totalRange));
			switch (sq.pivot)
			{
			case eRAP_LeftTop: childNode->CreateUnit(PIVOT_KEY, L"LT"); break;
			case eRAP_LeftCenter: childNode->CreateUnit(PIVOT_KEY, L"LC"); break;
			case eRAP_MiddleTop: childNode->CreateUnit(PIVOT_KEY, L"MT"); break;
			case eRAP_MiddleCenter: childNode->CreateUnit(PIVOT_KEY, L"MC"); break;
			case eRAP_MiddleBottom: childNode->CreateUnit(PIVOT_KEY, L"MB"); break;
			case eRAP_RightTop: childNode->CreateUnit(PIVOT_KEY, L"RT"); break;
			case eRAP_RightCenter: childNode->CreateUnit(PIVOT_KEY, L"RC"); break;
			case eRAP_RightBottom: childNode->CreateUnit(PIVOT_KEY, L"RB"); break;
			}
			if (!sq.loop)
			{
				childNode->CreateUnit(LOOP_KEY, sq.loop);
			}

			MkArray<double> dTimeList;
			sq.tracks.GetKeyList(dTimeList);

			MkArray<float> timeList(dTimeList.GetSize());
			MK_INDEXING_LOOP(dTimeList, i)
			{
				timeList.PushBack(static_cast<float>(dTimeList[i]));
			}

			if (timeList.GetSize() > 1) // 두 개 이상이어야 의미가 있음
			{
				childNode->CreateUnit(TIME_LIST_KEY, timeList);
			}

			MkArray<MkHashStr> subsetList;
			sq.tracks.GetFieldList(subsetList);
			childNode->CreateUnitEx(SUBSET_LIST_KEY, subsetList);
		}
	}

	return (textFormat) ? node.SaveToText(filePath) : node.SaveToBinary(filePath);
}

void MkImageInfo::ResetDefaultSubsetInfo(const MkInt2& srcSize, const MkInt2& realSize)
{
	if (m_Subsets.Exist(MkHashStr::EMPTY))
	{
		Subset& emptySubset = m_Subsets[MkHashStr::EMPTY];

		emptySubset.rectSize = MkFloat2(static_cast<float>(srcSize.x), static_cast<float>(srcSize.y));

		float u = static_cast<float>(srcSize.x) / static_cast<float>(realSize.x);
		float v = static_cast<float>(srcSize.y) / static_cast<float>(realSize.y);
		emptySubset.uv[MkFloatRect::eLeftTop] = MkFloat2::Zero;
		emptySubset.uv[MkFloatRect::eRightTop] = MkFloat2(u, 0.f);
		emptySubset.uv[MkFloatRect::eLeftBottom] = MkFloat2(0.f, v);
		emptySubset.uv[MkFloatRect::eRightBottom] = MkFloat2(u, v);
	}
}

const MkImageInfo::Sequence* MkImageInfo::GetSequencePtr(const MkHashStr& sequenceName) const
{
	return m_Sequences.Exist(sequenceName) ? &m_Sequences[sequenceName] : NULL;
}

const MkHashStr& MkImageInfo::GetCurrentSubsetName(const MkHashStr& sequenceName, double elapsed) const
{
	if ((elapsed >= 0.) && m_Sequences.Exist(sequenceName))
	{
		const Sequence& seq = m_Sequences[sequenceName];
		if (seq.loop)
		{
			elapsed = MkFloatOp::GetRemainder(elapsed, seq.totalRange);
		}
		return seq.tracks.GetLowerBoundField(elapsed);
	}

	return MkHashStr::EMPTY;
}

const MkImageInfo::Subset* MkImageInfo::GetCurrentSubsetPtr(const MkHashStr& subsetOrSequenceName, double elapsed) const
{
	if (m_Subsets.Exist(subsetOrSequenceName))
		return &m_Subsets[subsetOrSequenceName];

	const MkHashStr& subsetName = GetCurrentSubsetName(subsetOrSequenceName, elapsed);
	return subsetName.Empty() ? NULL : &m_Subsets[subsetName];
}

eRectAlignmentPosition MkImageInfo::GetCurrentPivot(const MkHashStr& subsetOrSequenceName) const
{
	if (m_Subsets.Exist(subsetOrSequenceName))
		return eRAP_LeftBottom;

	return m_Sequences.Exist(subsetOrSequenceName) ? m_Sequences[subsetOrSequenceName].pivot : eRAP_LeftBottom;
}

void MkImageInfo::Clear(void)
{
	m_Group.Clear();
	m_Subsets.Clear();
	m_Sequences.Clear();
}
/*
MkImageInfo& MkImageInfo::operator = (const MkImageInfo& source)
{
	Clear();
	m_Group = source.GetGroup();
	source.__CopyTo(m_Subsets);
	source.__CopyTo(m_Sequences);
	return *this;
}
*/

void MkImageInfo::UpdateSubset(const MkHashStr& name, const MkIntRect& subsetRect, const MkInt2& imgSize)
{
	MkFloat2 fSubsetSize(static_cast<float>(subsetRect.size.x), static_cast<float>(subsetRect.size.y));
	MkFloat2 fImageSize(static_cast<float>(imgSize.x), static_cast<float>(imgSize.y));
	MkFloat2 fUVSize(fSubsetSize.x / fImageSize.x, fSubsetSize.y / fImageSize.y);

	_UpdateSubset(name, fSubsetSize, subsetRect.position, fImageSize, fUVSize);
}

void MkImageInfo::SetGroup(const MkHashStr& name)
{
	m_Group = name;
}

void MkImageInfo::UpdateSubset(const MkHashMap<MkHashStr, MkIntRect>& subsets, const MkInt2& imgSize)
{
	m_Subsets.Clear();
	_CreateEmptySubset(MkFloat2(static_cast<float>(imgSize.x), static_cast<float>(imgSize.y)));

	MkConstHashMapLooper<MkHashStr, MkIntRect> looper(subsets);
	MK_STL_LOOP(looper)
	{
		UpdateSubset(looper.GetCurrentKey(), looper.GetCurrentField(), imgSize);
	}
}

void MkImageInfo::ChangeSubset(const MkHashStr& lastName, const MkHashStr& currName)
{
	if (m_Subsets.Exist(lastName) && (!m_Subsets.Exist(currName)))
	{
		m_Subsets.Create(currName, m_Subsets[lastName]);
		m_Subsets.Erase(lastName);

		// sequence에 등록된 subset이면 갱신
		MkHashMapLooper<MkHashStr, Sequence> seqLooper(m_Sequences);
		MK_STL_LOOP(seqLooper)
		{
			MkMapLooper<double, MkHashStr> trackLooper(seqLooper.GetCurrentField().tracks);
			MK_STL_LOOP(trackLooper)
			{
				if (trackLooper.GetCurrentField() == lastName)
				{
					trackLooper.GetCurrentField() = currName;
				}
			}
		}
	}
}

void MkImageInfo::RemoveSubset(const MkHashStr& name)
{
	m_Subsets.Erase(name);
}

void MkImageInfo::UpdateSequence(const MkHashStr& name, const Sequence& sequence)
{
	Sequence& s = m_Sequences.Exist(name) ? m_Sequences[name] : m_Sequences.Create(name);
	s = sequence;
}

void MkImageInfo::UpdateSequence(const MkHashMap<MkHashStr, Sequence>& sequences)
{
	m_Sequences.Clear();

	MkConstHashMapLooper<MkHashStr, Sequence> looper(sequences);
	MK_STL_LOOP(looper)
	{
		UpdateSequence(looper.GetCurrentKey(), looper.GetCurrentField());
	}
}

void MkImageInfo::ChangeSequence(const MkHashStr& lastName, const MkHashStr& currName)
{
	if (m_Sequences.Exist(lastName) && (!m_Sequences.Exist(currName)))
	{
		m_Sequences.Create(currName, m_Sequences[lastName]);
		m_Sequences.Erase(lastName);
	}
}

void MkImageInfo::RemoveSequence(const MkHashStr& name)
{
	if (m_Sequences.Exist(name))
	{
		m_Sequences.Erase(name);
	}
}

void MkImageInfo::_CreateEmptySubset(const MkFloat2& imgSize)
{
	Subset& emptySubset = m_Subsets.Create(MkHashStr::EMPTY);
	emptySubset.rectSize = imgSize;
	emptySubset.uv[MkFloatRect::eLeftTop] = MkFloat2::Zero;
	emptySubset.uv[MkFloatRect::eRightTop] = MkFloat2(1.f, 0.f);
	emptySubset.uv[MkFloatRect::eLeftBottom] = MkFloat2(0.f, 1.f);
	emptySubset.uv[MkFloatRect::eRightBottom] = MkFloat2(1.f, 1.f);
}

void MkImageInfo::_UpdateSubset(const MkHashStr& name, const MkFloat2& subsetSize, const MkInt2& pivot, const MkFloat2& imgSize, const MkFloat2& uvSize)
{
	Subset& ss = m_Subsets.Exist(name) ? m_Subsets[name] : m_Subsets.Create(name);
	ss.position = pivot;
	ss.rectSize = subsetSize;

	const float offset = 0.00001f; // floating 계산의 오류 보정. 현실적으로 만단위 크기의 texture를 사용하지는 않을 것이므로 이 정도의 해상도로 결정
	MkFloat2 uvPos(static_cast<float>(pivot.x) / imgSize.x + offset, static_cast<float>(pivot.y) / imgSize.y + offset);

	ss.uv[MkFloatRect::eLeftTop] = uvPos;

	ss.uv[MkFloatRect::eRightTop].x = uvPos.x + uvSize.x;
	ss.uv[MkFloatRect::eRightTop].y = uvPos.y;

	ss.uv[MkFloatRect::eLeftBottom].x = uvPos.x;
	ss.uv[MkFloatRect::eLeftBottom].y = uvPos.y + uvSize.y;

	ss.uv[MkFloatRect::eRightBottom] = uvPos + uvSize;
}

//------------------------------------------------------------------------------------------------//