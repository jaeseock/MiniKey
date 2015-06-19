#pragma once


//------------------------------------------------------------------------------------------------//
// MkSceneObjectInterface
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkTypeHierarchy.h"

#include "MkPA_MkGlobalDefinition.h"


class MkDataNode;
class MkSceneNode;

//------------------------------------------------------------------------------------------------//

// �Ļ� class header �����
#define MKDEF_DECLARE_SCENE_OBJECT_HEADER \
	static const MkHashStr& SceneClassKey(void); \
	virtual const MkHashStr& GetSceneClassKey(void) const; \
	static void SetObjectTemplate(MkDataNode& node); \
	virtual void LoadObject(const MkDataNode& node); \
	virtual void SaveObject(MkDataNode& node) const

// �Ļ� class body ������. SetObjectTemplate(), LoadObject(), SaveObject()�� ���� �����ؾ� ��
#define MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(type) \
	const MkHashStr& type::SceneClassKey(void) { const static MkHashStr& key = L#type; return key; } \
	const MkHashStr& type::GetSceneClassKey(void) const { return type::SceneClassKey(); }


//------------------------------------------------------------------------------------------------//
// PVC
//------------------------------------------------------------------------------------------------//

class MkSceneObject
{
public:

	//------------------------------------------------------------------------------------------------//

	// data�κ��� ���� �� scene node�� ��ȯ
	// ���� �� ������ node�� �״�� ��� ������ ���
	static MkSceneNode* Build(const MkDataNode& node);

	//------------------------------------------------------------------------------------------------//

	// data�κ��� �ڽ� �� ���� scene�� ����. scene node �迭�� ����
	// ��� �ִ� scene node �Ļ� ��ü���� ���θ� ȣ���� ä��� ����̹Ƿ� custum class�� root�� ��� �� �� ����
	virtual void Load(const MkDataNode& node) = NULL;

	// �ڽ� �� ���� scene�� ���. scene node �迭�� ����
	virtual void Save(MkDataNode& node) const = NULL;

	//------------------------------------------------------------------------------------------------//
	
	// class key
	virtual const MkHashStr& GetSceneClassKey(void) const = NULL;

	// object�� input & output
	// ȣ�� �� �̹� �´� template�� ����Ǿ� ����
	// ���� ���� panel/node���� ����Ǿ� ���� ���� ����
	virtual void LoadObject(const MkDataNode& node) = NULL;
	virtual void SaveObject(MkDataNode& node) const = NULL;

	// object�� loading ���� event
	// LoadObject() �� ���� panel/node�� ��� ����Ǿ� �ִ� ����
	virtual void LoadComplete(const MkDataNode& node) {}

	virtual ~MkSceneObject() {}

	// scene ������ ���� �ʱ�ȭ
	static void InitSceneObject(void);

	// class key�� scene node �迭 ���ռ� ��ȯ
	static bool IsNodeClassKey(const MkHashStr& classKey);

	// class key�� �´� scene node�� ������ ��ȯ
	static MkSceneNode* CreateNodeInstance(const MkHashStr& classKey, const MkHashStr& name);

protected:

	// class hierarchy
	static MkTypeHierarchy<ePA_SceneNodeType> SceneNodeTypeHierarchy;
};
