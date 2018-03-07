
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkDrawTextNodeStep.h"
#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkPanel.h"


const MkHashStr MkPanel::ObjKey_Attribute(L"Attribute");
const MkHashStr MkPanel::ObjKey_PanelSize(L"PanelSize");
const MkHashStr MkPanel::ObjKey_PixelScrollPosition(L"PScrollPos");
const MkHashStr MkPanel::ObjKey_ImagePath(L"ImagePath");
const MkHashStr MkPanel::ObjKey_SubsetOrSequenceName(L"SOSName");
const MkHashStr MkPanel::ObjKey_SequenceTimeOffset(L"STimeOffset");
const MkHashStr MkPanel::ObjKey_TextNodeName(L"TextNodeName");
const MkHashStr MkPanel::ObjKey_TextNodeData(L"TextNodeData");
const MkHashStr MkPanel::ObjKey_TextNodeWidthRestriction(L"TextNodeWR");

//------------------------------------------------------------------------------------------------//

void MkPanel::SetSmallerSourceOp(eSmallerSourceOp op)
{
	m_Attribute.SetValue(static_cast<unsigned int>(op), eSmallerSourceOpPosition, eSmallerSourceOpBandwidth);
}

MkPanel::eSmallerSourceOp MkPanel::GetSmallerSourceOp(void) const
{
	return static_cast<eSmallerSourceOp>(m_Attribute.GetValue(eSmallerSourceOpPosition, eSmallerSourceOpBandwidth));
}

void MkPanel::SetBiggerSourceOp(eBiggerSourceOp op)
{
	m_Attribute.SetValue(static_cast<unsigned int>(op), eBiggerSourceOpPosition, eBiggerSourceOpBandwidth);
}

MkPanel::eBiggerSourceOp MkPanel::GetBiggerSourceOp(void) const
{
	return static_cast<eBiggerSourceOp>(m_Attribute.GetValue(eBiggerSourceOpPosition, eBiggerSourceOpBandwidth));
}

void MkPanel::SetPanelSize(const MkFloat2& size)
{
	m_PanelSize = size;

	if (m_TargetTextNodePtr != NULL)
	{
		m_TargetTextNodePtr->SetWidthRestriction(static_cast<int>(m_PanelSize.x));
	}
}

void MkPanel::SetHorizontalReflection(bool enable)
{
	m_Attribute.Assign(eHorizontalReflection, enable);
}

bool MkPanel::GetHorizontalReflection(void) const
{
	return m_Attribute[eHorizontalReflection];
}

void MkPanel::SetVerticalReflection(bool enable)
{
	m_Attribute.Assign(eVerticalReflection, enable);
}

bool MkPanel::GetVerticalReflection(void) const
{
	return m_Attribute[eVerticalReflection];
}

void MkPanel::SetVisible(bool visible)
{
	m_Attribute.Assign(eVisible, visible);
}

bool MkPanel::GetVisible(void) const
{
	return m_Attribute[eVisible];
}

bool MkPanel::SetTexture(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName, double timeOffset)
{
	Clear();

	if (texture == NULL)
		return false;

	m_Texture = const_cast<MkBaseTexture*>(texture); // ref++
	m_MaterialKey.m_MainKey = MK_PTR_TO_ID64(m_Texture.GetPtr());
	return SetSubsetOrSequenceName(subsetOrSequenceName, timeOffset);
}

bool MkPanel::SetTexture(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName, double timeOffset)
{
	return SetTexture(imagePath.Empty() ? NULL : MK_BITMAP_POOL.GetBitmapTexture(imagePath), subsetOrSequenceName, timeOffset);
}

bool MkPanel::SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName, double timeOffset)
{
	bool ok = ((m_Texture != NULL) && m_Texture->GetImageInfo().IsValidName(subsetOrSequenceName));
	if (ok)
	{
		m_SubsetOrSequenceName = subsetOrSequenceName;
		m_SequenceTimeOffset = timeOffset;

		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);
		m_SequenceStartTime = ts.fullTime;

		const MkImageInfo::Subset* ssPtr = m_Texture->GetImageInfo().GetCurrentSubsetPtr(m_SubsetOrSequenceName, m_SequenceTimeOffset);
		m_TextureSize = ssPtr->rectSize;

		if (((m_PanelSize.x > m_TextureSize.x) && (GetSmallerSourceOp() == eReducePanel)) ||
			((m_PanelSize.x < m_TextureSize.x) && (GetBiggerSourceOp() == eExpandPanel)))
		{
			m_PanelSize.x = m_TextureSize.x;
		}
		if (((m_PanelSize.y > m_TextureSize.y) && (GetSmallerSourceOp() == eReducePanel)) ||
			((m_PanelSize.y < m_TextureSize.y) && (GetBiggerSourceOp() == eExpandPanel)))
		{
			m_PanelSize.y = m_TextureSize.y;
		}
	}
	return ok;
}

unsigned int MkPanel::GetAllSubsets(MkArray<MkHashStr>& keyList) const
{
	return (m_Texture == NULL) ? 0 : m_Texture->GetImageInfo().GetAllSubsets(keyList);
}

unsigned int MkPanel::GetAllSequences(MkArray<MkHashStr>& keyList) const
{
	return (m_Texture == NULL) ? 0 : m_Texture->GetImageInfo().GetAllSequences(keyList);
}

void MkPanel::SetTextNode(const MkTextNode& source, bool restrictToPanelWidth)
{
	Clear();

	do
	{
		m_TargetTextNodePtr = new MkTextNode;
		MK_CHECK(m_TargetTextNodePtr != NULL, L"MkTextNode alloc ����")
			break;

		*m_TargetTextNodePtr = source; // deep copy

		if (restrictToPanelWidth)
		{
			m_TargetTextNodePtr->SetWidthRestriction(static_cast<int>(m_PanelSize.x));
		}
		m_TargetTextNodePtr->Build();
		
		MkDrawTextNodeStep* drawStep = new MkDrawTextNodeStep;
		MK_CHECK(drawStep != NULL, L"MkDrawTextNodeStep alloc ����")
			break;

		m_DrawStep = drawStep;

		// setp �ʱ�ȭ �� ����
		_UpdateTextNodeTexture(drawStep);
		return;
	}
	while (false);

	MK_DELETE(m_TargetTextNodePtr);
	MK_DELETE(m_DrawStep);
}

void MkPanel::SetTextNode(const MkArray<MkHashStr>& name, bool restrictToPanelWidth)
{
	m_TargetTextNodeName = name;
	const MkTextNode& textNode = MK_STATIC_RES.GetTextNode(m_TargetTextNodeName);
	SetTextNode(textNode, restrictToPanelWidth);
}

void MkPanel::BuildAndUpdateTextCache(void)
{
	if ((m_TargetTextNodePtr != NULL) && (m_DrawStep != NULL))
	{
		MkDrawTextNodeStep* drawStep = dynamic_cast<MkDrawTextNodeStep*>(m_DrawStep);
		if (drawStep != NULL)
		{
			m_TargetTextNodePtr->Build();
			
			// draw step�� ���������� texture�� Build()�� ũ�Ⱑ ���� �� �����Ƿ� �ı� �� �����
			m_Texture = NULL; // ref-
			m_MaterialKey.m_MainKey = 0;

			// setp �ʱ�ȭ �� ����
			_UpdateTextNodeTexture(drawStep);
		}
	}
}

void MkPanel::SetMaskingNode(const MkSceneNode* sceneNode)
{
	MK_CHECK((m_PanelSize.x >= 1.f) && (m_PanelSize.y >= 1.f), L"��ȿ�� panel size�� �����Ǿ� �־�� ��")
		return;

	Clear();

	if (sceneNode != NULL)
	{
		do
		{
			MkDrawSceneNodeStep* drawStep = new MkDrawSceneNodeStep;
			MK_CHECK(drawStep != NULL, L"MkDrawSceneNodeStep alloc ����")
				break;

			m_DrawStep = drawStep;
			MK_CHECK(drawStep->SetUp(MkRenderTarget::eTexture, 1, MkInt2(static_cast<int>(m_PanelSize.x), static_cast<int>(m_PanelSize.y))), L"MkDrawSceneNodeStep SetUp ����")
				break;

			m_Texture = drawStep->GetTargetTexture();
			m_MaterialKey.m_MainKey = MK_PTR_TO_ID64(m_Texture.GetPtr());

			drawStep->SetSceneNode(sceneNode);

			m_TextureSize = m_PanelSize;
			return;
		}
		while (false);

		MK_DELETE(m_DrawStep);
	}
}

void MkPanel::Clear(void)
{
	m_PixelScrollPosition.Clear();
	m_Texture = NULL;
	m_MaterialKey.m_MainKey = 0;
	m_SubsetOrSequenceName.Clear();
	m_TextureSize.Clear();
	m_TargetTextNodeName.Clear();
	MK_DELETE(m_TargetTextNodePtr);
	MK_DELETE(m_DrawStep);
}

void MkPanel::__ExcuteCustomDrawStep(void)
{
	if (m_DrawStep != NULL)
	{
		m_DrawStep->Draw();
	}
}

void MkPanel::__FillVertexData(MkByteArray& buffer) const
{
	// xyz, uv
	// 0 --- 1
	// |  /  |
	// 2 --- 3
	// seq : 0, 1, 2, 2, 1, 3

	bool hr = GetHorizontalReflection();
	bool vr = GetVerticalReflection();

	_FillVertexData(MkFloatRect::eLeftTop, hr, vr, buffer); // 0
	_FillVertexData(MkFloatRect::eRightTop, hr, vr, buffer); // 1
	_FillVertexData(MkFloatRect::eLeftBottom, hr, vr, buffer); // 2
	_FillVertexData(MkFloatRect::eLeftBottom, hr, vr, buffer); // 2
	_FillVertexData(MkFloatRect::eRightTop, hr, vr, buffer); // 1
	_FillVertexData(MkFloatRect::eRightBottom, hr, vr, buffer); // 3
}

void MkPanel::__ApplyRenderState(void) const
{
	MK_RENDER_STATE.UpdateFVF(MKDEF_PANEL_FVF);

	if (m_Texture != NULL)
	{
		m_Texture->UpdateRenderState(m_MaterialKey.m_SubKey);
	}
}

void MkPanel::Load(const MkDataNode& node)
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object�� �������� Load�� �� �� ����") {}
}

void MkPanel::Save(MkDataNode& node) const
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object�� �������� Save�� �� �� ����") {}
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkPanel);

void MkPanel::SetObjectTemplate(MkDataNode& node)
{
	// attribute
	MkBitField32 attr;
	attr.Assign(eVisible, true);
	node.CreateUnit(ObjKey_Attribute, attr.m_Field);

	// transform
	MkSceneTransform::SetObjectTemplate(node);

	// panel size
	node.CreateUnitEx(ObjKey_PanelSize, MkFloat2::Zero);
	node.CreateUnitEx(ObjKey_PixelScrollPosition, MkFloat2::Zero);

	// texture image
	node.CreateUnit(ObjKey_ImagePath, MkStr::EMPTY);
	node.CreateUnit(ObjKey_SubsetOrSequenceName, MkStr::EMPTY);
	node.CreateUnit(ObjKey_SequenceTimeOffset, 0.f);

	// text node
	//ObjKey_TextNodeName
	//ObjKey_TextNodeData
	node.CreateUnit(ObjKey_TextNodeWidthRestriction, false);
}

void MkPanel::LoadObject(const MkDataNode& node)
{
	// attribute
	node.GetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// transform
	m_Transform.Load(node);

	// panel size
	node.GetDataEx(ObjKey_PanelSize, m_PanelSize, 0);
	node.GetDataEx(ObjKey_PixelScrollPosition, m_PixelScrollPosition, 0);

	do
	{
		// texture image
		MkStr imagePath;
		node.GetData(ObjKey_ImagePath, imagePath, 0);
		if (!imagePath.Empty())
		{
			MkStr subsetOrSeqName;
			node.GetData(ObjKey_SubsetOrSequenceName, subsetOrSeqName, 0);

			float stOffset = 0.f;
			node.GetData(ObjKey_SequenceTimeOffset, stOffset, 0);

			SetTexture(imagePath, subsetOrSeqName, static_cast<double>(stOffset));
			break;
		}

		// text node
		MkArray<MkHashStr> textNodeName;
		if ((node.GetDataEx(ObjKey_TextNodeName, textNodeName) || node.ChildExist(ObjKey_TextNodeData)))
		{
			bool widthRestriction;
			node.GetData(ObjKey_TextNodeWidthRestriction, widthRestriction, 0);

			if (textNodeName.Empty()) // dynamic
			{
				MkTextNode textNode;
				textNode.SetUp(*node.GetChildNode(ObjKey_TextNodeData));
				SetTextNode(textNode, widthRestriction);
			}
			else // static
			{
				SetTextNode(textNodeName, widthRestriction);
			}
			break;
		}
	}
	while (false);
}

void MkPanel::SaveObject(MkDataNode& node) const
{
	// attribute
	node.SetData(ObjKey_Attribute, m_Attribute.m_Field, 0);

	// transform
	m_Transform.Save(node);

	// panel size
	node.SetDataEx(ObjKey_PanelSize, m_PanelSize, 0);
	node.SetDataEx(ObjKey_PixelScrollPosition, m_PixelScrollPosition, 0);

	if (m_Texture != NULL)
	{
		do
		{
			// texture image
			const MkHashStr& poolKey = m_Texture->GetPoolKey();
			if (!poolKey.Empty())
			{
				node.SetData(ObjKey_ImagePath, poolKey.GetString(), 0);
				node.SetData(ObjKey_SubsetOrSequenceName, m_SubsetOrSequenceName.GetString(), 0);
				node.SetData(ObjKey_SequenceTimeOffset, static_cast<float>(m_SequenceTimeOffset), 0);
				break;
			}

			// text node
			if (m_TargetTextNodePtr != NULL)
			{
				node.SetData(ObjKey_TextNodeWidthRestriction, m_TargetTextNodePtr->GetWidthRestriction() > 0, 0);

				if (m_TargetTextNodeName.Empty()) // dynamic
				{
					MkDataNode* textNode = node.CreateChildNode(ObjKey_TextNodeData);
					if (textNode != NULL)
					{
						m_TargetTextNodePtr->Export(*textNode);
					}
				}
				else // static
				{
					node.CreateUnitEx(ObjKey_TextNodeName, m_TargetTextNodeName);
				}
				break;
			}
		}
		while (false);
	}
}

bool MkPanel::__CheckDrawable(void) const
{
	return
		(GetVisible() && // ���̱� Ȱ��ȭ ���̰�
		(m_Texture != NULL) && // �ؽ��İ� �����ϰ�
		m_PanelSize.IsPositive() && m_AABR.size.IsPositive() && // ��ȿ�� ũ�Ⱑ �����ϰ�
		(m_Transform.GetWorldDepth() >= 0.f) && (m_Transform.GetWorldDepth() <= MKDEF_PA_MAX_WORLD_DEPTH) && // ���̰��� ī�޶� ���� �ȿ� �ְ�
		(m_MaterialKey.m_SubKey > 0)); // ������Ʈ ���İ� 0���� ũ�� true
}

void MkPanel::__Update(const MkSceneTransform* parentTransform, double currTime)
{
	// transform
	m_Transform.Update(parentTransform);

	// alpha
	m_MaterialKey.m_SubKey = static_cast<DWORD>(m_Transform.GetWorldAlpha() * 255.f);

	// world vertex and uv
	if (m_Texture != NULL)
	{
		const MkImageInfo::Subset* ssPtr = m_Texture->GetImageInfo().GetCurrentSubsetPtr(m_SubsetOrSequenceName, currTime - m_SequenceStartTime + m_SequenceTimeOffset);
		if (ssPtr == NULL)
		{
			m_WorldVertice[MkFloatRect::eLeftTop] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eRightTop] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eLeftBottom] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eRightBottom] = m_Transform.GetWorldPosition();
			m_AABR.position = m_Transform.GetWorldPosition();
			m_AABR.size.Clear();
			m_TextureSize.Clear();
		}
		else
		{
			m_TextureSize = ssPtr->rectSize;

			bool copyU = true, copyV = true;
			bool srcSizeChanged = false;

			// �ֹ߼� local rect
			MkFloatRect internalRect;
			
			// x size, u
			if (m_PanelSize.x > m_TextureSize.x) // smaller src
			{
				switch (GetSmallerSourceOp())
				{
				case eReducePanel: // source�� ũ�⿡ �°� panel�� ����� ����
					m_PanelSize.x = m_TextureSize.x;
					break;
				case eExpandSource: // panel ũ�⿡ �°� source�� Ȯ���� ����
					srcSizeChanged = true;
					break;
				case eAttachToLeftTop: // panel�� source ũ�⸦ ��� ����. panel�� left-top�� �������� ���
					internalRect.size.x = m_TextureSize.x - m_PanelSize.x;
					break;
				}
			}
			else if (m_PanelSize.x < m_TextureSize.x) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source�� ũ�⿡ �°� panel�� Ȯ���� ����
					m_PanelSize.x = m_TextureSize.x;
					break;
				case eReduceSource: // panel ũ�⿡ �°� source�� ����� ����
					srcSizeChanged = true;
					break;
				case eCutSource: // source�� panel ũ�⿡ �°� �Ϻθ� �߶� ������
					{
						float uLength = ssPtr->uv[MkFloatRect::eRightTop].x - ssPtr->uv[MkFloatRect::eLeftTop].x;
						m_PixelScrollPosition.x = Clamp<float>(m_PixelScrollPosition.x, 0.f, m_TextureSize.x - m_PanelSize.x);
						float uBegin = uLength * m_PixelScrollPosition.x / m_TextureSize.x;
						float uEnd = uBegin + uLength * m_PanelSize.x / m_TextureSize.x;
						m_UV[MkFloatRect::eLeftTop].x = uBegin;
						m_UV[MkFloatRect::eRightTop].x = uEnd;
						m_UV[MkFloatRect::eLeftBottom].x = uBegin;
						m_UV[MkFloatRect::eRightBottom].x = uEnd;
						copyU = false;
					}
					break;
				}
			}

			// y size, v
			if (m_PanelSize.y > m_TextureSize.y) // smaller src
			{
				switch (GetSmallerSourceOp())
				{
				case eReducePanel: // source�� ũ�⿡ �°� panel�� ����� ����
					m_PanelSize.y = m_TextureSize.y;
					break;
				case eExpandSource: // panel ũ�⿡ �°� source�� Ȯ���� ����
					srcSizeChanged = true;
					break;
				case eAttachToLeftTop: // panel�� source ũ�⸦ ��� ����. panel�� left-top�� �������� ���
					internalRect.position.y = m_PanelSize.y - m_TextureSize.y;
					internalRect.size.y = -internalRect.position.y;
					break;
				}
			}
			else if (m_PanelSize.y < m_TextureSize.y) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source�� ũ�⿡ �°� panel�� Ȯ���� ����
					m_PanelSize.y = m_TextureSize.y;
					break;
				case eReduceSource: // panel ũ�⿡ �°� source�� ����� ����
					srcSizeChanged = true;
					break;
				case eCutSource: // source�� panel ũ�⿡ �°� �Ϻθ� �߶� ������
					{
						float vLength = ssPtr->uv[MkFloatRect::eLeftBottom].y - ssPtr->uv[MkFloatRect::eLeftTop].y;
						m_PixelScrollPosition.y = Clamp<float>(m_PixelScrollPosition.y, 0.f, m_TextureSize.y - m_PanelSize.y);
						float vBegin = vLength * m_PixelScrollPosition.y / m_TextureSize.y;
						float vEnd = vBegin + vLength * m_PanelSize.y / m_TextureSize.y;
						m_UV[MkFloatRect::eLeftTop].y = vBegin;
						m_UV[MkFloatRect::eRightTop].y = vBegin;
						m_UV[MkFloatRect::eLeftBottom].y = vEnd;
						m_UV[MkFloatRect::eRightBottom].y = vEnd;
						copyV = false;
					}
					break;
				}
			}

			// copy untouched uv
			if (copyU && copyV)
			{
				memcpy_s(m_UV, sizeof(MkFloat2) * MkFloatRect::eMaxPointName, ssPtr->uv, sizeof(MkFloat2) * MkFloatRect::eMaxPointName);
			}
			else if (copyU)
			{
				m_UV[MkFloatRect::eLeftTop].x = ssPtr->uv[MkFloatRect::eLeftTop].x;
				m_UV[MkFloatRect::eRightTop].x = ssPtr->uv[MkFloatRect::eRightTop].x;
				m_UV[MkFloatRect::eLeftBottom].x = ssPtr->uv[MkFloatRect::eLeftBottom].x;
				m_UV[MkFloatRect::eRightBottom].x = ssPtr->uv[MkFloatRect::eRightBottom].x;
			}
			else if (copyV)
			{
				m_UV[MkFloatRect::eLeftTop].y = ssPtr->uv[MkFloatRect::eLeftTop].y;
				m_UV[MkFloatRect::eRightTop].y = ssPtr->uv[MkFloatRect::eRightTop].y;
				m_UV[MkFloatRect::eLeftBottom].y = ssPtr->uv[MkFloatRect::eLeftBottom].y;
				m_UV[MkFloatRect::eRightBottom].y = ssPtr->uv[MkFloatRect::eRightBottom].y;
			}

			// vertices
			internalRect.size += m_PanelSize;
			m_Transform.GetWorldVertices(internalRect, m_WorldVertice);

			// filtering mode. �̹��� ������ Ȯ��/��� �� ��� point���ٴ� linear filtering�� ����
			// ��Ģ�����δ� ���� ��� rect�� ���ؾ� ������ �߶� �׸����� ��� uv�κ��� size�� �����ؾߵǴµ� floating ��� Ư���� �߻��ϴ�
			// ������ ���� ������ �߸��� ����� ���� ���ɼ��� ����. ���� �ܼ��ϰ� ���ǽ����� ó��
			m_Texture->SetFilterType
				((srcSizeChanged || (m_Transform.GetWorldRotation() != 0.f) || (m_Transform.GetWorldScale() != 1.f)) ?
				MkBaseTexture::eLinear : MkBaseTexture::ePoint);

			// update AABR
			MkFloat2 minPt(m_WorldVertice[MkFloatRect::eLeftTop]);
			minPt.CompareAndKeepMin(m_WorldVertice[MkFloatRect::eRightTop]);
			minPt.CompareAndKeepMin(m_WorldVertice[MkFloatRect::eLeftBottom]);
			minPt.CompareAndKeepMin(m_WorldVertice[MkFloatRect::eRightBottom]);

			MkFloat2 maxPt(m_WorldVertice[MkFloatRect::eLeftTop]);
			maxPt.CompareAndKeepMax(m_WorldVertice[MkFloatRect::eRightTop]);
			maxPt.CompareAndKeepMax(m_WorldVertice[MkFloatRect::eLeftBottom]);
			maxPt.CompareAndKeepMax(m_WorldVertice[MkFloatRect::eRightBottom]);

			m_AABR.position = minPt;
			m_AABR.size = maxPt - minPt;
		}
	}
}

bool MkPanel::__CheckWorldIntersection(const MkFloat2& worldPoint) const
{
	if (m_Transform.GetWorldRotation() == 0.f) // rotation�� �����Ƿ� (m_WorldVertice == m_AABR)��
		return m_AABR.CheckIntersection(worldPoint);

	// minikey�� ��ǥ�迡���� line vector�� point�� �������� line�� ������ �����ϸ� ���, ������ �����ϸ� ������
	// ���� ��� �� ���ο����� �������� ����� �ٱ��ʿ� �������� ���� ��
	if (_GetCrossProduct(MkFloatRect::eLeftTop, MkFloatRect::eRightTop, worldPoint) > 0.f)
		return false;
	if (_GetCrossProduct(MkFloatRect::eRightTop, MkFloatRect::eRightBottom, worldPoint) > 0.f)
		return false;
	if (_GetCrossProduct(MkFloatRect::eRightBottom, MkFloatRect::eLeftBottom, worldPoint) > 0.f)
		return false;
	if (_GetCrossProduct(MkFloatRect::eLeftBottom, MkFloatRect::eLeftTop, worldPoint) > 0.f)
		return false;
	return true;
}

MkPanel::MkPanel(void) : MkSceneRenderObject()
{
	m_ParentNode = NULL;

	m_MaterialKey.m_SubKey = 0xff; // alpha 1

	SetSmallerSourceOp(eReducePanel);
	SetBiggerSourceOp(eExpandPanel);

	m_SequenceStartTime = 0.;
	m_SequenceTimeOffset = 0.;
	m_MaterialKey.m_MainKey = 0;

	m_Attribute.Clear();
	SetVisible(true);
	SetHorizontalReflection(false);
	SetVerticalReflection(false);

	m_TargetTextNodePtr = NULL;
	m_DrawStep = NULL;
}

void MkPanel::_FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkByteArray& buffer) const
{
	VertexData vd;
	const MkFloat2& wv = m_WorldVertice[pn];
	vd.x = wv.x;
	vd.y = wv.y;
	vd.z = m_Transform.GetWorldDepth();

	const MkFloat2& uv = m_UV[pn];
	vd.u = (hr) ? (1.f - uv.x) : uv.x;
	vd.v = (vr) ? (1.f - uv.y) : uv.y;

	MkByteArrayHelper<VertexData>::PushBack(buffer, vd);
}

float MkPanel::_GetCrossProduct(MkFloatRect::ePointName from, MkFloatRect::ePointName to, const MkFloat2& point) const
{
	MkFloat2 lineVector = m_WorldVertice[to] - m_WorldVertice[from];
	MkFloat2 pointVector = point - m_WorldVertice[from];
	return (lineVector.x * pointVector.y - lineVector.y * pointVector.x);
}

void MkPanel::_UpdateTextNodeTexture(MkDrawTextNodeStep* drawStep)
{
	if ((m_TargetTextNodePtr != NULL) && (drawStep != NULL))
	{
		drawStep->SetUp(m_TargetTextNodePtr);
		m_Texture = drawStep->GetTargetTexture();
		m_MaterialKey.m_MainKey = MK_PTR_TO_ID64(m_Texture.GetPtr());

		// text node size
		const MkInt2& srcSize = m_TargetTextNodePtr->GetWholePixelSize();
		m_TextureSize.x = static_cast<float>(srcSize.x);
		m_TextureSize.y = static_cast<float>(srcSize.y);
	}
}

//------------------------------------------------------------------------------------------------//