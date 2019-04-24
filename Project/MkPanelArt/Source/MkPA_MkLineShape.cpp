
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkSceneTransform.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkLineShape.h"


const MkHashStr MkLineShape::ObjKey_Attribute(L"Attribute");
const MkHashStr MkLineShape::ObjKey_LocalVertices(L"LVertices");
const MkHashStr MkLineShape::ObjKey_LocalDepth(L"LDepth");
const MkHashStr MkLineShape::ObjKey_Indice(L"Indice");
const MkHashStr MkLineShape::ObjKey_Color(L"Color");

//------------------------------------------------------------------------------------------------//

void MkLineShape::SetRecordable(bool enable)
{
	m_Attribute.Assign(eRecordable, enable);
}

bool MkLineShape::GetRecordable(void) const
{
	return m_Attribute[eRecordable];
}

void MkLineShape::SetVisible(bool visible)
{
	m_Attribute.Assign(eVisible, visible);
}

bool MkLineShape::GetVisible(void) const
{
	return m_Attribute[eVisible];
}

void MkLineShape::ReserveSegments(unsigned int size)
{
	size *= 2;
	m_LocalVertices.Reserve(size);
	m_Indices.Reserve(size);
}

unsigned int MkLineShape::AddLine(const MkFloat2& begin, const MkFloat2& end)
{
	bool ok = (begin != end);
	if (ok)
	{
		unsigned int index = m_LocalVertices.GetSize();

		m_LocalVertices.PushBack(begin);
		m_Indices.PushBack(index);

		m_LocalVertices.PushBack(end);
		m_Indices.PushBack(index + 1);
	}
	return (ok) ? 1 : 0;
}

unsigned int MkLineShape::AddLine(const MkArray<MkFloat2>& vertices)
{
	unsigned int count = 0;
	unsigned int vSize = vertices.GetSize();

	if (vSize > 1) // 최소 두개는 존재해야 가능
	{
		unsigned int index = m_LocalVertices.GetSize();

		if (vertices[0] != vertices[1])
		{
			m_LocalVertices.PushBack(vertices[0]);
		}

		for (unsigned int i=1; i<vSize; ++i)
		{
			const MkFloat2& begin = vertices[i - 1];
			const MkFloat2& end = vertices[i];
			if (begin != end)
			{
				m_LocalVertices.PushBack(vertices[i]); // current

				m_Indices.PushBack(index); // last
				++index;
				m_Indices.PushBack(index); // current

				++count;
			}
		}
	}
	return count;
}

void MkLineShape::Clear(void)
{
	m_LocalVertices.Clear();
	m_WorldVertices.Clear();
	m_WorldDepth = 0.f;
	m_AABR.Clear();

	m_Indices.Clear();
}

bool MkLineShape::__CheckDrawable(void) const
{
	return
		(GetVisible() && // 보이기 활성화 중이고
		(!m_AABR.size.IsZero())); // 유효한 크기가 존재하면 true
}

void MkLineShape::__FillVertexData(MkByteArray& buffer) const
{
	// color
	D3DCOLOR color = static_cast<D3DCOLOR>(m_Color.ConvertToD3DCOLOR());

	// build data
	MkArray<SegmentData> sds(__GetVertexDataBlockSize());

	unsigned int cnt = m_Indices.GetSize();
	for (unsigned int i=0; i<cnt; i+=2)
	{
		SegmentData& sd = sds.PushBack();

		const MkFloat2& begin = m_WorldVertices[m_Indices[i]];
		const MkFloat2& end = m_WorldVertices[m_Indices[i + 1]];

		sd.begin.position.x = begin.x;
		sd.begin.position.y = begin.y;
		sd.begin.position.z = m_WorldDepth;
		sd.begin.color = color;

		sd.end.position.x = end.x;
		sd.end.position.y = end.y;
		sd.end.position.z = m_WorldDepth;
		sd.end.color = color;
	}

	MkByteArrayHelper<SegmentData>::PushBack(buffer, sds);
}

void MkLineShape::__ApplyRenderState(void) const
{
	MK_RENDER_STATE.UpdateFVF(MKDEF_LINESHAPE_FVF);
	MK_RENDER_STATE.UpdateBlendOp(FALSE, 0, 0, 0, 0, 0);
	MK_RENDER_STATE.UpdateBaseTexture(0, NULL, 0, D3DTEXF_NONE, 0, D3DTEXF_NONE, D3DTADDRESS_WRAP, 0);
}

void MkLineShape::Load(const MkDataNode& node)
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object는 독자적인 Load를 할 수 없음") {}
}

void MkLineShape::Save(MkDataNode& node) const
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object는 독자적인 Save를 할 수 없음") {}
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkLineShape);

void MkLineShape::SetObjectTemplate(MkDataNode& node)
{
	// attribute
	MkBitField32 attr;
	attr.Assign(eRecordable, false);
	attr.Assign(eVisible, true);
	node.CreateUnit(ObjKey_Attribute, attr.m_Field);

	// local vertices
	//MkArray<MkFloat2> fEmpty;
	//node.CreateUnitEx(ObjKey_LocalVertices, fEmpty);

	// local depth
	node.CreateUnit(ObjKey_LocalDepth, 0.f);

	// indice
	//MkArray<unsigned int> uiEmpty;
	//node.CreateUnit(ObjKey_Indice, uiEmpty);

	// color
	node.CreateUnit(ObjKey_Color, MkColor::White.ConvertToD3DCOLOR());
}

void MkLineShape::LoadObject(const MkDataNode& node)
{
	// attribute
	node.GetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// local vertices
	node.GetDataEx(ObjKey_LocalVertices, m_LocalVertices);

	// local depth
	node.GetData(ObjKey_LocalDepth, m_LocalDepth, 0);

	// indice
	node.GetData(ObjKey_Indice, m_Indices);

	// color
	unsigned int color;
	if (node.GetData(ObjKey_Color, color, 0))
	{
		m_Color.SetByD3DCOLOR(color);
	}
}

void MkLineShape::SaveObject(MkDataNode& node) const
{
	// attribute
	node.SetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// local vertices
	node.SetDataEx(ObjKey_LocalVertices, m_LocalVertices);

	// local depth
	node.SetData(ObjKey_LocalDepth, m_LocalDepth, 0);

	// indice
	node.SetData(ObjKey_Indice, m_Indices);

	// color
	node.SetData(ObjKey_Color, m_Color.ConvertToD3DCOLOR(), 0);
}

void MkLineShape::__Update(const MkSceneTransform* parentTransform)
{
	m_WorldVertices.Clear();
	m_WorldDepth = m_LocalDepth;
	m_AABR.Clear();

	if (!m_LocalVertices.Empty())
	{
		// transform
		if (parentTransform == NULL) // 부모 노드가 없음
		{
			m_WorldVertices = m_LocalVertices;
		}
		else // 자체 transform이 없으므로 부모 노드의 것을 그대로 사용
		{
			parentTransform->GetWorldVertices(m_LocalVertices, m_WorldVertices);
			m_WorldDepth += parentTransform->GetWorldDepth();
		}

		// aabb
		MkFloat2 minPt(m_WorldVertices[0]);
		MkFloat2 maxPt(m_WorldVertices[0]);

		MK_INDEXING_LOOP(m_WorldVertices, i)
		{
			const MkFloat2& vertex = m_WorldVertices[i];
			minPt.CompareAndKeepMin(vertex);
			maxPt.CompareAndKeepMax(vertex);
		}

		m_AABR.position = minPt;
		m_AABR.size = maxPt - minPt;
	}
}

MkLineShape::MkLineShape(void) : MkSceneRenderObject()
{
	m_ParentNode = NULL;

	m_MaterialKey.m_SubKey = static_cast<DWORD>(ePA_SOT_LineShape); // MkLineShape only

	SetLocalDepth(0.f);

	m_Attribute.Clear();
	SetRecordable(false);
	SetVisible(true);

	SetColor(MkColor::White);
}

//------------------------------------------------------------------------------------------------//