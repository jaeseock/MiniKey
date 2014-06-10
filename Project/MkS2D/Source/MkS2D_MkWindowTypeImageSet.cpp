
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkWindowTypeImageSet.h"


const static MkHashStr TYPE_KEY = L"Type";
const static MkHashStr IMAGE_KEY = L"Image";
const static MkHashStr SN_KEY = L"SN";
const static MkHashStr KEY31[3] = { L"L", L"M", L"R" };
const static MkHashStr KEY13[3] = { L"T", L"C", L"B" };
const static MkHashStr KEY33[9] = { L"LT", L"MT", L"RT", L"LC", L"MC", L"RC", L"LB", L"MB", L"RB" };

//------------------------------------------------------------------------------------------------//

void MkWindowTypeImageSet::SetUp(const MkDataNode& node)
{
	MkArray<MkHashStr> setNames;
	node.GetChildNodeList(setNames);
	MK_INDEXING_LOOP(setNames, i)
	{
		const MkHashStr& currSetName = setNames[i];
		const MkDataNode& currNode = *node.GetChildNode(currSetName);

		int type;
		MK_CHECK(currNode.GetData(TYPE_KEY, type, 0), currSetName.GetString() + L" window type image set에 " + TYPE_KEY.GetString() + L" key가 없음")
			continue;

		MkStr image;
		MK_CHECK(currNode.GetData(IMAGE_KEY, image, 0), currSetName.GetString() + L" window type image set에 " + IMAGE_KEY.GetString() + L" key가 없음")
			continue;

		MkArray<MkStr> snBuf;
		eSetType setType = static_cast<eSetType>(type);
		switch (setType)
		{
		case eSingleType:
			{
				snBuf.Fill(1);
				MK_CHECK(currNode.GetData(SN_KEY, snBuf[0], 0), currSetName.GetString() + L" window type image set에 " + SN_KEY.GetString() + L" key가 없음")
					continue;
			}
			break;
		case e3And1Type:
			{
				snBuf.Fill(3);
				MK_CHECK(currNode.GetData(KEY31[eL], snBuf[eL], 0), currSetName.GetString() + L" window type image set에 " + KEY31[eL].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY31[eM], snBuf[eM], 0), currSetName.GetString() + L" window type image set에 " + KEY31[eM].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY31[eR], snBuf[eR], 0), currSetName.GetString() + L" window type image set에 " + KEY31[eR].GetString() + L" key가 없음")
					continue;
			}
			break;
		case e1And3Type:
			{
				snBuf.Fill(3);
				MK_CHECK(currNode.GetData(KEY13[eT], snBuf[eT], 0), currSetName.GetString() + L" window type image set에 " + KEY13[eT].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY13[eC], snBuf[eC], 0), currSetName.GetString() + L" window type image set에 " + KEY13[eC].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY13[eB], snBuf[eB], 0), currSetName.GetString() + L" window type image set에 " + KEY13[eB].GetString() + L" key가 없음")
					continue;
			}
			break;
		case e3And3Type:
			{
				snBuf.Fill(9);
				MK_CHECK(currNode.GetData(KEY33[eLT], snBuf[eLT], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eLT].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eMT], snBuf[eMT], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eMT].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eRT], snBuf[eRT], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eRT].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eLC], snBuf[eLC], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eLC].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eMC], snBuf[eMC], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eMC].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eRC], snBuf[eRC], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eRC].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eLB], snBuf[eLB], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eLB].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eMB], snBuf[eMB], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eMB].GetString() + L" key가 없음")
					continue;
				MK_CHECK(currNode.GetData(KEY33[eRB], snBuf[eRB], 0), currSetName.GetString() + L" window type image set에 " + KEY33[eRB].GetString() + L" key가 없음")
					continue;
			}
			break;
		}

		Pack& currPack = m_Sets.Create(currSetName);
		currPack.type = static_cast<eSetType>(type);
		currPack.filePath = image;
		currPack.subsetNames.Reserve(snBuf.GetSize());
		MK_INDEXING_LOOP(snBuf, j)
		{
			currPack.subsetNames.PushBack(snBuf[j]);
		}

		MK_DEV_PANEL.MsgToLog(L"> image set : " + currSetName.GetString(), true);
	}
}

const MkHashStr& MkWindowTypeImageSet::GetImageSetKeyword(eSetType type, eTypeIndex index)
{
	switch (type)
	{
	case eSingleType: return SN_KEY;
	case e3And1Type: return KEY31[index];
	case e1And3Type: return KEY13[index];
	case e3And3Type: return KEY33[index];
	}

	return MkHashStr::NullHash;
}

MkWindowTypeImageSet::MkWindowTypeImageSet()
{
	m_Null.type = eNull;
}

//------------------------------------------------------------------------------------------------//
