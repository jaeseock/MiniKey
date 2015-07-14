#pragma once


//------------------------------------------------------------------------------------------------//
// window factory
// 자주 쓰이는 형태의 window 생성용 helper instance
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkPA_MkWindowThemeData.h"


class MkSceneNode;
class MkWindowTagNode;
class MkWindowBaseNode;
class MkTitleBarControlNode;

class MkWindowFactory
{
public:

	//------------------------------------------------------------------------------------------------//
	// 설정 변경
	//------------------------------------------------------------------------------------------------//

	void SetThemeName(const MkHashStr& themeName);
	inline const MkHashStr& ThemeName(void) const { return m_ThemeName; }

	inline void SetFrameType(MkWindowThemeData::eFrameType frameType) { m_FrameType = frameType; }
	inline MkWindowThemeData::eFrameType GetFrametype(void) const { return m_FrameType; }

	inline void SetMinClientSizeForButton(const MkFloat2& size) { m_MinClientSizeForButton = size; }
	inline const MkFloat2& GetMinClientSizeForButton(void) const { return m_MinClientSizeForButton; }

	//------------------------------------------------------------------------------------------------//
	// tag(text)
	//------------------------------------------------------------------------------------------------//

	MkWindowTagNode* CreateTextTagNode(const MkHashStr& name, const MkStr& message, MkFloat2& textRegion) const;
	MkWindowTagNode* CreateTextTagNode(const MkHashStr& name, const MkArray<MkHashStr>& message, MkFloat2& textRegion) const;

	//------------------------------------------------------------------------------------------------//
	// component
	//------------------------------------------------------------------------------------------------//

	// component node 생성
	MkWindowBaseNode* CreateComponentNode(const MkHashStr& name, MkWindowThemeData::eComponentType componentType) const;

	// button type component node 생성
	MkWindowBaseNode* CreateButtonTypeNode(const MkHashStr& name, MkWindowThemeData::eComponentType componentType, const MkStr& message) const;

	// normal button 생성
	MkWindowBaseNode* CreateNormalButtonNode(const MkHashStr& name, const MkStr& message) const;

	// ok button 생성
	MkWindowBaseNode* CreateOkButtonNode(void) const; // OkButtonName
	MkWindowBaseNode* CreateOkButtonNode(const MkStr& message) const; // OkButtonName

	// cancel button 생성
	MkWindowBaseNode* CreateCancelButtonNode(void) const; // CancelButtonName
	MkWindowBaseNode* CreateCancelButtonNode(const MkStr& message) const; // CancelButtonName

	//------------------------------------------------------------------------------------------------//
	// preset window : message box
	//------------------------------------------------------------------------------------------------//

	enum eMsgBoxType
	{
		eMBT_Default = 0, // eIT_Default
		eMBT_Notice, // eIT_Notice
		eMBT_Information, // eIT_Information
		eMBT_Warning, // eIT_Warning
		eMBT_EditMode // eIT_EditMode
	};

	enum eMsgBoxButton
	{
		eMBB_OK = 0, // OkButtonName
		eMBB_OkCancel // OkButtonName, CancelButtonName
	};

	// 휘발성 text
	MkTitleBarControlNode* CreateMessageBox
		(const MkHashStr& name, const MkStr& title, const MkStr& desc, MkWindowBaseNode* callBackWindow, eMsgBoxType boxType = eMBT_Default, eMsgBoxButton btnType = eMBB_OK) const;

	// 정적 text
	MkTitleBarControlNode* CreateMessageBox
		(const MkHashStr& name, const MkArray<MkHashStr>& title, const MkArray<MkHashStr>& desc, MkWindowBaseNode* callBackWindow, eMsgBoxType boxType = eMBT_Default, eMsgBoxButton btnType = eMBB_OK) const;

	// button 판별
	static bool IsOkButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName);
	static bool IsCancelButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName);

	//------------------------------------------------------------------------------------------------//

	MkWindowFactory();
	~MkWindowFactory() {}

protected:

	MkWindowTagNode* _CreateTextTagNode(const MkHashStr& name, const MkStr& message, const MkArray<MkHashStr>& textName, MkFloat2& textRegion) const;

	MkTitleBarControlNode* _CreateMessageBox
		(const MkHashStr& name, const MkStr& titleStr, const MkArray<MkHashStr>& titleTextName, const MkStr& descStr, const MkArray<MkHashStr>& descTextName,
		MkWindowBaseNode* callBackWindow, eMsgBoxType boxType, eMsgBoxButton btnType) const;

	static bool _IsMsgBoxButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName, const MkHashStr& buttonName);

protected:

	MkHashStr m_ThemeName;
	MkWindowThemeData::eFrameType m_FrameType;
	MkFloat2 m_MinClientSizeForButton;

public:

	static const MkHashStr OneAndOnlyTagName;
	static const MkHashStr OkButtonName;
	static const MkHashStr CancelButtonName;
	static const MkHashStr BodyFrameName;
};
