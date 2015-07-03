#pragma once


//------------------------------------------------------------------------------------------------//
// system window : message box
//
// ex>
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkTitleBarControlNode.h"

class MkSystemWindow
{
public:

	//------------------------------------------------------------------------------------------------//
	// button : ok
	//------------------------------------------------------------------------------------------------//

	static MkWindowBaseNode* CreateNormalButton(const MkHashStr& name);
	static MkWindowBaseNode* CreateOkButton(const MkHashStr& name);
	static MkWindowBaseNode* CreateCancelButton(const MkHashStr& name);

	class MkMessageBox : public MkTitleBarControlNode
	{
	public:

		// node type
		virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_MessageBox; }

		// alloc child instance
		static MkMessageBox* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

		//------------------------------------------------------------------------------------------------//
		// control interface
		//------------------------------------------------------------------------------------------------//

		enum eUsageType
		{
			eUT_Default = 0,
			eUT_Notice,
			eUT_Information,
			eUT_Warning,
			eUT_EditMode
		};

		enum eFrameType
		{
			eFT_Ok = 0,
			eFT_OkCancel
		};

		void SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkStr& message);
		void SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkTextNode& textNode);
		void SetMessageBox(const MkHashStr& themeName, eUsageType usageType, eFrameType frameType, const MkArray<MkHashStr>& textName);

		//------------------------------------------------------------------------------------------------//
		// event
		//------------------------------------------------------------------------------------------------//

		virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

		//------------------------------------------------------------------------------------------------//
		// MkSceneObject
		//------------------------------------------------------------------------------------------------//

		virtual void Save(MkDataNode& node) const;

		MKDEF_DECLARE_SCENE_OBJECT_HEADER;

		MkMessageBox(const MkHashStr& name);
		virtual ~MkMessageBox() {}

	public:

		static const MkHashStr TitleNodeName;
		static const MkHashStr BodyNodeName;
		static const MkHashStr TextPanelName;
		static const MkHashStr OkButtonNodeName;
		static const MkHashStr CancelButtonNodeName;
		static const MkHashStr ButtonPanelName;
	};

protected:

	static MkWindowBaseNode* _CreateDefaultComponent(const MkHashStr& name, MkWindowThemeData::eComponentType componentType);
	static MkWindowBaseNode* _CreateDefaultButton(const MkHashStr& name, MkWindowThemeData::eComponentType componentType, const MkStr& message);

public:

	static const MkHashStr OneAndOnlyTagName;
};
