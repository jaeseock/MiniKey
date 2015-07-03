#pragma once


//------------------------------------------------------------------------------------------------//
// window dispositioner
// ex>
//	MkBodyFrameControlNode* bodyFrame...
//	MkWindowTagNode* tagNode...
//	MkWindowBaseNode* okBtn...
//	MkWindowBaseNode* cancelBtn...
//
//	...
//
//	MkWindowDispositioner windowDispositioner;
//
//	windowDispositioner.AddNewLine(); // �� ����
//	windowDispositioner.AddDummyToCurrentLine(titleNode->CalculateWindowSize());
//
//	windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // text tag�� �߾� ����
//	windowDispositioner.AddRectToCurrentLine(tagNode->GetNodeName(), tagNode->CalculateWindowSize());
//
//	windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // button set�� �߾� ����
//	windowDispositioner.AddRectToCurrentLine(okBtn->GetNodeName(), okBtn->CalculateWindowSize());
//	windowDispositioner.AddRectToCurrentLine(cancelBtn->GetNodeName(), cancelBtn->CalculateWindowSize());
//
//	windowDispositioner.Calculate(); // dispositioner ���
//
//	MkFloat2 clientSize = windowDispositioner.GetRegion(); // body frame size
//	bodyFrame->SetBodyFrame(m_ThemeName, bodyType, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, clientSize);
//
//	// ���� node ��ġ �ݿ�
//	windowDispositioner.ApplyPositionToNode(tagNode);
//	windowDispositioner.ApplyPositionToNode(okBtn);
//	windowDispositioner.ApplyPositionToNode(cancelBtn);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkRect.h"


class MkVisualPatternNode;

class MkWindowDispositioner
{
public:

	//------------------------------------------------------------------------------------------------//

	// ��ü ������ �߰� ����/���� ��
	inline void SetMargin(const MkFloat2& margin) { m_Margin = margin; }
	inline const MkFloat2& GetMargin(void) const { return m_Margin; }

	// rect�� �Ÿ�
	inline void SetOffset(const MkFloat2& offset) { m_Offset = offset; }
	inline const MkFloat2& GetOffset(void) const { return m_Offset; }

	//------------------------------------------------------------------------------------------------//

	enum eLineHorizontalAlign
	{
		eLHA_Left = 0, // default
		eLHA_Center,
		eLHA_Right
	};

	enum eLineVerticalAlign
	{
		eLVA_Top = 0,
		eLVA_Center, // default
		eLVA_Bottom
	};

	// �� ���� �߰�. ��ȯ���� ������ ���� ��ȣ
	unsigned int AddNewLine(eLineHorizontalAlign horizontalAlign = eLHA_Left, eLineVerticalAlign verticalAlign = eLVA_Center);

	// ���� ���ο� rect�� ���
	bool AddRectToCurrentLine(const MkHashStr& name, const MkFloat2& size);

	// ���� ���ο� dummy�� ���
	bool AddDummyToCurrentLine(const MkFloat2& size);

	// ��� �Ϸ� ����. ��� ����
	void Calculate(void);

	// ��� �Ϸ�� ��ü ���� ��ȯ
	inline const MkFloat2& GetRegion(void) const { return m_Region; }

	// �ش� rect�� ��� ��� ��ȯ
	const MkFloatRect& GetRect(const MkHashStr& name) const;

	// �ش� rect�� ��� ��� �ݿ�
	bool ApplyPositionToNode(MkVisualPatternNode* node) const;

	// �ʱ�ȭ
	void Clear(void);
	
	MkWindowDispositioner();
	~MkWindowDispositioner() {}

protected:

	typedef struct __LineInfo
	{
		// given
		MkArray<MkHashStr> rects;

		eLineHorizontalAlign horizontalAlign;
		eLineVerticalAlign verticalAlign;

		// calculation
		MkFloatRect region;
	}
	_LineInfo;

protected:

	// given
	MkFloat2 m_Margin;
	MkFloat2 m_Offset;

	MkHashMap<MkHashStr, MkFloatRect> m_RectTable;
	MkArray<_LineInfo> m_Lines;

	// result
	MkFloat2 m_Region;
};
