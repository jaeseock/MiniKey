#pragma once


//------------------------------------------------------------------------------------------------//
// MaterialKey�� main key�� texture ptr, sub key�� object alpha. main key�� NULL�̸� �׷����� ����
//
// ex>
//	Node "ImgTest" : "MkPanel"
//	{
//		uint Attribute = 18;
//		str ImagePath = "Image\s01.jpg";
//		float LocalDepth = 9999.000;
//		vec2 PanelSize = (909.000, 505.000);
//	}
//
//	Node "P" : "MkPanel"
//	{
//		str ImagePath = "Image\rohan_screenshot.png";
//		vec2 PanelSize = (1024.000, 768.000);
//
//		Node "ShaderEffect"
//		{
//			str EffectName = "ttt";
//			str ImagePath = "Image\s01.jpg";
//			str SOSName = "VerticalChange";
//
//			Node "UDP"
//			{
//				float blendingWeight = // [4]
//					1.000 / 1.000 / 1.000 / 0.000;
//			}
//		}
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBitField32.h"
#include "MkPA_MkSceneTransform.h"
#include "MkPA_MkBaseTexture.h"
#include "MkPA_MkSceneObject.h"


class MkDrawStepInterface;
class MkDrawTextNodeStep;
class MkTextNode;
class MkSceneNode;
class MkShaderEffectSetting;

class MkPanel : public MkSceneRenderObject
{
public:

	// source�� ũ�Ⱑ panel�� ũ�⺸�� ���� ���� ����
	enum eSmallerSourceOp
	{
		eReducePanel = 0, // source�� ũ�⿡ �°� panel�� ����� ����. Update()�� panel size �ڵ����
		eExpandSource, // panel ũ�⿡ �°� source�� Ȯ���� ����. Update()�� panel size �����Է� �ʿ�
		eAttachToLeftTop // panel�� source ũ�⸦ ��� ����. panel�� left-top�� �������� ���. Update()�� panel size �����Է� �ʿ�
	};

	// source�� ũ�Ⱑ panel�� ũ�⺸�� Ŭ ���� ����
	enum eBiggerSourceOp
	{
		eExpandPanel = 0, // source�� ũ�⿡ �°� panel�� Ȯ���� ����. Update()�� panel size �ڵ����
		eReduceSource, // panel ũ�⿡ �°� source�� ����� ����. Update()�� panel size �����Է� �ʿ�
		eCutSource // source�� panel ũ�⿡ �°� �Ϻθ� �߶� ������. pixel position ���� ����. Update()�� panel size �����Է� �ʿ�
	};

public:

	virtual ePA_SceneObjectType GetObjectType(void) const { return ePA_SOT_Panel; }

	//------------------------------------------------------------------------------------------------//
	// transform
	//------------------------------------------------------------------------------------------------//

	// local
	inline void SetLocalPosition(const MkFloat2& position) { m_Transform.SetLocalPosition(position); }
	inline const MkFloat2& GetLocalPosition(void) const { return m_Transform.GetLocalPosition(); }

	inline void SetLocalDepth(float depth) { m_Transform.SetLocalDepth(depth); }
	inline float GetLocalDepth(void) const { return m_Transform.GetLocalDepth(); }

	inline void SetLocalRotation(float rotation) { m_Transform.SetLocalRotation(rotation); }
	inline float GetLocalRotation(void) const { return m_Transform.GetLocalRotation(); }

	inline void SetLocalScale(float scale) { m_Transform.SetLocalScale(scale); }
	inline float GetLocalScale(void) const { return m_Transform.GetLocalScale(); }

	inline void SetLocalAlpha(float alpha) { m_Transform.SetLocalAlpha(alpha); }
	inline float GetLocalAlpha(void) const { return m_Transform.GetLocalAlpha(); }

	// clear local
	inline void ClearLocalTransform(void) { m_Transform.ClearLocalTransform(); }

	// world
	inline const MkFloat2& GetWorldPosition(void) const { return m_Transform.GetWorldPosition(); }
	inline float GetWorldDepth(void) const { return m_Transform.GetWorldDepth(); }
	inline float GetWorldRotation(void) const { return m_Transform.GetWorldRotation(); }
	inline float GetWorldScale(void) const { return m_Transform.GetWorldScale(); }
	inline float GetWorldAlpha(void) const { return m_Transform.GetWorldAlpha(); }

	//------------------------------------------------------------------------------------------------//
	// panel size, uv op & resizing(image source�� panel ũ�Ⱑ �ٸ� ����� ó��)
	//------------------------------------------------------------------------------------------------//

	// panel resizing op : src is smaller than panel
	void SetSmallerSourceOp(eSmallerSourceOp op);
	eSmallerSourceOp GetSmallerSourceOp(void) const;

	// panel resizing op : src is bigger than panel
	void SetBiggerSourceOp(eBiggerSourceOp op);
	eBiggerSourceOp GetBiggerSourceOp(void) const;

	// panel size
	void SetPanelSize(const MkFloat2& size); // resizing type�� eFollowSource�� ���� �ǹ� ����
	inline const MkFloat2& GetPanelSize(void) const { return m_PanelSize; }

	// panel pixel scroll position
	// resizing type�� eExpandOrCut�̰� source ũ�Ⱑ panel���� Ŭ ���(cut) ��Ȳ���� ����
	// top-down coordinate. (0, 0)�� left-top�� �ǹ�
	inline void SetPixelScrollPosition(const MkFloat2& position) { m_PixelScrollPosition = position; }
	inline const MkFloat2& GetPixelScrollPosition(void) const { return m_PixelScrollPosition; }

	// uv reflection. default�� false
	void SetHorizontalReflection(bool enable);
	bool GetHorizontalReflection(void) const;

	void SetVerticalReflection(bool enable);
	bool GetVerticalReflection(void) const;

	// visible. default�� true
	void SetVisible(bool visible);
	bool GetVisible(void) const;

	//------------------------------------------------------------------------------------------------//
	// �׷��� ��� ����
	// - texture(static/dynamic)
	// - text node
	// - masking scene node
	//------------------------------------------------------------------------------------------------//

	// texture ����
	// sequence ������ ��� startTime�� ���� �ð�, initTime�� sequence ���ο����� ���� �ð��� �ǹ�
	// (NOTE) SetTextNode(), SetMaskingNode()�� ��Ÿ��
	bool SetTexture(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double timeOffset = 0.);
	bool SetTexture(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double timeOffset = 0.);

	// image info
	// (NOTE) ȣ�� �� texture�� �����Ǿ� �־�� ��
	bool SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName, double timeOffset = 0.);
	inline const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }
	
	inline void SetSequenceTimeOffset(double offset) { m_SequenceTimeOffset = offset; }
	inline double GetSequenceTimeOffset(void) const { return m_SequenceTimeOffset; }

	// ���� ������ sequence�� ���൵(0. ~ 1.)�� ��ȯ
	float GetSequenceProgress(double currTime, bool ignoreLoop = false) const;

	// text node ����
	// ȣ��� Build() ����
	// MkDrawTextNodeStep�� ���� static image�� caching�� ����ϹǷ� draw�� ���� �� ���� ���� �߻�
	// deep copy�� ��ü���� ��ü�� ����. �̴� GetTextNodePtr()�� ���� ��ü�� ������ ���� �ջ� ���� �������� �ǹ�(���� �� �߰����� Build() �ʿ�)
	// restrictToPanelWidth�� true�� ��� panel size�� x�� ���� �� ����
	// (NOTE) SetTexture(), SetMaskingNode()�� ��Ÿ��
	// (NOTE) ����� text node ������ �״�� ���. �뷮�� ũ�� ������ ���� ������ �ݿ��� �ʿ䰡 ���ٸ� ������ �̸����� ����ϱ� ����
	// (TIP) �幮�� ��� restrictToPanelWidth = true, eAttachToLeftTop, eCutSource�� �����ϸ� �Ϲ����� ���� ��ũ�� ����� ��
	void SetTextNode(const MkTextNode& source, bool restrictToPanelWidth = false);

	// �ش� �̸��� text node�� static resource container���� �ҷ��� ����
	// (NOTE) static���� �������� ������ GetTextNodePtr()�� ���� ���纻�� �����ϴ��� ����� ���� ������ �ݿ����� ����
	void SetTextNode(const MkArray<MkHashStr>& name, bool restrictToPanelWidth = false);

	// �⺻ ��Ÿ�Ϸ� msg�� ǥ��. deco ����� ���� ������ ������
	void SetTextMsg(const MkStr& msg, bool restrictToPanelWidth = false);

	// ������ text node ���� ��ȯ
	inline const MkArray<MkHashStr>& GetTextNodeName(void) const { return m_TargetTextNodeName; }
	inline MkTextNode* GetTextNodePtr(void) { return m_TargetTextNodePtr; }
	inline const MkTextNode* GetTextNodePtr(void) const { return m_TargetTextNodePtr; }

	// GetTextNodePtr()�� �� text node ������ �߻��߰ų�, panel size�� ����Ǿ��� ��� �ݿ��� ���� ȣ��
	// ex> sample test node�� "MkPA_MkTextNode.h"�� ��� �κ� ����
	//	MkPanel* panel = sceneNode->GetPanel(L"Test");
	//	MkTextNode* textNode = panel->GetTextNodePtr();
	//	MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"�̹��̱���");
	//	targetNode->SetFontStyle(L"Desc:Notice");
	//	targetNode->SetText(L"- �̰ɷ� �ٲ����� ( ��_-)r");
	//	panel->BuildAndUpdateTextCache();
	void BuildAndUpdateTextCache(void);

	// mask panel ����
	// (NOTE) SetTexture(), SetTextNode()�� ��Ÿ��
	// (NOTE) ȣ�� �� �ݵ�� ��ȿ�� panel size�� �����Ǿ� �־�� ��
	void SetMaskingNode(const MkSceneNode* sceneNode);

	// �׷��� ���(image, text, mask)�� ũ�⸦ ��ȯ
	inline const MkFloat2& GetTextureSize(void) const { return m_TextureSize; }

	// �ؽ��� ���� ��ȯ
	inline const MkBaseTexture* GetTexturePtr(void) const { return m_Texture.GetPtr(); }

	//------------------------------------------------------------------------------------------------//
	// shader effect
	//------------------------------------------------------------------------------------------------//

	// effect ����
	// (NOTE) ����� ���� �����Ǿ� �ִ� technique, texture, UDP���� ��� ���� ��
	bool SetShaderEffect(const MkHashStr& name);
	inline const MkHashStr& GetShaderEffect(void) const { return m_ShaderEffectName; }

	// effect�� ������ technique�� ���� �� ��� ��� technique ����
	bool SetTechnique(const MkHashStr& name);
	const MkHashStr& GetTechnique(void) const;

	// effect�� texture ����
	// �⺻ texture�� 0������(TEXTURE0) �ν� ��
	// sequence�� ����Ǵ� time�� �⺻ texture�� ���� �״�� ���
	bool SetEffectTexture1(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);
	bool SetEffectTexture1(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);
	bool SetEffectTexture2(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);
	bool SetEffectTexture2(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);
	bool SetEffectTexture3(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);
	bool SetEffectTexture3(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);

	const MkBaseTexture* GetEffectTexturePtr1(void) const;
	const MkBaseTexture* GetEffectTexturePtr2(void) const;
	const MkBaseTexture* GetEffectTexturePtr3(void) const;

	bool SetEffectSubsetOrSequenceName1(const MkHashStr& subsetOrSequenceName);
	bool SetEffectSubsetOrSequenceName2(const MkHashStr& subsetOrSequenceName);
	bool SetEffectSubsetOrSequenceName3(const MkHashStr& subsetOrSequenceName);

	inline const MkHashStr& GetEffectSubsetOrSequenceName1(void) const { return m_EffectSubsetOrSequenceName[0]; }
	inline const MkHashStr& GetEffectSubsetOrSequenceName2(void) const { return m_EffectSubsetOrSequenceName[1]; }
	inline const MkHashStr& GetEffectSubsetOrSequenceName3(void) const { return m_EffectSubsetOrSequenceName[2]; }

	// UDP ����. shader�� ���ǵ� ������� data type(float, float2~4)�� �����ؾ� ��
	bool SetUserDefinedProperty(const MkHashStr& name, float x);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z, float w);

	bool GetUserDefinedProperty(const MkHashStr& name, float& x) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z, float& w) const;

	// effect ����
	// (NOTE) ���� �����Ǿ� �ִ� technique, texture, UDP���� ��� ���� ��
	void ClearShaderEffect(void);

	//------------------------------------------------------------------------------------------------//

	inline MkSceneNode* GetParentNode(void) const { return m_ParentNode; }

	void ClearMainTexture(void);

	// ����
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// MkSceneRenderObject
	//------------------------------------------------------------------------------------------------//

	virtual bool __CheckDrawable(void) const;
	virtual void __ExcuteCustomDrawStep(void);
	virtual void __FillVertexData(MkByteArray& buffer) const;
	virtual void __ApplyRenderState(void) const;
	virtual bool __IsShaderEffectApplied(void) const;
	virtual void __DrawWithShaderEffect(LPDIRECT3DDEVICE9 device) const;

	virtual void Load(const MkDataNode& node);
	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

public:

	//------------------------------------------------------------------------------------------------//

	inline void __SetParentNode(MkSceneNode* parentNode) { m_ParentNode = parentNode; }

	void __Update(const MkSceneTransform* parentTransform, double currTime);

	bool __CheckWorldIntersection(const MkFloat2& worldPoint) const;

	MkPanel(void);
	virtual ~MkPanel() { Clear(); }

protected:

	enum eAttribute
	{
		// eSmallerSourceOp(value)
		eSmallerSourceOpPosition = 0,
		eSmallerSourceOpBandwidth = 2, // eSmallerSourceOp(0 ~ 2)

		// eBiggerSourceOp(value)
		eBiggerSourceOpPosition = 2, // eSmallerSourceOpPosition + eSmallerSourceOpBandwidth,
		eBiggerSourceOpBandwidth = 2, // eBiggerSourceOp(0 ~ 2)

		// ��� ����
		eVisible = 4, // eBiggerSourceOpPosition + eBiggerSourceOpBandwidth,

		// uv ����
		eHorizontalReflection, // u
		eVerticalReflection // v
	};

	bool _SetEffectTexture(unsigned int index, const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName);
	bool _SetEffectSubsetOrSequenceName(unsigned int index, const MkHashStr& subsetOrSequenceName);
	void _UpdateEffectUV(unsigned int index, double currentTimeStamp);

	void _FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkByteArray& buffer) const;
	float _GetCrossProduct(MkFloatRect::ePointName from, MkFloatRect::ePointName to, const MkFloat2& point) const;
	void _UpdateTextNodeTexture(MkDrawTextNodeStep* drawStep);

protected:

	// parent
	MkSceneNode* m_ParentNode;

	// flag
	MkBitField32 m_Attribute;

	// transform
	MkSceneTransform m_Transform;

	// panel size
	MkFloat2 m_PanelSize;
	MkFloat2 m_PixelScrollPosition;
	
	// transform result
	MkFloat2 m_WorldVertice[MkFloatRect::eMaxPointName];
	MkFloat2 m_UV[MkFloatRect::eMaxPointName];
	
	// texture
	MkBaseTexturePtr m_Texture;
	MkHashStr m_SubsetOrSequenceName;
	MkFloat2 m_TextureSize;
	double m_SequenceStartTime;
	double m_SequenceTimeOffset;

	MkBaseTexturePtr m_EffectTexture[3];
	MkHashStr m_EffectSubsetOrSequenceName[3];
	MkFloat2 m_EffectUV[3][MkFloatRect::eMaxPointName];
	
	// text node
	MkArray<MkHashStr> m_TargetTextNodeName;
	MkTextNode* m_TargetTextNodePtr;

	// custom draw step
	MkDrawStepInterface* m_DrawStep;

	// shader effect
	MkHashStr m_ShaderEffectName;
	MkShaderEffectSetting* m_ShaderEffectSetting;

public:

	static const MkHashStr ObjKey_Attribute;
	static const MkHashStr ObjKey_PanelSize;
	static const MkHashStr ObjKey_PixelScrollPosition;
	static const MkHashStr ObjKey_ImagePath;
	static const MkHashStr ObjKey_SubsetOrSequenceName;
	static const MkHashStr ObjKey_SequenceTimeOffset;
	static const MkHashStr ObjKey_TextNodeName;
	static const MkHashStr ObjKey_TextNodeData;
	static const MkHashStr ObjKey_TextNodeWidthRestriction;
	static const MkHashStr ObjKey_ShaderEffect;
		static const MkHashStr ObjKey_EffectName;
		// ObjKey_ImagePath
		// ObjKey_SubsetOrSequenceName
		static const MkHashStr ObjKey_EffectUDP;
};
