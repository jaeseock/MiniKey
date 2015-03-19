#pragma once


//------------------------------------------------------------------------------------------------//
// �ϳ��� �̹����� �������� �κ����� �ɰ� ����ϱ� ���� ����
// - Subset�� static image
// - Sequence�� time table�� ���� Subset list, �� �̹��� ��ü �ִϸ��̼���
// �ܺ� ���������� Subset�� Sequence�� ���� �� �ǹ̰� ����
//
// ex> subset
//	Node "PRIFIX(single�� ��� NAME)"
//	{
//		int2 Position = (0, 0); // (opt)��ġ. �⺻���� (0, 0)
//		int2 Size = (101, 101); // ũ��
//		int2 Table = (1, 1); // (opt)���̺��� ���(xy). �⺻���� (1, 1). �⺻���� (1, 1)�� �ƴ� ���(������ subset �ǹ�) PRIFIX �ڿ� numbering�� ����
//		int2 Offset = (101, 101); // (opt)Table���� ������ subset�� �ǹ��� ��� ���� subset Ž������ �̵� �Ÿ�. �����̵��� x, �����̵��� y ���. �⺻���� Size
//	}
//
// ex> sequence
//	Node "NAME"
//	{
//		float TotalRange = 4.5; // �� �ð� ������ ����(sec)
//		str SubsetList = "P_0" / "P_1" / "P_2"; // subset list
//		//str SubsetList = "~" / "P_" / "0" / "2"; // ���� ����Ʈ�� �̷� �����ε� ǥ�� ����. ������ ("~" / "PREFIX" / "1st FIRST_INDEX" / "1st LAST_INDEX" / "2nd FIRST_INDEX" / "2nd LAST_INDEX" / ...)
//		float TimeList = 0 / 1.5 / 3; // (opt)subset list�� 1:1 �����Ǵ� ���� �ð�. ������ Range�� Subset ������ŭ �����ϰ� �ɰ��� ���
//		bool Loop = yes; // (opt)���� ����. �⺻���� true
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"

class MkSceneNode;
class MkWindowBaseNode;

class MkWindowOpHelper
{
public:

	static MkWindowBaseNode* CreateTitleBar(
		const MkHashStr& name,
		const MkHashStr& themeName,
		float length,
		bool useCloseBtn,
		const MkHashStr& iconPath,
		const MkHashStr& iconSubsetOrSequenceName,
		const MkStr& titleCaption,
		bool alignCaptionToCenter
		);
};
