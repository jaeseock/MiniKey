#pragma once

//------------------------------------------------------------------------------------------------//
// window - animation view
//------------------------------------------------------------------------------------------------//

//#include "MkPA_MkImageInfo.h"
//#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkTitleBarControlNode.h"

#include "AppDefinition.h"


//class MkPathName;
//class MkSceneNode;

class MainControlWindowNode : public MkTitleBarControlNode
{
public:

	void SetUp(const MkArray<MkSceneNode*>& rootNode);

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	void UpdateEffectSetting(void);

	MainControlWindowNode(const MkHashStr& name) : MkTitleBarControlNode(name) {}
	virtual ~MainControlWindowNode() {}

protected:

	void _AddEffect(MkSceneNode* bodyFrame, const MkStr& name, const MkStr& msg, float yOffset, bool check);

	bool _CheckAndUpdateCheckBox(const MkArray<MkHashStr>& path, const MkHashStr& key, bool onOff);

	void _UpdateCheckBox(const MkHashStr& key, bool onOff, MkWindowManagerNode* mgrNode);
	void _UpdateEditBtnEnable(const MkStr& key, bool onOff);

	bool _CheckAndActivateEditWindow(const MkArray<MkHashStr>& path, const MkHashStr& key);

	MkPanel* _GetMainPanel(eDrawStepType drawStep);
	void _SetShaderEffectEnableOfMainPanel(eDrawStepType drawStep, bool enable);
	
protected:

	MkArray<MkSceneNode*> m_RootNode;
};
