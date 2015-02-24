#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkFloatOp.h"
#include "MkPA_MkImageInfo.h"


const static MkHashStr GROUP_KEY = L"Group";

const static MkHashStr POSITION_KEY = L"Position";
const static MkHashStr SIZE_KEY = L"Size";
const static MkHashStr TABLE_KEY = L"Table";
const static MkHashStr OFFSET_KEY = L"Offset";

const static MkHashStr TOTAL_RANGE_KEY = L"TotalRange";
const static MkHashStr SUBSET_LIST_KEY = L"SubsetList";
const static MkHashStr TIME_LIST_KEY = L"TimeList";
const static MkHashStr LOOP_KEY = L"Loop";

//------------------------------------------------------------------------------------------------//

void MkImageInfo::SetUp(const MkInt2& imageSize, const MkDataNode* node)
{
	Clear();

	// ready to uv converting
	MkFloat2 fImageSize(static_cast<float>(imageSize.x), static_cast<float>(imageSize.y));

	// empty subset
	Subset& emptySubset = m_Subsets.Create(MkHashStr::EMPTY);
	emptySubset.rectSize = fImageSize;
	emptySubset.uv[MkFloatRect::eLeftTop] = MkFloat2::Zero;
	emptySubset.uv[MkFloatRect::eRightTop] = MkFloat2(1.f, 0.f);
	emptySubset.uv[MkFloatRect::eLeftBottom] = MkFloat2(0.f, 1.f);
	emptySubset.uv[MkFloatRect::eRightBottom] = MkFloat2(1.f, 1.f);

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
					_RegisterSubset(childName, fSubsetSize, position, fImageSize, fUVSize);
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
							_RegisterSubset(childName.GetString() + MkStr(y * table.x + x), fSubsetSize, currPivot, fImageSize, fUVSize);
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

					MK_CHECK(invalidFrame > 0, childName.GetString() + L" 노드의 " + TIME_LIST_KEY.GetString() + L" 값에 후퇴 프레임 존재 : " + MkStr(invalidFrame))
						break;
				}

				// 등록
				Sequence& seq = m_Sequences.Create(childName);
				seq.totalRange = totalRange;
				seq.loop = loop;

				MK_INDEXING_LOOP(timeList, j)
				{
					seq.tracks.Create(timeList[j], subsetList[j]);
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
void MkImageInfo::_RegisterSubset(const MkHashStr& name, const MkFloat2& subsetSize, const MkInt2& pivot, const MkFloat2& imgSize, const MkFloat2& uvSize)
{
	Subset& ss = m_Subsets.Create(name);
	ss.rectSize = subsetSize;

	MkFloat2 uvPos(static_cast<float>(pivot.x) / imgSize.x, static_cast<float>(pivot.y) / imgSize.y);

	ss.uv[MkFloatRect::eLeftTop] = uvPos; //+0.00001f

	ss.uv[MkFloatRect::eRightTop].x = uvPos.x + uvSize.x;
	ss.uv[MkFloatRect::eRightTop].y = uvPos.y;

	ss.uv[MkFloatRect::eLeftBottom].x = uvPos.x;
	ss.uv[MkFloatRect::eLeftBottom].y = uvPos.y + uvSize.y;

	ss.uv[MkFloatRect::eRightBottom] = uvPos + uvSize;
}

//------------------------------------------------------------------------------------------------//