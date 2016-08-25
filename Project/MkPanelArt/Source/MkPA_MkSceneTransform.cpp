
#include "MkCore_MkAngleOp.h"
#include "MkCore_MkDataNode.h"
#include "MkPA_MkSceneTransform.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr MkSceneTransform::ObjKey_LocalPosition(L"LocalPosition");
const MkHashStr MkSceneTransform::ObjKey_LocalDepth(L"LocalDepth");
const MkHashStr MkSceneTransform::ObjKey_LocalRotation(L"LocalRotation");
const MkHashStr MkSceneTransform::ObjKey_LocalScale(L"LocalScale");
const MkHashStr MkSceneTransform::ObjKey_LocalAlpha(L"LocalAlpha");

#define MKDEF_CW_ROTATE_X(sinR, cosR, LP, PS, PP) ((cosR * LP.x + sinR * LP.y) * PS + PP.x)
#define MKDEF_CW_ROTATE_Y(sinR, cosR, LP, PS, PP) ((-sinR * LP.x + cosR * LP.y) * PS + PP.y)


void MkSceneTransform::SetLocalRotation(float rotation)
{
	m_LocalRotation = MkAngleOp::Unitize2PI(rotation);
}

void MkSceneTransform::Update(const MkSceneTransform* parentTransform)
{
	if (parentTransform == 0)
	{
		m_WorldPosition = m_LocalPosition;
		m_WorldDepth = m_LocalDepth;
		m_WorldRotation = m_LocalRotation;
		m_WorldScale = m_LocalScale;
		m_WorldAlpha = m_LocalAlpha;
	}
	else
	{
		// position : LP * PR * PS + PP
		float PR = parentTransform->GetWorldRotation();
		float PS = parentTransform->GetWorldScale();
		const MkFloat2& PP = parentTransform->GetWorldPosition();

		float cosR = (PR == 0.f) ? 1.f : MkAngleOp::Cos(PR);
		float sinR = (PR == 0.f) ? 0.f : MkAngleOp::Sin(PR);

		m_WorldPosition.x = MKDEF_CW_ROTATE_X(sinR, cosR, m_LocalPosition, PS, PP);
		m_WorldPosition.y = MKDEF_CW_ROTATE_Y(sinR, cosR, m_LocalPosition, PS, PP);

		// depth : LD + PD
		m_WorldDepth = m_LocalDepth + parentTransform->GetWorldDepth();

		// rotation : LR + PR
		m_WorldRotation = MkAngleOp::Unitize2PI(m_LocalRotation + parentTransform->GetWorldRotation());

		// scale :LS * PS
		m_WorldScale = m_LocalScale * parentTransform->GetWorldScale();

		// alpha :LA * PA
		m_WorldAlpha = m_LocalAlpha * parentTransform->GetWorldAlpha();
	}
}

void MkSceneTransform::ClearLocalTransform(void)
{
	m_LocalPosition.Clear();
	m_LocalDepth = 0.f;
	m_LocalRotation = 0.f;
	m_LocalScale = 1.f;
	m_LocalAlpha = 1.f;
}

void MkSceneTransform::GetWorldVertices(const MkArray<MkFloat2>& localVertices, MkArray<MkFloat2>& worldVertices) const
{
	if (!localVertices.Empty())
	{
		worldVertices.Fill(localVertices.GetSize());

		float cosR = (m_WorldRotation == 0.f) ? 1.f : MkAngleOp::Cos(m_WorldRotation);
		float sinR = (m_WorldRotation == 0.f) ? 0.f : MkAngleOp::Sin(m_WorldRotation);

		MK_INDEXING_LOOP(localVertices, i)
		{
			const MkFloat2& localVertex = localVertices[i];
			MkFloat2& worldVertex = worldVertices[i];

			worldVertex.x = MKDEF_CW_ROTATE_X(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
			worldVertex.y = MKDEF_CW_ROTATE_Y(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
		}
	}
}

void MkSceneTransform::GetWorldVertices(const MkFloatRect& rect, MkFloat2 (&vertices)[MkFloatRect::eMaxPointName]) const
{
	float cosR = (m_WorldRotation == 0.f) ? 1.f : MkAngleOp::Cos(m_WorldRotation);
	float sinR = (m_WorldRotation == 0.f) ? 0.f : MkAngleOp::Sin(m_WorldRotation);

	MkFloat2 localVertex = rect.GetAbsolutePosition(MkFloatRect::eLeftTop, false);
	vertices[MkFloatRect::eLeftTop].x = MKDEF_CW_ROTATE_X(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
	vertices[MkFloatRect::eLeftTop].y = MKDEF_CW_ROTATE_Y(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);

	localVertex = rect.GetAbsolutePosition(MkFloatRect::eRightTop, false);
	vertices[MkFloatRect::eRightTop].x = MKDEF_CW_ROTATE_X(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
	vertices[MkFloatRect::eRightTop].y = MKDEF_CW_ROTATE_Y(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);

	localVertex = rect.GetAbsolutePosition(MkFloatRect::eLeftBottom, false);
	vertices[MkFloatRect::eLeftBottom].x = MKDEF_CW_ROTATE_X(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
	vertices[MkFloatRect::eLeftBottom].y = MKDEF_CW_ROTATE_Y(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);

	localVertex = rect.GetAbsolutePosition(MkFloatRect::eRightBottom, false);
	vertices[MkFloatRect::eRightBottom].x = MKDEF_CW_ROTATE_X(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
	vertices[MkFloatRect::eRightBottom].y = MKDEF_CW_ROTATE_Y(sinR, cosR, localVertex, m_WorldScale, m_WorldPosition);
}

void MkSceneTransform::SetObjectTemplate(MkDataNode& node)
{
	MkSceneTransform transform;
	node.CreateUnitEx(ObjKey_LocalPosition, transform.GetLocalPosition());
	node.CreateUnit(ObjKey_LocalDepth, transform.GetLocalDepth());
	node.CreateUnit(ObjKey_LocalRotation, transform.GetLocalRotation());
	node.CreateUnit(ObjKey_LocalScale, transform.GetLocalScale());
	node.CreateUnit(ObjKey_LocalAlpha, transform.GetLocalAlpha());
}

void MkSceneTransform::Load(const MkDataNode& node)
{
	ClearLocalTransform();

	MkFloat2 f2Buf;
	if (node.GetDataEx(ObjKey_LocalPosition, f2Buf, 0))
	{
		SetLocalPosition(f2Buf);
	}

	float fBuf;
	if (node.GetData(ObjKey_LocalDepth, fBuf, 0))
	{
		SetLocalDepth(fBuf);
	}

	if (node.GetData(ObjKey_LocalRotation, fBuf, 0))
	{
		SetLocalRotation(fBuf);
	}

	if (node.GetData(ObjKey_LocalScale, fBuf, 0))
	{
		SetLocalScale(fBuf);
	}

	if (node.GetData(ObjKey_LocalAlpha, fBuf, 0))
	{
		SetLocalAlpha(fBuf);
	}
}

void MkSceneTransform::Save(MkDataNode& node) const
{
	node.SetDataEx(ObjKey_LocalPosition, m_LocalPosition, 0);
	node.SetData(ObjKey_LocalDepth, m_LocalDepth, 0);
	node.SetData(ObjKey_LocalRotation, m_LocalRotation, 0);
	node.SetData(ObjKey_LocalScale, m_LocalScale, 0);
	node.SetData(ObjKey_LocalAlpha, m_LocalAlpha, 0);
}

MkSceneTransform::MkSceneTransform()
{
	ClearLocalTransform();

	m_WorldPosition.Clear();
	m_WorldDepth = 0.f;
	m_WorldRotation = 0.f;
	m_WorldScale = 1.f;
	m_WorldAlpha = 1.f;
}

//------------------------------------------------------------------------------------------------//