#pragma once


//------------------------------------------------------------------------------------------------//
// 모든 MkLineShape는 재질 구성이 동일하므로 같은 MaterialKey(main key:0, sub key:ePA_SOT_LineShape)를 사용
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBitField32.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkRect.h"
#include "MkPA_MkD3DDefinition.h"
#include "MkPA_MkSceneObject.h"


class MkSceneTransform;
class MkSceneNode;

class MkLineShape : public MkSceneRenderObject
{
public:

	typedef struct _PointData
	{
		D3DXVECTOR3 position;
		D3DCOLOR color;
	}
	PointData;

	typedef struct _SegmentData
	{
		PointData begin;
		PointData end;
	}
	SegmentData;

public:

	virtual ePA_SceneObjectType GetObjectType(void) const { return ePA_SOT_LineShape; }

	// 저장&읽기 가능 여부 설정
	// (NOTE) 주 용도는 디버깅용이므로 default는 false
	void SetRecordable(bool enable);
	bool GetRecordable(void) const;

	// visible. default는 true
	void SetVisible(bool visible);
	bool GetVisible(void) const;

	//------------------------------------------------------------------------------------------------//
	// add segment or line
	//------------------------------------------------------------------------------------------------//

	// color
	inline void SetColor(const MkColor& color) { m_Color = color; }
	inline const MkColor& GetColor(void) const { return m_Color; }

	// segment 크기 예약
	void ReserveSegments(unsigned int size);

	// segment 하나를 더함
	// (NOTE) 자체적인 reserve를 하지 않으므로 다수 호출 될 경우 ReserveSegments() 필요
	unsigned int AddLine(const MkFloat2& begin, const MkFloat2& end);

	// vertices로 이어진 line을 더함
	// (NOTE) 자체적인 reserve를 하지 않으므로 다수 호출 될 경우 ReserveSegments() 필요
	unsigned int AddLine(const MkArray<MkFloat2>& vertices);

	// 등록된 segment 수 반환
	inline unsigned int GetSegmentCount(void) const { return (m_Indices.GetSize() / 2); }

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	inline void SetLocalDepth(float depth) { m_LocalDepth = depth; }
	inline float GetLocalDepth(void) const { return m_LocalDepth; }

	inline float GetWorldDepth(void) const { return m_WorldDepth; }

	//------------------------------------------------------------------------------------------------//

	inline MkSceneNode* GetParentNode(void) const { return m_ParentNode; }

	// 해제
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneRenderObject
	//------------------------------------------------------------------------------------------------//

	virtual bool __CheckDrawable(void) const;
	virtual unsigned int __GetVertexDataBlockSize(void) const { return (m_Indices.GetSize() / 2); }
	virtual void __FillVertexData(MkByteArray& buffer) const;
	virtual void __ApplyRenderState(void) const;
	
	virtual void Load(const MkDataNode& node);
	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

public:

	//------------------------------------------------------------------------------------------------//

	inline void __SetParentNode(MkSceneNode* parentNode) { m_ParentNode = parentNode; }

	void __Update(const MkSceneTransform* parentTransform);

	MkLineShape(void);
	virtual ~MkLineShape() { Clear(); }

protected:

	enum eAttribute
	{
		// 저장 가능 여부
		eRecordable = 0,

		// 출력 여부
		eVisible
	};

protected:

	// parent
	MkSceneNode* m_ParentNode;

	// flag
	MkBitField32 m_Attribute;

	// transform
	MkArray<MkFloat2> m_LocalVertices;
	float m_LocalDepth;

	MkArray<MkFloat2> m_WorldVertices;
	float m_WorldDepth;
	
	MkArray<unsigned int> m_Indices;
	MkColor m_Color;

public:

	static const MkHashStr ObjKey_Attribute;
	static const MkHashStr ObjKey_LocalVertices;
	static const MkHashStr ObjKey_LocalDepth;
	static const MkHashStr ObjKey_Indice;
	static const MkHashStr ObjKey_Color;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkLineShape::PointData)
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkLineShape::SegmentData)
