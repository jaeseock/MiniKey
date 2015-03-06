
#include "MkCore_MkCheck.h"
#include "MkCore_MkTimeManager.h"

//#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkWindowThemedNode.h"
#include "MkPA_MkWindowBaseNode.h"


//------------------------------------------------------------------------------------------------//

MkWindowBaseNode* MkWindowBaseNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkWindowBaseNode* node = __TSI_SceneNodeDerivedInstanceOp<MkWindowBaseNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkWindowBaseNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowBaseNode::PickWindowBaseNode(MkArray<MkWindowBaseNode*>& buffer, const MkFloat2& worldPoint, float startDepth) const
{
	buffer.Clear();

	MkArray<MkPanel*> panelBuffer;
	MkBitField32 filterAttr;
	filterAttr.Set(eAT_AcceptInput); // input을 받아들이는 node만 대상으로 함

	// panel picking
	if (PickPanel(panelBuffer, worldPoint, startDepth, filterAttr))
	{
		buffer.Reserve(panelBuffer.GetSize());

		MK_INDEXING_LOOP(panelBuffer, i)
		{
			// MkWindowBaseNode 파생 객체만 대상으로 함
			MkSceneNode* targetNode = panelBuffer[0]->GetParentNode()->FindNearestDerivedNode(ePA_SNT_WindowBaseNode);
			MkWindowBaseNode* winBaseNode = dynamic_cast<MkWindowBaseNode*>(targetNode);
			if ((winBaseNode != NULL) && (buffer.FindFirstInclusion(MkArraySection(0), winBaseNode)))
			{
				buffer.PushBack(winBaseNode);
			}
		}
	}
	return !buffer.Empty();
}

//------------------------------------------------------------------------------------------------//
/*
void MkWindowBaseNode::SendRootToLeafDirectionNodeEvent(int eventType, MkDataNode& argument)
{
	switch (eventType)
	{
	case eET_ChangeTheme:
		{
			MkArray<MkHashStr> names;
			if (argument.GetDataEx(CHANGE_THEME_ARG_KEY, names) && (names.GetSize() == 2))
			{
				if (names[0].Empty() || (names[0] == m_ThemeName))
				{
					SetThemeName(names[1]);
				}
			}
		}
		break;
	}
	
	MkVisualPatternNode::SendRootToLeafDirectionNodeEvent(eventType, argument);
}
*/
void MkWindowBaseNode::Clear(void)
{
	MkWindowThemedNode::Clear();
}

MkWindowBaseNode::MkWindowBaseNode(const MkHashStr& name) : MkWindowThemedNode(name)
{
	SetEnable(true);
}

//------------------------------------------------------------------------------------------------//