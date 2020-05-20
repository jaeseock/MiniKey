
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTimeManager.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkShaderEffect.h"
#include "MkPA_MkShaderEffectSetting.h"
#include "MkPA_MkShaderEffectPool.h"
#include "MkPA_MkDrawTextNodeStep.h"
#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkPanel.h"


const MkHashStr MkPanel::ObjKey_Attribute(L"Attribute");
const MkHashStr MkPanel::ObjKey_PanelSize(L"PanelSize");
const MkHashStr MkPanel::ObjKey_PixelScrollPosition(L"PScrollPos");
const MkHashStr MkPanel::ObjKey_ImagePath(L"ImagePath");
const MkHashStr MkPanel::ObjKey_SequenceTimeOffset(L"STimeOffset");
const MkHashStr MkPanel::ObjKey_TextNodeName(L"TextNodeName");
const MkHashStr MkPanel::ObjKey_TextNodeData(L"TextNodeData");
const MkHashStr MkPanel::ObjKey_TextNodeWidthRestriction(L"TextNodeWR");
const MkHashStr MkPanel::ObjKey_ShaderEffect(L"ShaderEffect");
const MkHashStr MkPanel::ObjKey_EffectName(L"EffectName");
const MkHashStr MkPanel::ObjKey_EffectUDP(L"UDP");

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
	ClearMainTexture();

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

float MkPanel::GetSequenceProgress(double currTime, bool ignoreLoop) const
{
	if ((m_Texture == NULL) || m_SubsetOrSequenceName.Empty())
		return 0.f;

	const MkImageInfo& info = GetTexturePtr()->GetImageInfo();
	if (!info.GetSequences().Exist(m_SubsetOrSequenceName))
		return 0.f;
	
	currTime -= m_SequenceStartTime;
	currTime += m_SequenceTimeOffset;

	if (currTime >= 0.)
	{
		const MkImageInfo::Sequence& seq = info.GetSequences()[m_SubsetOrSequenceName];
		currTime = (ignoreLoop || seq.loop) ? MkFloatOp::GetRemainder(currTime, seq.totalRange) : GetMin<double>(currTime, seq.totalRange);
		return static_cast<float>(currTime / seq.totalRange);
	}
	return 0.f;
}

void MkPanel::SetTextNode(const MkTextNode& source, bool restrictToPanelWidth)
{
	ClearMainTexture();

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

		m_DrawStep = drawStep;

		// setp 초기화 및 설정
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

void MkPanel::SetTextMsg(const MkStr& msg, bool restrictToPanelWidth)
{
	MkTextNode textNode;
	textNode.SetFontType(MkFontManager::DefaultT);
	textNode.SetFontStyle(MkFontManager::DefaultS);
	textNode.SetText(msg);
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
			
			// draw step은 재사용하지만 texture는 Build()시 크기가 변할 수 있으므로 파괴 후 재생성
			m_Texture = NULL; // ref-
			m_MaterialKey.m_MainKey = 0;

			// setp 초기화 및 설정
			_UpdateTextNodeTexture(drawStep);
		}
	}
}

void MkPanel::SetMaskingNode(const MkSceneNode* sceneNode)
{
	MK_CHECK((m_PanelSize.x >= 1.f) && (m_PanelSize.y >= 1.f), L"유효한 panel size가 설정되어 있어야 함")
		return;

	ClearMainTexture();

	if (sceneNode != NULL)
	{
		do
		{
			MkDrawSceneNodeStep* drawStep = new MkDrawSceneNodeStep;
			MK_CHECK(drawStep != NULL, L"MkDrawSceneNodeStep alloc 실패")
				break;

			m_DrawStep = drawStep;
			MK_CHECK(drawStep->SetUp(MkRenderTarget::eTexture, 1, MkInt2(static_cast<int>(m_PanelSize.x), static_cast<int>(m_PanelSize.y))), L"MkDrawSceneNodeStep SetUp 실패")
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

bool MkPanel::SetShaderEffect(const MkHashStr& name)
{
	if (name == m_ShaderEffectName)
		return true;

	if (!m_ShaderEffectName.Empty())
	{
		ClearShaderEffect();
	}

	MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(name);
	if (effect == NULL)
		return false;

	m_ShaderEffectSetting = effect->CreateEffectSetting();
	bool ok = (m_ShaderEffectSetting != NULL);
	if (ok)
	{
		m_ShaderEffectName = name;
	}
	return ok;
}

bool MkPanel::SetTechnique(const MkHashStr& name)
{
	if (m_ShaderEffectName.Empty() || (m_ShaderEffectSetting == NULL))
		return false;

	MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(m_ShaderEffectName);
	if ((effect == NULL) || (!effect->IsValidTechnique(name)))
		return false;

	m_ShaderEffectSetting->SetTechnique(name);
	return true;
}

const MkHashStr& MkPanel::GetTechnique(void) const
{
	return (m_ShaderEffectSetting == NULL) ? MkHashStr::EMPTY : m_ShaderEffectSetting->GetTechnique();
}

bool MkPanel::SetEffectTexture1(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName)
{
	return _SetEffectTexture(0, texture, subsetOrSequenceName);
}
bool MkPanel::SetEffectTexture1(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName)
{
	return SetEffectTexture1(imagePath.Empty() ? NULL : MK_BITMAP_POOL.GetBitmapTexture(imagePath), subsetOrSequenceName);
}

bool MkPanel::SetEffectTexture2(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName)
{
	return _SetEffectTexture(1, texture, subsetOrSequenceName);
}

bool MkPanel::SetEffectTexture2(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName)
{
	return SetEffectTexture2(imagePath.Empty() ? NULL : MK_BITMAP_POOL.GetBitmapTexture(imagePath), subsetOrSequenceName);
}

bool MkPanel::SetEffectTexture3(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName)
{
	return _SetEffectTexture(2, texture, subsetOrSequenceName);
}

bool MkPanel::SetEffectTexture3(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName)
{
	return SetEffectTexture3(imagePath.Empty() ? NULL : MK_BITMAP_POOL.GetBitmapTexture(imagePath), subsetOrSequenceName);
}

const MkBaseTexture* MkPanel::GetEffectTexturePtr1(void) const { return (m_EffectTexture[0] == NULL) ? NULL : m_EffectTexture[0].GetPtr(); }
const MkBaseTexture* MkPanel::GetEffectTexturePtr2(void) const { return (m_EffectTexture[1] == NULL) ? NULL : m_EffectTexture[1].GetPtr(); }
const MkBaseTexture* MkPanel::GetEffectTexturePtr3(void) const { return (m_EffectTexture[2] == NULL) ? NULL : m_EffectTexture[2].GetPtr(); }

bool MkPanel::SetEffectSubsetOrSequenceName1(const MkHashStr& subsetOrSequenceName) { return _SetEffectSubsetOrSequenceName(0, subsetOrSequenceName); }
bool MkPanel::SetEffectSubsetOrSequenceName2(const MkHashStr& subsetOrSequenceName) { return _SetEffectSubsetOrSequenceName(1, subsetOrSequenceName); }
bool MkPanel::SetEffectSubsetOrSequenceName3(const MkHashStr& subsetOrSequenceName) { return _SetEffectSubsetOrSequenceName(2, subsetOrSequenceName); }

bool MkPanel::SetUserDefinedProperty(const MkHashStr& name, float x) { return SetUserDefinedProperty(name, x, 0.f, 0.f, 0.f); }
bool MkPanel::SetUserDefinedProperty(const MkHashStr& name, float x, float y) { return SetUserDefinedProperty(name, x, y, 0.f, 0.f); }
bool MkPanel::SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z) { return SetUserDefinedProperty(name, x, y, z, 0.f); }

bool MkPanel::SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z, float w)
{
	return (m_ShaderEffectSetting == NULL) ? false : m_ShaderEffectSetting->SetUDP(name, D3DXVECTOR4(x, y, z, w));
}

bool MkPanel::GetUserDefinedProperty(const MkHashStr& name, float& x) const  { float y, z, w; return GetUserDefinedProperty(name, x, y, z, w); }
bool MkPanel::GetUserDefinedProperty(const MkHashStr& name, float& x, float& y) const { float z, w; return GetUserDefinedProperty(name, x, y, z, w); }
bool MkPanel::GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z) const { float w; return GetUserDefinedProperty(name, x, y, z, w); }

bool MkPanel::GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z, float& w) const
{
	if (m_ShaderEffectSetting == NULL)
		return false;

	D3DXVECTOR4 buffer;
	bool ok = m_ShaderEffectSetting->GetUDP(name, buffer);
	if (ok)
	{
		x = buffer.x;
		y = buffer.y;
		z = buffer.z;
		w = buffer.w;
	}
	return ok;
}

void MkPanel::ClearShaderEffect(void)
{
	m_ShaderEffectName.Clear();
	MK_DELETE(m_ShaderEffectSetting);

	m_EffectTexture[0] = NULL;
	m_EffectTexture[1] = NULL;
	m_EffectTexture[2] = NULL;
	m_EffectSubsetOrSequenceName[0].Clear();
	m_EffectSubsetOrSequenceName[1].Clear();
	m_EffectSubsetOrSequenceName[2].Clear();
}

void MkPanel::ClearMainTexture(void)
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

void MkPanel::Clear(void)
{
	ClearMainTexture();
	ClearShaderEffect();
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

bool MkPanel::__IsShaderEffectApplied(void) const
{
	return (m_ShaderEffectEnable && (m_ShaderEffectSetting != NULL));
}

void MkPanel::__DrawWithShaderEffect(LPDIRECT3DDEVICE9 device) const
{
	// 예외처리 하지 않음
	DWORD fvf = MKDEF_PANEL_FVF;
	unsigned int stride;
	
	if (m_EffectTexture[2] != NULL)
	{
		fvf |= D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(3);
		stride = sizeof(float) * 11;
	}
	else if (m_EffectTexture[1] != NULL)
	{
		fvf |= D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(2);
		stride = sizeof(float) * 9;
	}
	else if (m_EffectTexture[0] != NULL)
	{
		fvf |= D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(1);
		stride = sizeof(float) * 7;
	}
	else
	{
		stride = sizeof(float) * 5;
	}

	MK_RENDER_STATE.UpdateFVF(fvf);

	MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(m_ShaderEffectName);

	MkByteArray vertexData(stride * 6);
	__FillVertexData(vertexData);

	unsigned int passCount = effect->BeginTechnique(m_ShaderEffectSetting);
	for (unsigned int i=0; i<passCount; ++i)
	{
		effect->BeginPass(i);
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, vertexData.GetPtr(), stride);
		effect->EndPass();
	}
	effect->EndTechnique();
}

void MkPanel::Load(const MkDataNode& node)
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object는 독자적인 Load를 할 수 없음") {}
}

void MkPanel::Save(MkDataNode& node) const
{
	MK_CHECK(false, GetSceneClassKey().GetString() + L" object는 독자적인 Save를 할 수 없음") {}
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
	node.CreateUnit(ObjKey_SequenceTimeOffset, 0.f);

	// text node
	//ObjKey_TextNodeName
	//ObjKey_TextNodeData
	node.CreateUnit(ObjKey_TextNodeWidthRestriction, false);

	// shader effect
	//ObjKey_ShaderEffect
	//ObjKey_EffectName
	//ObjKey_ImagePath
	//ObjKey_EffectUDP
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
		MkArray<MkStr> imagePath;
		node.GetData(ObjKey_ImagePath, imagePath);
		if (imagePath.IsValidIndex(0))
		{
			float stOffset = 0.f;
			node.GetData(ObjKey_SequenceTimeOffset, stOffset, 0);

			SetTexture(imagePath[0], imagePath.IsValidIndex(1) ? imagePath[1] : MkHashStr::EMPTY, static_cast<double>(stOffset));
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

	// shader effect
	ClearShaderEffect();
	if (node.ChildExist(ObjKey_ShaderEffect))
	{
		const MkDataNode& shaderNode = *node.GetChildNode(ObjKey_ShaderEffect);

		do
		{
			// effect name
			MkArray<MkHashStr> effectName;
			if ((!shaderNode.GetDataEx(ObjKey_EffectName, effectName)) || effectName.Empty() || effectName[0].Empty())
				break;

			if (!SetShaderEffect(effectName[0]))
				break;

			// technique
			if (effectName.IsValidIndex(1))
			{
				SetTechnique(effectName[1]);
			}

			// texture image
			MkArray<MkStr> imagePath;
			shaderNode.GetData(ObjKey_ImagePath, imagePath);

			if (imagePath.IsValidIndex(1) && (!imagePath[0].Empty()))
			{
				SetEffectTexture1(imagePath[0], imagePath[1]);
			}
			if (imagePath.IsValidIndex(3) && (!imagePath[2].Empty()))
			{
				SetEffectTexture2(imagePath[2], imagePath[3]);
			}
			if (imagePath.IsValidIndex(5) && (!imagePath[4].Empty()))
			{
				SetEffectTexture3(imagePath[4], imagePath[5]);
			}

			// udp
			if (shaderNode.ChildExist(ObjKey_EffectUDP))
			{
				const MkDataNode& udpNode = *shaderNode.GetChildNode(ObjKey_EffectUDP);
				MkArray<MkHashStr> keys;
				udpNode.GetChildNodeList(keys);
				MK_INDEXING_LOOP(keys, i)
				{
					const MkHashStr& currKey = keys[i];
					MkArray<float> values;
					udpNode.GetData(currKey, values);
					if (values.GetSize() == 4)
					{
						SetUserDefinedProperty(currKey, values[0], values[1], values[2], values[3]);
					}
				}
			}
		}
		while (false);
	}
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
				MkArray<MkStr> imagePath(2);
				imagePath.PushBack(poolKey.GetString());
				if (!m_SubsetOrSequenceName.Empty())
				{
					imagePath.PushBack(m_SubsetOrSequenceName.GetString());
				}
				node.SetData(ObjKey_ImagePath, imagePath);
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

	// shader effect
	if ((!m_ShaderEffectName.Empty()) && (m_ShaderEffectSetting != NULL))
	{
		MkShaderEffect* effect = MK_SHADER_POOL.GetShaderEffect(m_ShaderEffectName);
		if (effect != NULL)
		{
			MkDataNode& shaderNode = *node.CreateChildNode(ObjKey_ShaderEffect);

			// effect name
			MkArray<MkStr> effectName(2);
			effectName.PushBack(m_ShaderEffectName.GetString());

			// technique
			if (m_ShaderEffectSetting->GetTechnique() != effect->GetDefaultTechnique())
			{
				effectName.PushBack(m_ShaderEffectSetting->GetTechnique().GetString());
			}

			shaderNode.CreateUnit(ObjKey_EffectName, effectName);

			// texture image
			MkArray<MkStr> imagePath;
			imagePath.Fill(6);

			const MkBaseTexture* texture1 = GetEffectTexturePtr1();
			if ((texture1 != NULL) && (texture1 != effect->GetDefaultTexture1()))
			{
				imagePath[0] = texture1->GetPoolKey().GetString();
				imagePath[1] = GetEffectSubsetOrSequenceName1().GetString();
			}
			
			const MkBaseTexture* texture2 = GetEffectTexturePtr2();
			if ((texture2 != NULL) && (texture2 != effect->GetDefaultTexture2()))
			{
				imagePath[2] = texture2->GetPoolKey().GetString();
				imagePath[3] = GetEffectSubsetOrSequenceName2().GetString();
			}
			
			const MkBaseTexture* texture3 = GetEffectTexturePtr3();
			if ((texture3 != NULL) && (texture3 != effect->GetDefaultTexture3()))
			{
				imagePath[4] = texture3->GetPoolKey().GetString();
				imagePath[5] = GetEffectSubsetOrSequenceName3().GetString();
			}
			
			if (imagePath[4].Empty())
			{
				imagePath.PopBack(2);

				if (imagePath[2].Empty())
				{
					imagePath.PopBack(2);

					if (imagePath[0].Empty())
					{
						imagePath.Clear();
					}
				}
			}

			if (!imagePath.Empty())
			{
				shaderNode.CreateUnit(ObjKey_ImagePath, imagePath);
			}

			// udp
			const MkHashMap<MkHashStr, D3DXVECTOR4>& udps = m_ShaderEffectSetting->GetUDP();
			if (!udps.Empty())
			{
				MkArray<MkHashStr> keys(udps.GetSize());
				MkArray<D3DXVECTOR4> v4s(udps.GetSize());

				MkConstHashMapLooper<MkHashStr, D3DXVECTOR4> looper(udps);
				MK_STL_LOOP(looper)
				{
					D3DXVECTOR4 defValue;
					if (effect->GetDefaultUDP(looper.GetCurrentKey(), defValue) && (looper.GetCurrentField() != defValue))
					{
						keys.PushBack(looper.GetCurrentKey());
						v4s.PushBack(looper.GetCurrentField());
					}
				}

				if (!keys.Empty())
				{
					MkDataNode& udpNode = *shaderNode.CreateChildNode(ObjKey_EffectUDP);
					MK_INDEXING_LOOP(keys, i)
					{
						const D3DXVECTOR4& v4 = v4s[i];
						MkArray<float> value;
						value.Fill(4);
						value[0] = v4.x;
						value[1] = v4.y;
						value[2] = v4.z;
						value[3] = v4.w;

						udpNode.CreateUnit(keys[i], value);
					}
				}
			}
		}
	}
}

bool MkPanel::__CheckDrawable(void) const
{
	return
		(GetVisible() && // 보이기 활성화 중이고
		(m_Texture != NULL) && // 텍스쳐가 존재하고
		m_PanelSize.IsPositive() && m_AABR.size.IsPositive() && // 유효한 크기가 존재하고
		(m_Transform.GetWorldDepth() >= 0.f) && (m_Transform.GetWorldDepth() <= MKDEF_PA_MAX_WORLD_DEPTH) && // 깊이값이 카메라 범위 안에 있고
		(m_MaterialKey.m_SubKey > 0)); // 오브젝트 알파가 0보다 크면 true
}

void MkPanel::__Update(const MkSceneTransform* parentTransform, double currTime)
{
	// transform
	m_Transform.Update(parentTransform);

	// alpha
	m_MaterialKey.m_SubKey = static_cast<DWORD>(m_Transform.GetWorldAlpha() * 255.f);

	if (m_ShaderEffectSetting != NULL)
	{
		m_ShaderEffectSetting->SetAlpha(m_Transform.GetWorldAlpha());
	}

	// world vertex and uv
	if (m_Texture != NULL)
	{
		double currentTimeStamp = currTime - m_SequenceStartTime + m_SequenceTimeOffset;
		const MkImageInfo::Subset* ssPtr = m_Texture->GetImageInfo().GetCurrentSubsetPtr(m_SubsetOrSequenceName, currentTimeStamp);
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

			// 휘발성 local rect
			MkFloatRect localVisibleRect;
			
			// x size, u
			if (m_PanelSize.x > m_TextureSize.x) // smaller src
			{
				switch (GetSmallerSourceOp())
				{
				case eReducePanel: // source의 크기에 맞게 panel을 축소해 맞춤
					m_PanelSize.x = m_TextureSize.x;
					break;
				case eExpandSource: // panel 크기에 맞게 source를 확대해 맞춤
					srcSizeChanged = true;
					break;
				case eAttachToLeftTop: // panel과 source 크기를 모두 유지. panel의 left-top을 기준으로 출력
					localVisibleRect.size.x = m_TextureSize.x - m_PanelSize.x;
					break;
				}
			}
			else if (m_PanelSize.x < m_TextureSize.x) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source의 크기에 맞게 panel을 확대해 맞춤
					m_PanelSize.x = m_TextureSize.x;
					break;
				case eReduceSource: // panel 크기에 맞게 source를 축소해 맞춤
					srcSizeChanged = true;
					break;
				case eCutSource: // source를 panel 크기에 맞게 일부만 잘라 보여줌
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
				case eReducePanel: // source의 크기에 맞게 panel을 축소해 맞춤
					m_PanelSize.y = m_TextureSize.y;
					break;
				case eExpandSource: // panel 크기에 맞게 source를 확대해 맞춤
					srcSizeChanged = true;
					break;
				case eAttachToLeftTop: // panel과 source 크기를 모두 유지. panel의 left-top을 기준으로 출력
					localVisibleRect.position.y = m_PanelSize.y - m_TextureSize.y;
					localVisibleRect.size.y = -localVisibleRect.position.y;
					break;
				}
			}
			else if (m_PanelSize.y < m_TextureSize.y) // bigger src
			{
				switch (GetBiggerSourceOp())
				{
				case eExpandPanel: // source의 크기에 맞게 panel을 확대해 맞춤
					m_PanelSize.y = m_TextureSize.y;
					break;
				case eReduceSource: // panel 크기에 맞게 source를 축소해 맞춤
					srcSizeChanged = true;
					break;
				case eCutSource: // source를 panel 크기에 맞게 일부만 잘라 보여줌
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

			localVisibleRect.size += m_PanelSize;

			// pivot
			eRectAlignmentPosition pivot = m_Texture->GetImageInfo().GetCurrentPivot(m_SubsetOrSequenceName);
			if (pivot != eRAP_LeftBottom)
			{
				switch (GetHorizontalRectAlignmentType(pivot))
				{
				case eRAT_Middle: localVisibleRect.position.x -= MkFloatOp::SnapToUpperBound(localVisibleRect.size.x * 0.5f, 1.f); break;
				case eRAT_Right: localVisibleRect.position.x -= localVisibleRect.size.x; break;
				}
				switch (GetVerticalRectAlignmentType(pivot))
				{
				case eRAT_Top: localVisibleRect.position.y -= localVisibleRect.size.y; break;
				case eRAT_Center: localVisibleRect.position.y -= MkFloatOp::SnapToUpperBound(localVisibleRect.size.y * 0.5f, 1.f); break;
				}
			}

			// vertices
			m_Transform.GetWorldVertices(localVisibleRect, m_WorldVertice, true);

			// filtering mode. 이미지 원본을 확대/축소 할 경우 point보다는 linear filtering이 유리
			// 원칙적으로는 최종 출력 rect로 비교해야 하지만 잘라 그리기의 경우 uv로부터 size를 역산해야되는데 floating 계산 특성상 발생하는
			// 오차로 인해 오히려 잘못된 결과가 나올 가능성이 높음. 따라서 단순하게 조건식으로 처리
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

			if (m_ShaderEffectSetting != NULL)
			{
				m_ShaderEffectSetting->SetTexture0(m_Texture.GetPtr());

				_UpdateEffectUV(0, currentTimeStamp);
				_UpdateEffectUV(1, currentTimeStamp);
				_UpdateEffectUV(2, currentTimeStamp);
			}
		}
	}
}

bool MkPanel::__CheckWorldIntersection(const MkFloat2& worldPoint) const
{
	if (m_Transform.GetWorldRotation() == 0.f) // rotation이 없으므로 (m_WorldVertice == m_AABR)임
		return m_AABR.CheckIntersection(worldPoint);

	// minikey의 좌표계에서는 line vector와 point의 외적값이 line의 좌측에 존재하면 양수, 우측에 존재하면 음수임
	// 따라서 어느 한 라인에서라도 외적값이 양수면 바깥쪽에 존재함이 증명 됨
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

	m_ShaderEffectEnable = true;
	m_ShaderEffectSetting = NULL;
}

bool MkPanel::_SetEffectTexture(unsigned int index, const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName)
{
	m_EffectTexture[index] = NULL;
	m_EffectSubsetOrSequenceName[index].Clear();

	if (texture == NULL)
		return false;

	m_EffectTexture[index] = const_cast<MkBaseTexture*>(texture); // ref++
	bool ok = _SetEffectSubsetOrSequenceName(index, subsetOrSequenceName);
	if (ok && (m_ShaderEffectSetting != NULL))
	{
		_UpdateEffectUV(index, 0.);
	}
	return ok;
}

bool MkPanel::_SetEffectSubsetOrSequenceName(unsigned int index, const MkHashStr& subsetOrSequenceName)
{
	bool ok = ((m_EffectTexture[index] != NULL) && m_EffectTexture[index]->GetImageInfo().IsValidName(subsetOrSequenceName));
	if (ok)
	{
		m_EffectSubsetOrSequenceName[index] = subsetOrSequenceName;
	}
	return ok;
}

void MkPanel::_UpdateEffectUV(unsigned int index, double currentTimeStamp)
{
	const MkImageInfo::Subset* ssPtr =
		(m_EffectTexture[index] == NULL) ? NULL : m_EffectTexture[index]->GetImageInfo().GetCurrentSubsetPtr(m_EffectSubsetOrSequenceName[index], currentTimeStamp);

	if (ssPtr == NULL)
	{
		m_EffectUV[index][MkFloatRect::eLeftTop] = MkFloat2::Zero;
		m_EffectUV[index][MkFloatRect::eRightTop] = MkFloat2(1.f, 0.f);
		m_EffectUV[index][MkFloatRect::eLeftBottom] = MkFloat2(0.f, 1.f);
		m_EffectUV[index][MkFloatRect::eRightBottom] = MkFloat2(1.f, 1.f);
	}
	else
	{
		memcpy_s(m_EffectUV[index], sizeof(MkFloat2) * MkFloatRect::eMaxPointName, ssPtr->uv, sizeof(MkFloat2) * MkFloatRect::eMaxPointName);

		switch (index)
		{
		case 0: m_ShaderEffectSetting->SetTexture1(m_EffectTexture[index].GetPtr()); break;
		case 1: m_ShaderEffectSetting->SetTexture2(m_EffectTexture[index].GetPtr()); break;
		case 2: m_ShaderEffectSetting->SetTexture3(m_EffectTexture[index].GetPtr()); break;
		}
	}
}

void MkPanel::_FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkByteArray& buffer) const
{
	MkArray<float> data;

	if (__IsShaderEffectApplied())
	{
		if (m_EffectTexture[2] != NULL)
		{
			data.Fill(11);
		}
		else if (m_EffectTexture[1] != NULL)
		{
			data.Fill(9);
		}
		else if (m_EffectTexture[0] != NULL)
		{
			data.Fill(7);
		}
	}

	if (data.Empty())
	{
		data.Fill(5);
	}

	const MkFloat2& wv = m_WorldVertice[pn];
	data[0] = wv.x;
	data[1] = wv.y;
	data[2] = m_Transform.GetWorldDepth();

	const MkFloat2& uv = m_UV[pn];
	data[3] = (hr) ? (1.f - uv.x) : uv.x;
	data[4] = (vr) ? (1.f - uv.y) : uv.y;

	if (data.GetSize() > 5)
	{
		const MkFloat2& uv1 = m_EffectUV[0][pn];
		data[5] = (hr) ? (1.f - uv1.x) : uv1.x;
		data[6] = (vr) ? (1.f - uv1.y) : uv1.y;

		if (data.GetSize() > 7)
		{
			const MkFloat2& uv2 = m_EffectUV[1][pn];
			data[7] = (hr) ? (1.f - uv2.x) : uv2.x;
			data[8] = (vr) ? (1.f - uv2.y) : uv2.y;

			if (data.GetSize() > 9)
			{
				const MkFloat2& uv3 = m_EffectUV[2][pn];
				data[9] = (hr) ? (1.f - uv3.x) : uv3.x;
				data[10] = (vr) ? (1.f - uv3.y) : uv3.y;
			}
		}
	}

	MkByteArrayHelper<float>::PushBack(buffer, data);
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