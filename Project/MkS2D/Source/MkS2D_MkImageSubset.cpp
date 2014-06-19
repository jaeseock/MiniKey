
#include "MkS2D_MkProjectDefinition.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkImageSubset.h"


//------------------------------------------------------------------------------------------------//

void MkImageSubset::SetUp(const MkUInt2& imageSize, const MkDataNode* node)
{
	// default subset
	m_DefaultSubset.originalSize = MkFloat2(static_cast<float>(imageSize.x), static_cast<float>(imageSize.y));
	m_DefaultSubset.uv[MkFloatRect::eLeftTop] = MkFloat2(0.f, 0.f);
	m_DefaultSubset.uv[MkFloatRect::eRightTop] = MkFloat2(1.f, 0.f);
	m_DefaultSubset.uv[MkFloatRect::eLeftBottom] = MkFloat2(0.f, 1.f);
	m_DefaultSubset.uv[MkFloatRect::eRightBottom] = MkFloat2(1.f, 1.f);

	// image subset node
	if ((node != NULL) && (imageSize.x > 0) && (imageSize.y > 0) && (node->GetChildNodeCount() > 0))
	{
		MkArray<MkHashStr> keyList;
		node->GetChildNodeList(keyList);

		// key
		const static MkHashStr POSITION_KEY = MKDEF_S2D_IMAGE_SUBSET_FILE_POSITION_KEY;
		const static MkHashStr SIZE_KEY = MKDEF_S2D_IMAGE_SUBSET_FILE_SIZE_KEY;
		const static MkHashStr QUILT_KEY = MKDEF_S2D_IMAGE_SUBSET_FILE_QUILT_KEY;
		const static MkHashStr TABLE_KEY = MKDEF_S2D_IMAGE_SUBSET_FILE_TABLE_KEY;

		MkFloat2 fimg = MkFloat2(static_cast<float>(imageSize.x), static_cast<float>(imageSize.y));

		MK_INDEXING_LOOP(keyList, i)
		{
			const MkHashStr& subsetName = keyList[i];
			const MkDataNode* subsetNode = node->GetChildNode(subsetName);

			// pos, size는 left-top 기준
			MkInt2 pos, size;
			if (subsetNode->GetData(POSITION_KEY, pos, 0) && subsetNode->GetData(SIZE_KEY, size, 0))
			{
				MkFloat2 fsize = MkFloat2(static_cast<float>(size.x), static_cast<float>(size.y));
				MkFloat2 fuv = MkFloat2(fsize.x / fimg.x, fsize.y / fimg.y);
				
				// 패턴 리스트면 이미지 크기 안에서 주어진 갯수만큼 생성
				if (subsetNode->IsValidKey(QUILT_KEY))
				{
					int quilt;
					if (subsetNode->GetData(QUILT_KEY, quilt, 0) && (quilt > 0))
					{
						unsigned int count = static_cast<unsigned int>(quilt);
						MkArray<MkFloat2> fposList(count);

						MkInt2 maxSize(static_cast<int>(imageSize.x), static_cast<int>(imageSize.y));
						MkInt2 currPos = pos;
						while ((currPos.y < maxSize.y) && (fposList.GetSize() < count))
						{
							while ((currPos.x < maxSize.x) && (fposList.GetSize() < count))
							{
								fposList.PushBack(MkFloat2(static_cast<float>(currPos.x) / fimg.x, static_cast<float>(currPos.y) / fimg.y));
								currPos.x += size.x;
							}

							currPos.x = pos.x;
							currPos.y += size.y;
						}

						MK_INDEXING_LOOP(fposList, j)
						{
							_RegisterSubset(subsetName.GetString() + MkStr(j), fposList[j], fsize, fuv);
						}
					}
				}
				// 테이블 패턴이면 주어진 갯수만큼 생성
				else if (subsetNode->IsValidKey(TABLE_KEY))
				{
					MkInt2 table;
					if (subsetNode->GetData(TABLE_KEY, table, 0) && (table.x > 0) && (table.y > 0))
					{
						if (table.x == 1)
						{
							if (table.y == 3) // 1 * 3
							{
								float fx = static_cast<float>(pos.x) / fimg.x;
								_RegisterSubset(subsetName.GetString() + L"T", MkFloat2(fx, static_cast<float>(pos.y) / fimg.y), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"C", MkFloat2(fx, static_cast<float>(pos.y + size.y) / fimg.y), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"B", MkFloat2(fx, static_cast<float>(pos.y + size.y * 2) / fimg.y), fsize, fuv);
							}
						}
						else if (table.x == 3)
						{
							if (table.y == 1) // 3 * 1
							{
								float fy = static_cast<float>(pos.y) / fimg.y;
								_RegisterSubset(subsetName.GetString() + L"L", MkFloat2(static_cast<float>(pos.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"M", MkFloat2(static_cast<float>(pos.x + size.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"R", MkFloat2(static_cast<float>(pos.x + size.x * 2) / fimg.x, fy), fsize, fuv);
							}
							else if (table.y == 3) // 3 * 3
							{
								float fy = static_cast<float>(pos.y) / fimg.y;
								_RegisterSubset(subsetName.GetString() + L"LT", MkFloat2(static_cast<float>(pos.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"MT", MkFloat2(static_cast<float>(pos.x + size.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"RT", MkFloat2(static_cast<float>(pos.x + size.x * 2) / fimg.x, fy), fsize, fuv);

								fy = static_cast<float>(pos.y + size.y) / fimg.y;
								_RegisterSubset(subsetName.GetString() + L"LC", MkFloat2(static_cast<float>(pos.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"MC", MkFloat2(static_cast<float>(pos.x + size.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"RC", MkFloat2(static_cast<float>(pos.x + size.x * 2) / fimg.x, fy), fsize, fuv);

								fy = static_cast<float>(pos.y + size.y * 2) / fimg.y;
								_RegisterSubset(subsetName.GetString() + L"LB", MkFloat2(static_cast<float>(pos.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"MB", MkFloat2(static_cast<float>(pos.x + size.x) / fimg.x, fy), fsize, fuv);
								_RegisterSubset(subsetName.GetString() + L"RB", MkFloat2(static_cast<float>(pos.x + size.x * 2) / fimg.x, fy), fsize, fuv);
							}
						}
					}
				}
				// 일반 서브셋이면 추가
				else
				{
					_RegisterSubset(subsetName, MkFloat2(static_cast<float>(pos.x) / fimg.x, static_cast<float>(pos.y) / fimg.y), fsize, fuv);
				}
			}
		}
	}
}

const MkFloat2& MkImageSubset::GetSubsetSize(const MkHashStr& name) const
{
	return (name.Empty() || (!m_Subsets.Exist(name))) ? m_DefaultSubset.originalSize : m_Subsets[name].originalSize;
}

void MkImageSubset::GetSubsetInfo(const MkHashStr& name, MkFloat2& localSize, MkFloat2 (&uv)[MkFloatRect::eMaxPointName]) const
{
	const Subset& ss = (name.Empty() || (!m_Subsets.Exist(name))) ? m_DefaultSubset : m_Subsets[name];

	localSize = ss.originalSize;
	memcpy_s(uv, sizeof(MkFloat2) * 4, ss.uv, sizeof(MkFloat2) * 4);
}

void MkImageSubset::Clear(void)
{
	m_Subsets.Clear();
}

MkImageSubset& MkImageSubset::operator = (const MkImageSubset& source)
{
	m_Subsets.Clear();
	source.__CopySubsets(m_Subsets);

	memcpy_s(&m_DefaultSubset, sizeof(Subset), source.__GetDefaultSubsetPtr(), sizeof(Subset));

	return *this;
}

void MkImageSubset::_RegisterSubset(const MkHashStr& name, const MkFloat2& fpos, const MkFloat2& fsize, const MkFloat2& fuv)
{
	Subset& ss = m_Subsets.Create(name);
	ss.originalSize = fsize;
	ss.uv[MkFloatRect::eLeftTop] = fpos;
	ss.uv[MkFloatRect::eRightTop] = MkFloat2(fpos.x + fuv.x, fpos.y);
	ss.uv[MkFloatRect::eLeftBottom] = MkFloat2(fpos.x, fpos.y + fuv.y);
	ss.uv[MkFloatRect::eRightBottom] = fpos + fuv;
}

//------------------------------------------------------------------------------------------------//