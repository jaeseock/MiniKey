
#include "MkCore_MkCheck.h"
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkTexturePool.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkSRect.h"


// MkVec2
const static MkHashStr POSITION_KEY = L"Position";

// MkVec2
const static MkHashStr SIZE_KEY = L"Size";

// float
const static MkHashStr DEPTH_KEY = L"Depth";

// MkStr
const static MkHashStr FORCED_FONT_TYPE_KEY = L"FontType";

// MkStr
const static MkHashStr FORCED_FONT_STATE_KEY = L"FontState";

// MkArray<MkStr>
const static MkHashStr RESOURCE_KEY = L"Resource";

// map : MkStr 4개
// - tag(MAP_TAG)
// - bitmap file path
// - group
// - subset name
const static MkStr MAP_TAG = L"map";

// original deco text : MkStr 2개
// - tag(TEXT_O_TAG)
// - deco text
const static MkStr TEXT_O_TAG = L"odt";

// scene deco text : MkStr 1 + n개
// - tag(TEXT_S_TAG)
// - MK_WIN_RES_MGR에 등록되어 있는 deco text node name & key
const static MkStr TEXT_S_TAG = L"sdt";

// unsigned int
const static MkHashStr ALPHA_KEY = L"Alpha";

// bool(horizontal), bool(vertical)
const static MkHashStr REFLECTION_KEY = L"Reflection";

// bool
const static MkHashStr VISIBLE_KEY = L"Visible";

//------------------------------------------------------------------------------------------------//

void MkSRect::Load(const MkDataNode& node)
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

	// font type
	MkStr fontType;
	node.GetData(FORCED_FONT_TYPE_KEY, fontType, 0);
	m_ForcedFontType = fontType;

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
		if ((tag == MAP_TAG) && (resBuf.GetSize() == 4)) // map
		{
			MkPathName filePath = resBuf[1];
			unsigned int group = resBuf[2].ToUnsignedInteger();
			if (MK_TEXTURE_POOL.LoadBitmapTexture(filePath, group))
			{
				MkBaseTexturePtr texture;
				MK_TEXTURE_POOL.GetBitmapTexture(filePath, texture, group);
				SetTexture(texture, resBuf[3]);
			}
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

void MkSRect::Save(MkDataNode& node) // Load의 역
{
	node.Clear();
	node.ApplyTemplate(MKDEF_S2D_BT_SRECT_TEMPLATE_NAME);

	node.SetData(POSITION_KEY, MkVec2(m_LocalRect.position.x, m_LocalRect.position.y), 0);
	node.SetData(SIZE_KEY, MkVec2(m_LocalRect.size.x, m_LocalRect.size.y), 0);
	node.SetData(DEPTH_KEY, m_LocalDepth, 0);
	node.SetData(FORCED_FONT_TYPE_KEY, m_ForcedFontType.GetString(), 0);
	node.SetData(FORCED_FONT_STATE_KEY, m_ForcedFontState.GetString(), 0);

	if (m_Texture != NULL)
	{
		MkArray<MkStr> resBuf;

		if (m_SceneDecoTextNodeNameAndKey.Empty())
		{
			if (m_OriginalDecoStr.Empty()) // map
			{
				resBuf.Reserve(4);
				resBuf.PushBack(MAP_TAG);
				resBuf.PushBack(m_Texture->GetPoolKey().GetString());
				resBuf.PushBack(MkStr(m_Texture->GetBitmapGroup()));
				resBuf.PushBack(m_CurrentSubsetName.GetString());
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

void MkSRect::SetObjectAlpha(float alpha)
{
	m_MaterialKey.m_ObjectAlpha = static_cast<DWORD>(alpha * 255.f);
}

float MkSRect::GetObjectAlpha(void) const
{
	return (static_cast<float>(m_MaterialKey.m_ObjectAlpha) / 255.f);
}

void MkSRect::SetFocedFontTypeAndState(const MkHashStr& type, const MkHashStr& state)
{
	bool typeDiff = (m_ForcedFontType != type);
	bool stateDiff = (m_ForcedFontState != state);

	if (typeDiff || stateDiff)
	{
		m_ForcedFontType = type;
		if (!MK_FONT_MGR.CheckAvailableFontType(type))
		{
			m_ForcedFontType.Clear();
		}

		m_ForcedFontState = state;
		if (!MK_FONT_MGR.CheckAvailableFontState(state))
		{
			m_ForcedFontState.Clear();
		}

		RestoreDecoString();
	}
}

void MkSRect::SetTexture(const MkBaseTexturePtr& texture)
{
	SetTexture(texture, L"");
}

void MkSRect::SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetName)
{
	Clear();

	m_Texture = texture;
	m_MaterialKey.m_TextureID = MK_PTR_TO_ID64(m_Texture.GetPtr());

	SetSubset(subsetName);
}

void MkSRect::SetTexture(const MkPathName& imagePath, const MkHashStr& subsetName)
{
	MkBaseTexturePtr texture;
	MK_TEXTURE_POOL.GetBitmapTexture(imagePath, texture, 0);
	if (texture != NULL)
	{
		SetTexture(texture, subsetName);
	}
}

bool MkSRect::SetDecoString(const MkStr& decoStr)
{
	Clear();

	if (decoStr.GetFirstValidPosition() == MKDEF_ARRAY_ERROR)
		return false;

	bool ok = _SetDecoString(MkDecoStr(decoStr));
	if (ok)
	{
		m_OriginalDecoStr = decoStr;
	}
	return ok;
}

bool MkSRect::SetDecoString(const MkArray<MkHashStr>& nodeNameAndKey)
{
	if (!nodeNameAndKey.Empty())
	{
		Clear();

		const MkDecoStr& decoStr = MK_WR_DECO_TEXT.GetDecoText(nodeNameAndKey);
		if ((!decoStr.Empty()) && _SetDecoString(decoStr))
		{
			m_SceneDecoTextNodeNameAndKey = nodeNameAndKey;
			return true;
		}
	}
	return false;
}

void MkSRect::RestoreDecoString(void)
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

void MkSRect::SetSubset(const MkHashStr& name)
{
	m_CurrentSubsetName = name;

	if (m_Texture != NULL)
	{
		m_Texture->GetSubsetInfo(m_CurrentSubsetName, m_LocalRect.size, m_UV);
	}
}

bool MkSRect::CheckValidation(const MkFloatRect& cameraAABR) const
{
	return
		(m_Visible && // 보이기 설정이 되어 있고
		m_LocalRect.SizeIsNotZero() && // 유효한 크기가 존재하며
		((m_WorldDepth >= 0.f) && (m_WorldDepth <= MKDEF_S2D_MAX_WORLD_DEPTH)) && // 깊이값이 카메라 범위 안에 있고
		(m_Texture != NULL) && // 텍스쳐가 있으며
		(m_MaterialKey.m_ObjectAlpha > 0) && // 오브젝트 알파가 0보다 크고
		m_AABR.CheckIntersection(cameraAABR)); // 프러스텀 체크도 통과하면 true
}

void MkSRect::AlignRect(const MkFloat2& anchorSize, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset, float depthOffset)
{
	if (!anchorSize.IsZero() && m_LocalRect.SizeIsNotZero() && (alignment != eRAP_NonePosition))
	{
		MkFloat2 localPos =	MkFloatRect(anchorSize).GetSnapPosition(m_LocalRect, alignment, border);
		localPos.y += heightOffset;
		SetLocalPosition(localPos);
		SetLocalDepth(m_LocalDepth + depthOffset);
	}
}

void MkSRect::Clear(void)
{
	m_Texture = NULL;
	m_TextCacheStep.Clear();
	m_SceneDecoTextNodeNameAndKey.Clear();
	m_OriginalDecoStr.Clear();
	m_MaterialKey.m_TextureID = 0;
	m_LocalRect.size.Clear();
}

void MkSRect::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MKDEF_S2D_BT_SRECT_TEMPLATE_NAME);
	MK_CHECK(tNode != NULL, MkStr(MKDEF_S2D_BT_SRECT_TEMPLATE_NAME) + L" template node alloc 실패")
		return;

	tNode->CreateUnit(POSITION_KEY, MkVec2::Zero);
	tNode->CreateUnit(SIZE_KEY, MkVec2::Zero);
	tNode->CreateUnit(DEPTH_KEY, 0.f);
	tNode->CreateUnit(FORCED_FONT_TYPE_KEY, MkStr::Null);
	tNode->CreateUnit(FORCED_FONT_STATE_KEY, MkStr::Null);
	tNode->CreateUnit(RESOURCE_KEY, MkStr::Null);
	tNode->CreateUnit(ALPHA_KEY, static_cast<unsigned int>(255));
	MkArray<bool> refBuf;
	refBuf.Fill(2, false);
	tNode->CreateUnit(REFLECTION_KEY, refBuf);
	tNode->CreateUnit(VISIBLE_KEY, true);

	tNode->DeclareToTemplate(true);
}

void MkSRect::__GenerateTextCache(void)
{
	m_TextCacheStep.Draw();
}

void MkSRect::__UpdateTransform(const MkVec3& position, float rotation, float cosR, float sinR, float scale)
{
	if ((m_LocalRect.size.x > 0.f) && (m_LocalRect.size.y > 0.f))
	{
		// update world vertex
		_UpdateWorldVertex(MkFloatRect::eLeftTop, position, rotation, cosR, sinR, scale);
		_UpdateWorldVertex(MkFloatRect::eRightTop, position, rotation, cosR, sinR, scale);
		_UpdateWorldVertex(MkFloatRect::eLeftBottom, position, rotation, cosR, sinR, scale);
		_UpdateWorldVertex(MkFloatRect::eRightBottom, position, rotation, cosR, sinR, scale);

		// update depth
		m_WorldDepth = m_LocalDepth + position.z;

		// update AABR
		MkFloat2 minPt(m_WorldVertex[MkFloatRect::eLeftTop]);
		minPt.CompareAndKeepMin(m_WorldVertex[MkFloatRect::eRightTop]);
		minPt.CompareAndKeepMin(m_WorldVertex[MkFloatRect::eLeftBottom]);
		minPt.CompareAndKeepMin(m_WorldVertex[MkFloatRect::eRightBottom]);

		MkFloat2 maxPt(m_WorldVertex[MkFloatRect::eLeftTop]);
		maxPt.CompareAndKeepMax(m_WorldVertex[MkFloatRect::eRightTop]);
		maxPt.CompareAndKeepMax(m_WorldVertex[MkFloatRect::eLeftBottom]);
		maxPt.CompareAndKeepMax(m_WorldVertex[MkFloatRect::eRightBottom]);

		m_AABR.position = minPt;
		m_AABR.size = maxPt - minPt;
	}
}

void MkSRect::__FillVertexData(MkArray<VertexData>& buffer) const
{
	// xyz, uv
	// 0 --- 1
	// |  /  |
	// 2 --- 3
	// seq : 0, 1, 2, 2, 1, 3

	_FillVertexData(MkFloatRect::eLeftTop, buffer); // 0
	_FillVertexData(MkFloatRect::eRightTop, buffer); // 1
	_FillVertexData(MkFloatRect::eLeftBottom, buffer); // 2
	_FillVertexData(MkFloatRect::eLeftBottom, buffer); // 2
	_FillVertexData(MkFloatRect::eRightTop, buffer); // 1
	_FillVertexData(MkFloatRect::eRightBottom, buffer); // 3
}

void MkSRect::__AffectTexture(void) const
{
	if (m_Texture != NULL)
	{
		m_Texture->UpdateRenderState(m_MaterialKey.m_ObjectAlpha);
	}
}

MkSRect::MkSRect()
{
	SetLocalDepth(0.f);

	SetObjectAlpha(1.f);

	SetHorizontalReflection(false);
	SetVerticalReflection(false);
	
	SetVisible(true);
}

//------------------------------------------------------------------------------------------------//

void MkSRect::_UpdateWorldVertex(MkFloatRect::ePointName pn, const MkVec3& position, float rotation, float cosR, float sinR, float scale)
{
	MkFloat2 lp = m_LocalRect.GetAbsolutePosition(pn);
	MkFloat2& wv = m_WorldVertex[pn];

	wv.x = (cosR * lp.x + sinR * lp.y) * scale + position.x; // CW rotation
	wv.y = (-sinR * lp.x + cosR * lp.y) * scale + position.y;
}

void MkSRect::_FillVertexData(MkFloatRect::ePointName pn, MkArray<VertexData>& buffer) const
{
	VertexData& vd = buffer.PushBack();

	const MkFloat2& wv = m_WorldVertex[pn];
	vd.x = wv.x;
	vd.y = wv.y;

	vd.z = m_WorldDepth;

	const MkFloat2& uv = m_UV[pn];
	vd.u = (m_HorizontalReflection) ? (1.f - Clamp<float>(uv.x, 0.f, 1.f)) : Clamp<float>(uv.x, 0.f, 1.f);
	vd.v = (m_VerticalReflection) ? (1.f - Clamp<float>(uv.y, 0.f, 1.f)) : Clamp<float>(uv.y, 0.f, 1.f);
}

bool MkSRect::_SetDecoString(const MkDecoStr& decoStr)
{
	bool ok;
	bool changeType = (!m_ForcedFontType.Empty());
	bool changeState = (!m_ForcedFontState.Empty());
	if (changeType || changeState)
	{
		MkDecoStr tmpStr = decoStr;
		if (changeType)
		{
			tmpStr.ChangeType(m_ForcedFontType);
		}
		if (changeState)
		{
			tmpStr.ChangeState(m_ForcedFontState);
		}
		ok = m_TextCacheStep.SetUp(tmpStr);
	}
	else
	{
		ok = m_TextCacheStep.SetUp(decoStr);
	}
	if (ok)
	{
		m_TextCacheStep.GetTargetTexture(m_Texture);
		m_MaterialKey.m_TextureID = MK_PTR_TO_ID64(m_Texture.GetPtr());
	}

	SetSubset(L"");
	return ok;
}

//------------------------------------------------------------------------------------------------//