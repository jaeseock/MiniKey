#pragma once


//------------------------------------------------------------------------------------------------//
// MkSceneObjectInterface
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"
#include "MkCore_MkRect.h"
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
// scene�� base object ����(PVC)
//------------------------------------------------------------------------------------------------//

class MkSceneObject
{
public:

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


//------------------------------------------------------------------------------------------------//
// ������ �׷����� object(PVC)
//------------------------------------------------------------------------------------------------//

class MkSceneRenderObject : public MkSceneObject
{
public:

	virtual ePA_SceneObjectType GetObjectType(void) const = NULL;

	inline const MkFloatRect& GetWorldAABR(void) const { return m_AABR; }

	// batch rendering�� ����
	// (NOTE) ��ӹ��� class�� ���� �������� MaterialKey ���¸� ������ �־�� ��(�ߺ� �߻� ���ɼ��� ������ �ȵ�)
	// std::map�� key�� �� �� �ִ� �ּ� ����(operator <) ����
	class MaterialKey
	{
	public:
		inline bool operator == (const MaterialKey& key) const { return ((m_MainKey == key.m_MainKey) && (m_SubKey == key.m_SubKey)); }
		inline bool operator < (const MaterialKey& key) const { return ((m_MainKey < key.m_MainKey) ? true : ((m_MainKey == key.m_MainKey) ? (m_SubKey < key.m_SubKey) : false)); }

		MaterialKey()
		{
			m_MainKey = 0;
			m_SubKey = 0;
		}

		ID64 m_MainKey;
		DWORD m_SubKey;
	};

	inline const MaterialKey& __GetMaterialKey(void) const { return m_MaterialKey; }

	// ������ �׷������� ���� ��ȯ
	virtual bool __CheckDrawable(void) const { return false; }

	// __CheckDrawable() + ī�޶� ���� üũ
	inline bool __CheckDrawableAndInside(const MkFloatRect& cameraAABR) const { return (__CheckDrawable() && m_AABR.CheckIntersection(cameraAABR)); }

	// ���� draw step�� ���� �� ��� ����
	virtual void __ExcuteCustomDrawStep(void) {}

	// draw�� object�� �´� vertex block �� ��ȯ. ����� ��� ���� ����
	virtual unsigned int __GetVertexDataBlockSize(void) const { return 0; }

	// ���� �׷��� vertex data ����
	virtual void __FillVertexData(MkByteArray& buffer) const {}

	// render state �ݿ�
	virtual void __ApplyRenderState(void) const {}

	// shader effect ���뿩�� ��ȯ
	virtual bool __IsShaderEffectApplied(void) const { return false; }

	// shader effect ������ �׸���
	virtual void __DrawWithShaderEffect(LPDIRECT3DDEVICE9 device) const {}

	virtual ~MkSceneRenderObject() {}

protected:

	MkFloatRect m_AABR; // axis-aligned minimum bounding rect
	MaterialKey m_MaterialKey;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSceneRenderObject::MaterialKey)

//------------------------------------------------------------------------------------------------//
