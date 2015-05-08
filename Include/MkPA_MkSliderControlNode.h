#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : slider
//
// ex>
//	MkSliderControlNode* hSliderNode = MkSliderControlNode::CreateChildNode(sbodyFrame, L"HSlider");
//	hSliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 210.f, 10, 90, 20);
//
//	MkSliderControlNode* vSliderNode = MkSliderControlNode::CreateChildNode(bodyClient, L"VSlider");
//	vSliderNode->SetVerticalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 150.f, 5, 15, 12);
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkWindowTagNode;

class MkSliderControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_SliderControlNode; }

	// alloc child instance
	static MkSliderControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// length : slider ���� pixel size
	// beginValue, valueSize : ����
	// initValue : �ʱ� ��(beginValue <= initValue <= (beginValue + valueSize))
	// showValue : cursor over�ų� ��ư �巡�� ���� �� �� ǥ�� ��� on/off
	void SetHorizontalSlider
		(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue = true);

	void SetVerticalSlider
		(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue = true);

	// ���� ����
	void SetSliderRange(int beginValue, int valueSize);

	// ���� �� ��ȯ
	inline int GetBeginValue(void) const { return m_BeginValue; }

	// ���� ���� ��ȯ
	inline int GetValueSize(void) const { return m_ValueSize; }

	// �� ����(beginValue <= value <= (beginValue + valueSize))
	void SetSliderValue(int value);

	// ���� �� ��ȯ
	inline int GetSliderValue(void) const { return m_CurrentValue; }

	//------------------------------------------------------------------------------------------------//
	// attribute
	//------------------------------------------------------------------------------------------------//

	virtual const MkFloatRect* GetDraggingMovementLock(void) const { return &m_DraggingMovementLock; }

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//
	// proceed
	//------------------------------------------------------------------------------------------------//

	virtual void Update(double currTime = 0.);

	virtual void Clear(void);

	MkSliderControlNode(const MkHashStr& name);
	virtual ~MkSliderControlNode() {}

protected:

	void _SetSlider
		(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int beginValue, int valueSize, int initValue, bool showValue, bool horizontal);

	void _ApplyValueToSliderButton(void);
	void _SendSliderMovementEvent(void);

	inline float _CalculateFraction(void) const { return (static_cast<float>(m_CurrentValue - m_BeginValue) / static_cast<float>(m_ValueSize)); }
	inline int _CalculateValue(float fraction) const { return (m_BeginValue + static_cast<int>(static_cast<float>(m_ValueSize) * fraction)); }

protected:

	float m_LineLength;

	int m_BeginValue;
	int m_ValueSize;
	int m_CurrentValue;

	bool m_Horizontal;

	bool m_CursorFocusing;
	bool m_ButtonDragging;
	double m_ValueTagTimeStamp;

	MkFloatRect m_DraggingMovementLock;

public:

	static const MkHashStr BtnNodeName;
	static const MkHashStr ValueTagName;

	static const MkHashStr ArgKey_SliderPos;
};
