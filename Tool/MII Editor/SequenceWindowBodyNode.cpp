
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"

#include "AppWindowCreationHelper.h"
#include "AnimationViewWindowNode.h"
#include "SequenceWindowBodyNode.h"


//------------------------------------------------------------------------------------------------//

#define APPDEF_FRAMES_PER_SEC 30

const static MkHashStr DELETE_SUBSET_KEY = L"< �� �� >";


void SequenceWindowBodyNode::CreateControls(void)
{
	// setting
	{
		MkWindowThemedNode* guideNode = AppWindowCreationHelper::AddStaticGuide(this, L"GD:Setting", 10.f, m_ClientRect.size.y - 60.f, 480.f, 30.f);

		AppWindowCreationHelper::AddStaticTextPanel(guideNode, L"ST:TotalRange", L"��� �ð�(��)", 30.f, 12.f);
		AppWindowCreationHelper::AddEditBoxWindow(guideNode, L"TotalRangeEB", MkStr::EMPTY, 109, 9.f, 64.f);

		AppWindowCreationHelper::AddStaticTextPanel(guideNode, L"ST:Pivot", L"������", 232.f, 12.f);
		MkDropDownListControlNode* pivotList = AppWindowCreationHelper::AddDropdownListWindow(guideNode, L"PivotDL", 273.f, 9.f, 40.f, 9);
		pivotList->SetLocalDepth(-10.f); // frame drop box���� �� ��
		pivotList->AddItem(MkStr(eRAP_LeftBottom), L"��");
		pivotList->AddItem(MkStr(eRAP_LeftCenter), L"��");
		pivotList->AddItem(MkStr(eRAP_LeftTop), L"��");
		pivotList->AddItem(MkStr(eRAP_MiddleBottom), L"��");
		pivotList->AddItem(MkStr(eRAP_MiddleCenter), L"��");
		pivotList->AddItem(MkStr(eRAP_MiddleTop), L"��");
		pivotList->AddItem(MkStr(eRAP_RightBottom), L"��");
		pivotList->AddItem(MkStr(eRAP_RightCenter), L"��");
		pivotList->AddItem(MkStr(eRAP_RightTop), L"��");
		pivotList->SetTargetItemKey(MkStr(eRAP_LeftBottom));

		AppWindowCreationHelper::AddCheckBoxWindow(guideNode, L"LoopCB", L"�ݺ����", 384.f, 9.f, true);
	}

	AppWindowCreationHelper::AddOkButton(this, L"���� ���� �ݿ� �� ���� ����", 520.f, m_ClientRect.size.y - 60.f, 200.f, 30.f);

	// subset list
	{
		const float TextWidth = 200.f;
		AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SubsetList", L"�� ����� ���", 10.f, m_ClientRect.size.y - 90.f);

		AppWindowCreationHelper::AddEditBoxWindow(this, L"SubsetFilterEB", MkStr::EMPTY, 10.f, m_ClientRect.size.y - 116.f, TextWidth + 6.f);
		AppWindowCreationHelper::AddListBoxWindow(this, L"SubsetListLB", 10.f, 10.f, TextWidth, 30);

		AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SubsetInOut", L"����", TextWidth + 29.f, m_ClientRect.size.y - 114.f);

		AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:CurrFrame", L"�� ��� ������(30 FPS)", TextWidth + 60.f, m_ClientRect.size.y - 90.f);
		MkDropDownListControlNode* frameList = AppWindowCreationHelper::AddDropdownListWindow
			(this, L"TargetFrameDL", TextWidth + 60.f, m_ClientRect.size.y - 116.f, TextWidth + 30.f, 30);
		frameList->SetLocalDepth(-5.f);
		
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnToBegin", L"��", TextWidth + 60.f, m_ClientRect.size.y - 150.f, 28.f);
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnSubOneSec", L"<<", TextWidth + 100.f, m_ClientRect.size.y - 150.f, 28.f);
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnSubOneFrame", L"<", TextWidth + 140.f, m_ClientRect.size.y - 150.f, 28.f);
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnAddOneFrame", L" >", TextWidth + 182.f, m_ClientRect.size.y - 150.f, 28.f);
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnAddOneSec", L" >>", TextWidth + 222.f, m_ClientRect.size.y - 150.f, 28.f);
		AppWindowCreationHelper::AddNormalButton(this, L"FrameNavBtnToEnd", L" ��", TextWidth + 262.f, m_ClientRect.size.y - 150.f, 28.f);

		AppWindowCreationHelper::AddSlideBarWindow(this, L"TargetFrameSB", TextWidth + 60.f, m_ClientRect.size.y - 184.f, TextWidth + 30.f);
	}
}

void SequenceWindowBodyNode::WakeUp(const MkPathName& imagePath, const MkHashStr& name)
{
	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(imagePath);
	if ((texture == NULL) || (!texture->GetImageInfo().GetSequences().Exist(name)))
		return;

	m_SeqName = name;
	m_Sequence = texture->GetImageInfo().GetSequences()[name];
	texture->GetImageInfo().GetSubsets().GetKeyList(m_AllSubsets);
	m_AllSubsets.EraseFirstInclusion(MkArraySection(0), MkHashStr::EMPTY);
	m_AllSubsets.SortInAscendingOrder();

	// title
	{
		MkTitleBarControlNode* titleBar = dynamic_cast<MkTitleBarControlNode*>(GetParentNode());
		if (titleBar != NULL)
		{
			titleBar->SetCaption(L"������ ���� : " + m_SeqName.GetString(), eRAP_LeftCenter);
		}
	}

	// setting
	MkSceneNode* guideNode = GetChildNode(L"GD:Setting");
	if (guideNode != NULL)
	{
		MkEditBoxControlNode* rangeNode = dynamic_cast<MkEditBoxControlNode*>(guideNode->GetChildNode(L"TotalRangeEB"));
		rangeNode->SetText(MkStr(m_Sequence.totalRange));

		MkDropDownListControlNode* pivotList = dynamic_cast<MkDropDownListControlNode*>(guideNode->GetChildNode(L"PivotDL"));
		pivotList->SetTargetItemKey(MkStr(m_Sequence.pivot));

		MkCheckBoxControlNode* loopNode = dynamic_cast<MkCheckBoxControlNode*>(guideNode->GetChildNode(L"LoopCB"));
		loopNode->SetCheck(m_Sequence.loop);
	}

	// subset list
	{
		MkEditBoxControlNode* filterNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SubsetFilterEB"));
		filterNode->SetText(MkStr::EMPTY);

		MkListBoxControlNode* listNode = dynamic_cast<MkListBoxControlNode*>(GetChildNode(L"SubsetListLB"));
		listNode->ClearAllItems();

		listNode->AddItem(DELETE_SUBSET_KEY, DELETE_SUBSET_KEY.GetString());

		MK_INDEXING_LOOP(m_AllSubsets, i)
		{
			listNode->AddItem(m_AllSubsets[i], m_AllSubsets[i].GetString());
		}
	}

	// frame list
	_UpdateFrameDataControl();

	// animation view
	{
		if (ChildExist(L"AniView"))
		{
			RemoveChildNode(L"AniView");
		}

		MkFloat2 clientSize;
		AnimationViewWindowNode* viewNode = new AnimationViewWindowNode(L"AniView");
		AttachChildNode(viewNode);
		viewNode->SetLocalDepth(-1.f);
		viewNode->SetImagePath(imagePath);
		viewNode->UpdateSequence(MkHashStr::EMPTY, m_Sequence, clientSize);
		viewNode->SetLocalPosition(MkFloat2(530.f, m_ClientRect.size.y - 90.f - clientSize.y));
	}
}

void SequenceWindowBodyNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_TextCommitted)
	{
		MkStr buffer;
		if (argument->GetData(MkEditBoxControlNode::ArgKey_Text, buffer, 0))
		{
			if (path.GetSize() == 2)
			{
				if (path[1].Equals(0, L"TotalRangeEB"))
				{
					float newRange = buffer.ToFloat();
					if ((newRange > 0.001f) && (!MkFloatOp::CloseToNear(newRange, static_cast<float>(m_Sequence.totalRange))))
					{
						m_Sequence.totalRange = static_cast<double>(newRange);
						_UpdateFrameDataControl();
						_UpdateSequenceAniView();
					}
				}
			}
			else if (path.GetSize() == 1)
			{
				if (path[0].Equals(0, L"SubsetFilterEB"))
				{
					MkArray<MkHashStr> filter(m_AllSubsets.GetSize());
					filter.PushBack(DELETE_SUBSET_KEY);
					
					MkListBoxControlNode* listNode = dynamic_cast<MkListBoxControlNode*>(GetChildNode(L"SubsetListLB"));
					if (buffer.Empty())
					{
						filter += m_AllSubsets;
					}
					else
					{
						MK_INDEXING_LOOP(m_AllSubsets, i)
						{
							if (m_AllSubsets[i].GetString().Exist(buffer))
							{
								filter.PushBack(m_AllSubsets[i]);
							}
						}
					}
					listNode->SetItemSequence(filter);
				}
			}
		}
	}
	else if (eventType == ePA_SNE_DropDownItemSet)
	{
		MkHashStr buffer;
		if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0) && (!buffer.Empty()))
		{
			if ((path.GetSize() == 2) && path[1].Equals(0, L"PivotDL"))
			{
				m_Sequence.pivot = static_cast<eRectAlignmentPosition>(buffer.GetString().ToInteger());

				_UpdateSequenceAniView();
			}
			else if ((path.GetSize() == 1) && path[0].Equals(0, L"TargetFrameDL"))
			{
				MkSliderControlNode* slideNode = dynamic_cast<MkSliderControlNode*>(GetChildNode(L"TargetFrameSB"));
				slideNode->SetSliderValue(buffer.GetString().ToInteger());
			}
		}
	}
	else if (eventType == ePA_SNE_CheckOn)
	{
		if ((path.GetSize() == 2) && path[1].Equals(0, L"LoopCB"))
		{
			m_Sequence.loop = true;
		}
	}
	else if (eventType == ePA_SNE_CheckOff)
	{
		if ((path.GetSize() == 2) && path[1].Equals(0, L"LoopCB"))
		{
			m_Sequence.loop = false;
		}
	}
	else if (eventType == ePA_SNE_ItemRBtnPressed)
	{
		if ((path.GetSize() == 1) && path[0].Equals(0, L"SubsetListLB"))
		{
			MkHashStr buffer;
			if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0))
			{
				MkDropDownListControlNode* frameList = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"TargetFrameDL"));
				const MkHashStr& targetKey = frameList->GetTargetItemKey();
				unsigned int frameIndex = targetKey.GetString().ToUnsignedInteger();
				_FrameData& frameData = m_FrameData[frameIndex];
				MkStr msg;

				// �� �����ӿ��� ����� ����
				if (buffer == DELETE_SUBSET_KEY)
				{
					if ((frameIndex > 0) && (!frameData.subsetName.Empty())) // ù��° �������� ���� �Ұ�
					{
						frameData.subsetName.Clear();
						_GetFrameDataItemMsg(frameIndex, msg);

						// m_Sequence���� �ش� pair ����
						MkArray<double> keyList;
						m_Sequence.tracks.GetKeyList(keyList);
						MK_INDEXING_LOOP(keyList, i)
						{
							if (MkFloatOp::CloseToNear(static_cast<float>(keyList[i]), m_FrameData[frameIndex].timeStame))
							{
								m_Sequence.tracks.Erase(keyList[i]);
								break;
							}
						}

						_UpdateSequenceAniView();
					}
				}
				// �� �����ӿ��� ����� ����
				else
				{
					if (buffer != frameData.subsetName)
					{
						frameData.subsetName = buffer;
						_GetFrameDataItemMsg(frameIndex, msg);

						// m_Sequence�� �ش� pair ����
						bool addNewPair = true;
						MkArray<double> keyList;
						m_Sequence.tracks.GetKeyList(keyList);
						MK_INDEXING_LOOP(keyList, i)
						{
							if (MkFloatOp::CloseToNear(static_cast<float>(keyList[i]), m_FrameData[frameIndex].timeStame))
							{
								m_Sequence.tracks[keyList[i]] = buffer;
								addNewPair = false;
								break;
							}
						}

						// Ȥ�� �߰�
						if (addNewPair)
						{
							m_Sequence.tracks.Create(static_cast<double>(m_FrameData[frameIndex].timeStame), buffer);
						}

						_UpdateSequenceAniView();
					}
				}

				if (!msg.Empty())
				{
					frameList->SetItemMessage(targetKey, msg);
				}
			}
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnPressed)
	{
		if (path.GetSize() == 1)
		{
			// ok
			if (path[0].Equals(0, L"__#WC:Ok"))
			{
				if (ChildExist(L"AniView"))
				{
					AnimationViewWindowNode* viewNode = dynamic_cast<AnimationViewWindowNode*>(GetChildNode(L"AniView"));
					if (viewNode != NULL)
					{
						viewNode->SetSequenceData(m_SeqName, m_Sequence);
					}

					RemoveChildNode(L"AniView");

					StartNodeReportTypeEvent(ePA_SNE_CloseWindow, NULL); // ����
				}
			}
			else if (!m_FrameData.Empty())
			{
				// ������ Ž�� ��ư
				if (path[0].GetString().CheckPrefix(L"FrameNavBtn"))
				{
					MkDropDownListControlNode* frameList = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"TargetFrameDL"));
					unsigned int lastFrameIndex = frameList->GetTargetItemKey().GetString().ToUnsignedInteger();
					unsigned int currFrameIndex = lastFrameIndex;

					if (path[0].Equals(0, L"FrameNavBtnToBegin"))
					{
						currFrameIndex = 0;
					}
					else if (path[0].Equals(0, L"FrameNavBtnToEnd"))
					{
						currFrameIndex = m_FrameData.GetSize() - 1;
					}
					else if (path[0].Equals(0, L"FrameNavBtnSubOneSec"))
					{
						currFrameIndex = (lastFrameIndex < APPDEF_FRAMES_PER_SEC) ? 0 : (lastFrameIndex - APPDEF_FRAMES_PER_SEC);
					}
					else if (path[0].Equals(0, L"FrameNavBtnSubOneFrame"))
					{
						currFrameIndex = (lastFrameIndex == 0) ? 0 : (lastFrameIndex - 1);
					}
					else if (path[0].Equals(0, L"FrameNavBtnAddOneFrame"))
					{
						currFrameIndex = GetMin<unsigned int>(lastFrameIndex + 1, m_FrameData.GetSize() - 1);
					}
					else if (path[0].Equals(0, L"FrameNavBtnAddOneSec"))
					{
						currFrameIndex = GetMin<unsigned int>(lastFrameIndex + APPDEF_FRAMES_PER_SEC, m_FrameData.GetSize() - 1);
					}

					if (currFrameIndex != lastFrameIndex)
					{
						frameList->SetTargetItemKey(MkStr(currFrameIndex));

						MkSliderControlNode* slideNode = dynamic_cast<MkSliderControlNode*>(GetChildNode(L"TargetFrameSB"));
						slideNode->SetSliderValue(static_cast<int>(currFrameIndex));
					}
				}
			}
		}
	}
	else if (eventType == ePA_SNE_SliderMoved)
	{
		int buffer;
		if (argument->GetData(MkSliderControlNode::ArgKey_SliderPos, buffer, 0))
		{
			if (path.GetSize() == 1)
			{
				if (path[0].Equals(0, L"TargetFrameSB"))
				{
					MkDropDownListControlNode* frameList = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"TargetFrameDL"));
					frameList->SetTargetItemKey(MkStr(buffer));
				}
			}
		}
	}
	//else if ((eventType == ePA_SNE_CursorLBtnReleased) || (eventType == ePA_SNE_CursorLeft))

	MkBodyFrameControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

//------------------------------------------------------------------------------------------------//

void SequenceWindowBodyNode::_GetFrameDataItemMsg(unsigned int frameIndex, MkStr& buffer) const
{
	const _FrameData& frameData = m_FrameData[frameIndex];

	MkStr msg;
	msg.Reserve(16 + frameData.subsetName.GetSize());
	if (frameIndex < 10)
	{
		msg += L"00";
	}
	else if (frameIndex < 100)
	{
		msg += L"0";
	}
	msg += frameIndex;
	msg += L" - ";
	msg += frameData.timeStame;
	if (!frameData.subsetName.Empty())
	{
		msg += L" : ";
		msg += frameData.subsetName.GetString();
	}
	buffer = msg;
}

void SequenceWindowBodyNode::_UpdateFrameDataControl(void)
{
	const float FPS = static_cast<float>(APPDEF_FRAMES_PER_SEC);
	int totalFrameCount = static_cast<int>(MkFloatOp::SnapToUpperBound(FPS * static_cast<float>(m_Sequence.totalRange), 1.f));

	// frame data ����
	m_FrameData.Clear();
	m_FrameData.Fill(totalFrameCount);

	MkMapLooper<double, MkHashStr> trackLooper(m_Sequence.tracks);
	MK_STL_LOOP(trackLooper)
	{
		int frameIndex = static_cast<int>(static_cast<float>(trackLooper.GetCurrentKey()) * FPS);
		if (m_FrameData.IsValidIndex(frameIndex))
		{
			if (m_FrameData[frameIndex].subsetName.Empty()) // �Էµ� track�� �ػ󵵰� ����ġ�� ���� �� ��� ���
			{
				m_FrameData[frameIndex].subsetName = trackLooper.GetCurrentField();
			}
		}
	}
	m_Sequence.tracks.Clear();

	// frame list control�� ������ �߰�
	MkDropDownListControlNode* frameList = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"TargetFrameDL"));
	frameList->ClearAllItems();

	MK_INDEXING_LOOP(m_FrameData, i)
	{
		_FrameData& frameData = m_FrameData[i];
		frameData.timeStame = static_cast<float>(i) / FPS;

		if (!frameData.subsetName.Empty())
		{
			m_Sequence.tracks.Create(static_cast<double>(frameData.timeStame), frameData.subsetName); // track �籸��
		}
		
		MkStr msg;
		_GetFrameDataItemMsg(i, msg);
		frameList->AddItem(MkStr(i), msg);
	}
	
	// slide ���� �缳��
	MkSliderControlNode* slideNode = dynamic_cast<MkSliderControlNode*>(GetChildNode(L"TargetFrameSB"));
	slideNode->SetSliderRange(0, totalFrameCount - 1);
}

void SequenceWindowBodyNode::_UpdateSequenceAniView(void)
{
	AnimationViewWindowNode* viewNode = dynamic_cast<AnimationViewWindowNode*>(GetChildNode(L"AniView"));
	if (viewNode != NULL)
	{
		MkFloat2 clientSize;
		viewNode->UpdateSequence(MkHashStr::EMPTY, m_Sequence, clientSize);
		viewNode->SetLocalPosition(MkFloat2(530.f, m_ClientRect.size.y - 90.f - clientSize.y));
	}
}

//------------------------------------------------------------------------------------------------//
