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

// 파생 class header 선언부
#define MKDEF_DECLARE_SCENE_OBJECT_HEADER \
	static const MkHashStr& SceneClassKey(void); \
	virtual const MkHashStr& GetSceneClassKey(void) const; \
	static void SetObjectTemplate(MkDataNode& node); \
	virtual void LoadObject(const MkDataNode& node); \
	virtual void SaveObject(MkDataNode& node) const

// 파생 class body 구현부. SetObjectTemplate(), LoadObject(), SaveObject()는 직접 구현해야 함
#define MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(type) \
	const MkHashStr& type::SceneClassKey(void) { const static MkHashStr& key = L#type; return key; } \
	const MkHashStr& type::GetSceneClassKey(void) const { return type::SceneClassKey(); }


//------------------------------------------------------------------------------------------------//
// scene의 base object 정의(PVC)
//------------------------------------------------------------------------------------------------//

class MkSceneObject
{
public:

	// data로부터 구성 된 scene node를 반환
	// 저장 된 상태의 node를 그대로 얻고 싶으면 사용
	static MkSceneNode* Build(const MkDataNode& node);

	//------------------------------------------------------------------------------------------------//

	// data로부터 자신 및 하위 scene을 구성. scene node 계열만 가능
	// 비어 있는 scene node 파생 객체에서 내부를 호출해 채우는 방식이므로 custum class를 root로 사용 할 수 있음
	virtual void Load(const MkDataNode& node) = NULL;

	// 자신 및 하위 scene을 출력. scene node 계열만 가능
	virtual void Save(MkDataNode& node) const = NULL;

	//------------------------------------------------------------------------------------------------//
	
	// class key
	virtual const MkHashStr& GetSceneClassKey(void) const = NULL;

	// object별 input & output
	// 호출 전 이미 맞는 template은 적용되어 있음
	// 아직 하위 panel/node들은 적용되어 있지 않은 상태
	virtual void LoadObject(const MkDataNode& node) = NULL;
	virtual void SaveObject(MkDataNode& node) const = NULL;

	// object별 loading 종료 event
	// LoadObject() 및 하위 panel/node들 모두 적용되어 있는 상태
	virtual void LoadComplete(const MkDataNode& node) {}

	virtual ~MkSceneObject() {}

	// scene 구성을 위한 초기화
	static void InitSceneObject(void);

	// class key의 scene node 계열 적합성 반환
	static bool IsNodeClassKey(const MkHashStr& classKey);

	// class key에 맞는 scene node를 생성해 반환
	static MkSceneNode* CreateNodeInstance(const MkHashStr& classKey, const MkHashStr& name);

protected:

	// class hierarchy
	static MkTypeHierarchy<ePA_SceneNodeType> SceneNodeTypeHierarchy;
};


//------------------------------------------------------------------------------------------------//
// 실제로 그려지는 object(PVC)
//------------------------------------------------------------------------------------------------//

class MkSceneRenderObject : public MkSceneObject
{
public:

	virtual ePA_SceneObjectType GetObjectType(void) const = NULL;

	inline const MkFloatRect& GetWorldAABR(void) const { return m_AABR; }

	// batch rendering의 기준
	// (NOTE) 상속받은 class는 각각 독자적인 MaterialKey 형태를 가지고 있어야 함(중복 발생 가능성이 있으면 안됨)
	// std::map의 key가 될 수 있는 최소 조건(operator <) 만족
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

	// 실제로 그려지는지 여부 반환
	virtual bool __CheckDrawable(void) const { return false; }

	// __CheckDrawable() + 카메라 영역 체크
	inline bool __CheckDrawableAndInside(const MkFloatRect& cameraAABR) const { return (__CheckDrawable() && m_AABR.CheckIntersection(cameraAABR)); }

	// 전용 draw step이 존재 할 경우 수행
	virtual void __ExcuteCustomDrawStep(void) {}

	// draw시 object에 맞는 vertex block 수 반환. 상수일 경우 생략 가능
	virtual unsigned int __GetVertexDataBlockSize(void) const { return 0; }

	// 실제 그려질 vertex data 생성
	virtual void __FillVertexData(MkByteArray& buffer) const {}

	// render state 반영
	virtual void __ApplyRenderState(void) const {}

	// shader effect 적용여부 반환
	virtual bool __IsShaderEffectApplied(void) const { return false; }

	// shader effect 적용해 그리기
	virtual void __DrawWithShaderEffect(LPDIRECT3DDEVICE9 device) const {}

	virtual ~MkSceneRenderObject() {}

protected:

	MkFloatRect m_AABR; // axis-aligned minimum bounding rect
	MaterialKey m_MaterialKey;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSceneRenderObject::MaterialKey)

//------------------------------------------------------------------------------------------------//
