
#include "MkCore_MkCheck.h"
//#include "MkCore_MkGlobalFunction.h"
//#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkBitmapPool.h"
//#include "MkS2D_MkFontManager.h"
//#include "MkS2D_MkWindowResourceManager.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkDrawTextNodeStep.h"
#include "MkPA_MkPanel.h"


//const static MkHashStr TEMPLATE_NAME = MKDEF_S2D_BT_SRECT_TEMPLATE_NAME;

// MkVec2
//const static MkHashStr POSITION_KEY = L"Position";

// MkVec2
//const static MkHashStr SIZE_KEY = L"Size";

// float
//const static MkHashStr DEPTH_KEY = L"Depth";

// MkStr
//const static MkHashStr FORCED_FONT_STATE_KEY = L"FontState";

// MkArray<MkStr>
//const static MkHashStr RESOURCE_KEY = L"Resource";

// map : MkStr 3개
// - tag(MAP_TAG)
// - bitmap file path
// - subset name
//const static MkStr MAP_TAG = L"map";

// original deco text : MkStr 2개
// - tag(TEXT_O_TAG)
// - deco text
//const static MkStr TEXT_O_TAG = L"odt";

// scene deco text : MkStr 1 + n개
// - tag(TEXT_S_TAG)
// - MK_WIN_RES_MGR에 등록되어 있는 deco text node name & key
//const static MkStr TEXT_S_TAG = L"sdt";

// unsigned int
//const static MkHashStr ALPHA_KEY = L"Alpha";

// bool(horizontal), bool(vertical)
//const static MkHashStr REFLECTION_KEY = L"Reflection";

// bool
//const static MkHashStr VISIBLE_KEY = L"Visible";

//------------------------------------------------------------------------------------------------//
/*
void MkPanel::Load(const MkDataNode& node)
{
	Clear();

	// position
	MkVec2 position;
	node.GetData(POSITION_KEY, position, 0);
	SetLocalPosition(MkFloat2(position.x, position.y));

	// depth
	float depth = 0.f;
	node.GetData(DEPTH_KEY, depth, 0);
	SetLocalDepth(depth);

	// font state
	MkStr fontState;
	node.GetData(FORCED_FONT_STATE_KEY, fontState, 0);
	m_ForcedFontState = fontState;

	// resource
	MkArray<MkStr> resBuf;
	node.GetData(RESOURCE_KEY, resBuf);
	if (!resBuf.Empty())
	{
		MkStr tag = resBuf[0];
		tag.ToLower();
		if ((tag == MAP_TAG) && (resBuf.GetSize() == 3)) // map
		{
			SetTexture(resBuf[1], resBuf[2]);
		}
		else if ((tag == TEXT_O_TAG) && (resBuf.GetSize() == 2)) // original deco text
		{
			SetDecoString(resBuf[1]);
		}
		else if ((tag == TEXT_S_TAG) && (resBuf.GetSize() >= 2)) // scene deco text
		{
			MkArray<MkHashStr> nodeNameAndKey(resBuf.GetSize() - 1);
			for (unsigned int counter = resBuf.GetSize(), i = 1; i < counter; ++i)
			{
				nodeNameAndKey.PushBack() = resBuf[i];
			}

			SetDecoString(nodeNameAndKey);
		}
	}

	// size
	MkVec2 size;
	node.GetData(SIZE_KEY, size, 0);
	SetLocalSize(MkFloat2(size.x, size.y));

	// alpha
	unsigned int alpha = 255;
	node.GetData(ALPHA_KEY, alpha, 0);
	m_MaterialKey.m_ObjectAlpha = static_cast<DWORD>(alpha);
	
	// horizontal reflection
	bool hr = false;
	node.GetData(REFLECTION_KEY, hr, 0);
	SetHorizontalReflection(hr);

	// vertical reflection
	bool vr = false;
	node.GetData(REFLECTION_KEY, vr, 1);
	SetVerticalReflection(vr);

	// visible
	bool visible = true;
	node.GetData(VISIBLE_KEY, visible, 0);
	SetVisible(visible);
}

void MkPanel::Save(MkDataNode& node) // Load의 역
{
	node.Clear();
	node.ApplyTemplate(TEMPLATE_NAME);

	node.SetData(POSITION_KEY, MkVec2(m_LocalRect.position.x, m_LocalRect.position.y), 0);
	node.SetData(SIZE_KEY, MkVec2(m_LocalRect.size.x, m_LocalRect.size.y), 0);
	node.SetData(DEPTH_KEY, m_LocalDepth, 0);
	node.SetData(FORCED_FONT_STATE_KEY, m_ForcedFontState.GetString(), 0);

	if (m_Texture != NULL)
	{
		MkArray<MkStr> resBuf;

		if (m_SceneDecoTextNodeNameAndKey.Empty())
		{
			if (m_OriginalDecoStr.Empty()) // map
			{
				resBuf.Reserve(3);
				resBuf.PushBack(MAP_TAG);
				resBuf.PushBack(m_Texture->GetPoolKey().GetString());
				resBuf.PushBack(m_SubsetOrSequenceName.GetString());
			}
			else // original deco text
			{
				resBuf.Reserve(2);
				resBuf.PushBack(TEXT_O_TAG);
				resBuf.PushBack(m_OriginalDecoStr);
			}
		}
		else // scene deco text
		{
			resBuf.Reserve(1 + m_SceneDecoTextNodeNameAndKey.GetSize());
			resBuf.PushBack(TEXT_S_TAG);
			MK_INDEXING_LOOP(m_SceneDecoTextNodeNameAndKey, i)
			{
				resBuf.PushBack(m_SceneDecoTextNodeNameAndKey[i].GetString());
			}
		}

		node.SetData(RESOURCE_KEY, resBuf);
	}

	node.SetData(ALPHA_KEY, static_cast<unsigned int>(m_MaterialKey.m_ObjectAlpha), 0);
	node.SetData(REFLECTION_KEY, m_HorizontalReflection, 0);
	node.SetData(REFLECTION_KEY, m_VerticalReflection, 1);
	node.SetData(VISIBLE_KEY, m_Visible, 0);
}

void MkPanel::SetFocedFontState(const MkHashStr& fontState)
{
	if (m_ForcedFontState != fontState)
	{
		m_ForcedFontState = fontState;
		if (!MK_FONT_MGR.CheckAvailableFontState(fontState))
		{
			m_ForcedFontState.Clear();
		}

		RestoreDecoString();
	}
}
*/

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

bool MkPanel::SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetOrSequenceName, double startTime, double initTime)
{
	Clear();

	m_Texture = texture;
	m_MaterialKey.m_TextureID = MK_PTR_TO_ID64(m_Texture.GetPtr());
	return SetSubsetOrSequenceName(subsetOrSequenceName, startTime, initTime);
}

bool MkPanel::SetTexture(const MkPathName& imagePath, const MkHashStr& subsetOrSequenceName, double startTime, double initTime)
{
	MkBaseTexturePtr texture;
	MK_BITMAP_POOL.GetBitmapTexture(imagePath, texture);
	MK_CHECK(texture != NULL, MkStr(imagePath) + L" 이미지 파일 로딩 실패") {}
	return SetTexture(texture, subsetOrSequenceName, startTime, initTime);
}

bool MkPanel::SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName, double startTime, double initTime)
{
	bool ok = ((m_Texture != NULL) && m_Texture->GetImageInfo().IsValidName(subsetOrSequenceName));
	if (ok)
	{
		m_SubsetOrSequenceName = subsetOrSequenceName;
		m_SequenceStartTime = startTime;
		m_SequenceInitTime = initTime;

		const MkImageInfo::Subset* ssPtr = m_Texture->GetImageInfo().GetCurrentSubsetPtr(m_SubsetOrSequenceName, m_SequenceInitTime);
		if (((m_PanelSize.x > ssPtr->rectSize.x) && (GetSmallerSourceOp() == eReducePanel)) ||
			((m_PanelSize.x < ssPtr->rectSize.x) && (GetBiggerSourceOp() == eExpandPanel)))
		{
			m_PanelSize.x = ssPtr->rectSize.x;
		}
		if (((m_PanelSize.y > ssPtr->rectSize.y) && (GetSmallerSourceOp() == eReducePanel)) ||
			((m_PanelSize.y < ssPtr->rectSize.y) && (GetBiggerSourceOp() == eExpandPanel)))
		{
			m_PanelSize.y = ssPtr->rectSize.y;
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
		MK_CHECK(m_TargetTextNodePtr != NULL, L"MkTextNode alloc 실패")
			break;

		*m_TargetTextNodePtr = source; // deep copy

		if (restrictToPanelWidth)
		{
			m_TargetTextNodePtr->SetWidthRestriction(static_cast<int>(m_PanelSize.x));
		}
		m_TargetTextNodePtr->Build();

		MkDrawTextNodeStep* drawStep = new MkDrawTextNodeStep;
		MK_CHECK(drawStep != NULL, L"MkDrawTextNodeStep alloc 실패")
			break;

		drawStep->SetUp(m_TargetTextNodePtr);
		drawStep->GetTargetTexture(0, m_Texture);
		m_MaterialKey.m_TextureID = MK_PTR_TO_ID64(m_Texture.GetPtr());

		m_DrawStep = drawStep;
		return;
	}
	while (false);

	MK_DELETE(m_TargetTextNodePtr);
	MK_DELETE(m_DrawStep);
}

bool MkPanel::SetTextNode(const MkHashStr& name, bool restrictToPanelWidth)
{
	return true;
}

void MkPanel::BuildAndUpdateTextCache(void)
{
	if ((m_TargetTextNodePtr != NULL) && (m_DrawStep != NULL))
	{
		MkDrawTextNodeStep* drawTextNodeStep = dynamic_cast<MkDrawTextNodeStep*>(m_DrawStep);
		if (drawTextNodeStep != NULL)
		{
			m_TargetTextNodePtr->Build();

			// draw step은 재사용하지만 texture는 Build()시 크기가 변할 수 있으므로 파괴 후 재생성
			m_Texture = NULL; // ref-
			m_MaterialKey.m_TextureID = 0;

			drawTextNodeStep->SetUp(m_TargetTextNodePtr);

			m_DrawStep->GetTargetTexture(0, m_Texture);
			m_MaterialKey.m_TextureID = MK_PTR_TO_ID64(m_Texture.GetPtr());
		}
	}
}
/*
void MkPanel::RestoreDecoString(void)
{
	if (!m_OriginalDecoStr.Empty())
	{
		MkStr backup = m_OriginalDecoStr;
		SetDecoString(backup);
	}
	else if (!m_SceneDecoTextNodeNameAndKey.Empty())
	{
		MkArray<MkHashStr> backup = m_SceneDecoTextNodeNameAndKey;
		SetDecoString(backup);
	}
}
*/
/*
void MkPanel::AlignRect(const MkFloat2& anchorSize, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset)
{
	if (!anchorSize.IsZero() && m_LocalRect.SizeIsNotZero() && (alignment != eRAP_NonePosition))
	{
		MkFloat2 localPos =	MkFloatRect(anchorSize).GetSnapPosition(m_LocalRect, alignment, border);
		localPos.y += heightOffset;
		SetLocalPosition(localPos);
	}
}
*/
void MkPanel::Clear(void)
{
	m_PixelScrollPosition.Clear();
	m_Texture = NULL;
	m_MaterialKey.m_TextureID = 0;
	m_SubsetOrSequenceName.Clear();
	m_SequenceStartTime = 0.;
	m_SequenceInitTime = 0.;
	m_TargetTextNodeName.Clear();
	MK_DELETE(m_TargetTextNodePtr);
	MK_DELETE(m_DrawStep);
}
/*
void MkPanel::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, TEMPLATE_NAME.GetString() + L" template node alloc 실패")
		return;

	tNode->CreateUnit(POSITION_KEY, MkVec2::Zero);
	tNode->CreateUnit(SIZE_KEY, MkVec2::Zero);
	tNode->CreateUnit(DEPTH_KEY, 0.f);
	tNode->CreateUnit(FORCED_FONT_STATE_KEY, MkStr::Null);
	tNode->CreateUnit(RESOURCE_KEY, MkStr::Null);
	tNode->CreateUnit(ALPHA_KEY, static_cast<unsigned int>(255));
	MkArray<bool> refBuf;
	refBuf.Fill(2, false);
	tNode->CreateUnit(REFLECTION_KEY, refBuf);
	tNode->CreateUnit(VISIBLE_KEY, true);

	tNode->DeclareToTemplate(true);
}
*/
void MkPanel::__ExcuteCustomDrawStep(void)
{
	if (m_DrawStep != NULL)
	{
		m_DrawStep->Draw();
	}
}

bool MkPanel::__CheckDrawable(void) const
{
	return
		(GetVisible() && // 보이기 활성화 중이고
		(m_Texture != NULL) && // 텍스쳐가 존재하고
		(m_PanelSize.x > 0) && (m_PanelSize.y > 0) && (m_AABR.size.x > 0) && (m_AABR.size.y > 0) && // 유효한 크기가 존재하고
		(m_Transform.GetWorldDepth() >= 0.f) && (m_Transform.GetWorldDepth() <= MKDEF_PA_MAX_WORLD_DEPTH) && // 깊이값이 카메라 범위 안에 있고
		(m_MaterialKey.m_ObjectAlpha > 0)); // 오브젝트 알파가 0보다 크면 true
}

bool MkPanel::__CheckDrawable(const MkFloatRect& cameraAABR) const
{
	return (__CheckDrawable() && m_AABR.CheckIntersection(cameraAABR)); // 영역 체크도 통과하면 true
}

void MkPanel::__Update(const MkSceneTransform* parentTransform, double currTime)
{
	// transform
	m_Transform.Update(parentTransform);

	// alpha
	m_MaterialKey.m_ObjectAlpha = static_cast<DWORD>(m_Transform.GetWorldAlpha() * 255.f);

	// world vertex and uv
	if (m_Texture != NULL)
	{
		const MkImageInfo::Subset* ssPtr = m_Texture->GetImageInfo().GetCurrentSubsetPtr(m_SubsetOrSequenceName, currTime - m_SequenceStartTime + m_SequenceInitTime);
		if (ssPtr == NULL)
		{
			m_WorldVertice[MkFloatRect::eLeftTop] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eRightTop] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eLeftBottom] = m_Transform.GetWorldPosition();
			m_WorldVertice[MkFloatRect::eRightBottom] = m_Transform.GetWorldPosition();
			m_AABR.position = m_Transform.GetWorldPosition();
			m_AABR.size.Clear();
		}
		else
		{
			bool copyU = true, copyV = true;

			// 휘발성 local position/size offset
			MkFloat2 internalPos, internalSize;

			// x size, u
			if (m_PanelSize.x > ssPtr->rectSize.x) // smaller src
			{
				switch (GetSmallerSourceOp())
				{
				case eReducePanel: // source의 크기에 맞게 panel을 축소해 맞춤
					m_PanelSize.x = ssPtr->rectSize.x;
					break;
				case eExpandSource: // panel 크기에 맞게 source를 확대해 맞춤
					break;
				case eAttachToLeftTop: // panel과 source 크기를 모두 유지. panel의 left-top을 기준으로 출력
					internalSize.x = ssPtr->rectSize.x - m_PanelSize.x;
					break;
				}
			}
			else if (m_PanelSize.x < ssPtr->rectSize.x) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source의 크기에 맞게 panel을 확대해 맞춤
					m_PanelSize.x = ssPtr->rectSize.x;
					break;
				case eReduceSource: // panel 크기에 맞게 source를 축소해 맞춤
					break;
				case eCutSource: // source를 panel 크기에 맞게 일부만 잘라 보여줌
					{
						float uLength = ssPtr->uv[MkFloatRect::eRightTop].x - ssPtr->uv[MkFloatRect::eLeftTop].x;
						m_PixelScrollPosition.x = Clamp<float>(m_PixelScrollPosition.x, 0.f, ssPtr->rectSize.x - m_PanelSize.x);
						float uBegin = uLength * m_PixelScrollPosition.x / ssPtr->rectSize.x;
						float uEnd = uBegin + uLength * m_PanelSize.x / ssPtr->rectSize.x;
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
			if (m_PanelSize.y > ssPtr->rectSize.y) // smaller src
			{
				switch (GetSmallerSourceOp())
				{
				case eReducePanel: // source의 크기에 맞게 panel을 축소해 맞춤
					m_PanelSize.y = ssPtr->rectSize.y;
					break;
				case eExpandSource: // panel 크기에 맞게 source를 확대해 맞춤
					break;
				case eAttachToLeftTop: // panel과 source 크기를 모두 유지. panel의 left-top을 기준으로 출력
					internalPos.y = m_PanelSize.y - ssPtr->rectSize.y;
					internalSize.y = -internalPos.y;
					break;
				}
			}
			else if (m_PanelSize.y < ssPtr->rectSize.y) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source의 크기에 맞게 panel을 확대해 맞춤
					m_PanelSize.y = ssPtr->rectSize.y;
					break;
				case eReduceSource: // panel 크기에 맞게 source를 축소해 맞춤
					break;
				case eCutSource: // source를 panel 크기에 맞게 일부만 잘라 보여줌
					{
						float vLength = ssPtr->uv[MkFloatRect::eLeftBottom].y - ssPtr->uv[MkFloatRect::eLeftTop].y;
						m_PixelScrollPosition.y = Clamp<float>(m_PixelScrollPosition.y, 0.f, ssPtr->rectSize.y - m_PanelSize.y);
						float vBegin = vLength * m_PixelScrollPosition.y / ssPtr->rectSize.y;
						float vEnd = vBegin + vLength * m_PanelSize.y / ssPtr->rectSize.y;
						m_UV[MkFloatRect::eLeftTop].y = vBegin;
						m_UV[MkFloatRect::eRightTop].y = vBegin;
						m_UV[MkFloatRect::eLeftBottom].y = vEnd;
						m_UV[MkFloatRect::eRightBottom].y = vEnd;
						copyV = false;
					}
					break;
				}
			}
			
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
			m_Transform.GetWorldRectVertices(MkFloatRect(internalPos, m_PanelSize + internalSize), m_WorldVertice);

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

void MkPanel::__FillVertexData(MkArray<VertexData>& buffer) const
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

void MkPanel::__AffectTexture(void) const
{
	if (m_Texture != NULL)
	{
		m_Texture->UpdateRenderState(m_MaterialKey.m_ObjectAlpha);
	}
}
/*
MkPanel::eSrcType MkPanel::__GetSrcInfo(MkPathName& imagePath, MkHashStr& subsetName, MkStr& decoStr, MkArray<MkHashStr>& nodeNameAndKey) const
{
	if (m_Texture != NULL)
	{
		const MkHashStr& imgPath = m_Texture->GetPoolKey();

		// deco str
		if (imgPath.Empty())
		{
			if (!m_OriginalDecoStr.Empty())
			{
				decoStr = m_OriginalDecoStr;
				return eCustomDecoStr;
			}
			else if (!m_SceneDecoTextNodeNameAndKey.Empty())
			{
				nodeNameAndKey = m_SceneDecoTextNodeNameAndKey;
				return eSceneDecoStr;
			}
			else
				return eRenderToTexture;
		}

		imagePath = imgPath.GetString();
		subsetName = m_SubsetOrSequenceName;
		return eStaticImage;
	}
	return eNone;
}
*/
MkPanel::MkPanel()
{
	SetSmallerSourceOp(eReducePanel);
	SetBiggerSourceOp(eExpandPanel);

	m_Attribute.Clear();
	SetVisible(true);
	SetHorizontalReflection(false);
	SetVerticalReflection(false);

	m_TargetTextNodePtr = NULL;
	m_DrawStep = NULL;
}

void MkPanel::_FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkArray<VertexData>& buffer) const
{
	VertexData& vd = buffer.PushBack();

	const MkFloat2& wv = m_WorldVertice[pn];
	vd.x = wv.x;
	vd.y = wv.y;
	vd.z = m_Transform.GetWorldDepth();

	const MkFloat2& uv = m_UV[pn];
	vd.u = (hr) ? (1.f - uv.x) : uv.x;
	vd.v = (vr) ? (1.f - uv.y) : uv.y;
}

//------------------------------------------------------------------------------------------------//