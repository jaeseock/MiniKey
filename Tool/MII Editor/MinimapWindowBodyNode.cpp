
#include "MkCore_MkVec2.h"
#include "MkCore_MkDataNode.h"
#include "MinimapWindowBodyNode.h"


//------------------------------------------------------------------------------------------------//

void MinimapWindowBodyNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (((eventType == ePA_SNE_CursorLBtnPressed) && (path.GetSize() == 0)) ||
		(m_Hitting && (eventType == ePA_SNE_CursorMoved)))
	{
		MkVec2 pt;
		if (argument->GetData(MkWindowBaseNode::ArgKey_CursorLocalPosition, pt, 0))
		{
			MkPanel* panel = GetPanel(L"BG");
			MkFloatRect rect(panel->GetLocalPosition(), panel->GetPanelSize());
			MkFloat2 hitPos(pt.x, pt.y);

			// 배경(world) 너머로 나갔을 경우 근접한 배경의 위치를 할당
			if (!rect.CheckIntersection(hitPos))
			{
				MkFloat2 LB = rect.GetAbsolutePosition(MkFloatRect::eLeftBottom);
				MkFloat2 RT = rect.GetAbsolutePosition(MkFloatRect::eRightTop);

				if (hitPos.x <= LB.x)
				{
					hitPos.x = LB.x;
					if (hitPos.y >= RT.y) // LT
						hitPos.y = RT.y;
					else if (hitPos.y <= LB.y) // LB
						hitPos.y = LB.y;
				}
				else if (hitPos.x >= RT.x)
				{
					hitPos.x = RT.x;
					if (hitPos.y >= RT.y) // RT
						hitPos.y = RT.y;
					else if (hitPos.y <= LB.y) // RB
						hitPos.y = LB.y;
				}
				else
				{
					if (hitPos.y >= RT.y) // CT
						hitPos.y = RT.y;
					else if (hitPos.y <= LB.y) // CB
						hitPos.y = LB.y;
				}
			}

			m_Hitting = true;
			m_HitPosition = hitPos - rect.position;
			m_HitPosition.x /= rect.size.x;
			m_HitPosition.y /= rect.size.y;
		}
	}
	else if ((eventType == ePA_SNE_CursorLBtnReleased) || (eventType == ePA_SNE_CursorLeft))
	{
		m_Hitting = false;
	}

	MkBodyFrameControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

//------------------------------------------------------------------------------------------------//
