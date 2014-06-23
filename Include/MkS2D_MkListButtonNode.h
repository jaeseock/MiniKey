#pragma once


//------------------------------------------------------------------------------------------------//
// list button node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseWindowNode.h"


class MkListButtonNode : public MkBaseWindowNode
{
public:

	enum eListButtonType // ��ư ����
	{
		eSeletionRoot = 0, // ���õ� ��ư�� ǥ���ϴ� ��Ʈ ��ư(ex> �Ϲ� list button)
		eConstantRoot, // ǥ�ó����� ������ �ʴ� ��Ʈ(ex> ������ ���� ��ư)
		eChildButton // �Ϲ� ��ư
	};

	enum eOpeningMethod // ����Ʈ ������ ���
	{
		ePressAndOnCursorInTimeOnly = 0, // �����ų� ���� �ð� �ӹ�����
		ePressOnly, // ���� ��
		eOnCursorInTimeOnly, // ���� �ð� �ӹ��� ��
		eRelease // Ŭ���Ǿ��� ��
	};

	enum eOpeningDirection // ����Ʈ ���� ����
	{
		eRightside = 0, // ���� ����
		eDownward, // ���� ����
		eUpward // ���� ����
	};

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_ListButtonNode; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	//bool CreateListButton(eListButtonType btnType, eOpeningMethod opMethod, eOpeningDirection opDir,
	//	const MkHashStr& themeName, const MkPathName& iconPath, const MkHashStr& iconSubset)

	//bool CreateList(const MkArray<MkHashStr>& menuList, unsigned int initIndex);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	virtual bool InputEventMousePress(unsigned int button, const MkFloat2& position, bool managedRoot);
	virtual bool InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot);

	MkListButtonNode(const MkHashStr& name) : MkBaseWindowNode(name) {}
	MkListButtonNode(const MkHashStr& name, const MkHashStr& themeName, const MkFloat2& bodySize, const MkHashStr& componentName) : MkBaseWindowNode(name, themeName, bodySize, componentName) {}
	virtual ~MkListButtonNode() {}

public:

	static void __GenerateBuildingTemplate(void);

protected:

	MkHashStr m_TargetNodeName;
};
