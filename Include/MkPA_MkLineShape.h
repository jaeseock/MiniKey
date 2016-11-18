#pragma once


//------------------------------------------------------------------------------------------------//
// ��� MkLineShape�� ���� ������ �����ϹǷ� ���� MaterialKey(main key:0, sub key:ePA_SOT_LineShape)�� ���
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

	// ����&�б� ���� ���� ����
	// (NOTE) �� �뵵�� �������̹Ƿ� default�� false
	void SetRecordable(bool enable);
	bool GetRecordable(void) const;

	// visible. default�� true
	void SetVisible(bool visible);
	bool GetVisible(void) const;

	//------------------------------------------------------------------------------------------------//
	// add segment or line
	//------------------------------------------------------------------------------------------------//

	// color
	inline void SetColor(const MkColor& color) { m_Color = color; }
	inline const MkColor& GetColor(void) const { return m_Color; }

	// segment ũ�� ����
	void ReserveSegments(unsigned int size);

	// segment �ϳ��� ����
	// (NOTE) ��ü���� reserve�� ���� �����Ƿ� �ټ� ȣ�� �� ��� ReserveSegments() �ʿ�
	unsigned int AddLine(const MkFloat2& begin, const MkFloat2& end);

	// vertices�� �̾��� line�� ����
	// (NOTE) ��ü���� reserve�� ���� �����Ƿ� �ټ� ȣ�� �� ��� ReserveSegments() �ʿ�
	unsigned int AddLine(const MkArray<MkFloat2>& vertices);

	// ��ϵ� segment �� ��ȯ
	inline unsigned int GetSegmentCount(void) const { return (m_Indices.GetSize() / 2); }

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	inline void SetLocalDepth(float depth) { m_LocalDepth = depth; }
	inline float GetLocalDepth(void) const { return m_LocalDepth; }

	inline float GetWorldDepth(void) const { return m_WorldDepth; }

	//------------------------------------------------------------------------------------------------//

	inline MkSceneNode* GetParentNode(void) const { return m_ParentNode; }

	// ����
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
		// ���� ���� ����
		eRecordable = 0,

		// ��� ����
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
