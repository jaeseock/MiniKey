#pragma once


//------------------------------------------------------------------------------------------------//
// MaterialKey의 main key는 texture ptr, sub key는 object alpha. main key가 NULL이면 그려지지 않음
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

	// source의 크기가 panel의 크기보다 작을 때의 동작
	enum eSmallerSourceOp
	{
		eReducePanel = 0, // source의 크기에 맞게 panel을 축소해 맞춤. Update()시 panel size 자동계산
		eExpandSource, // panel 크기에 맞게 source를 확대해 맞춤. Update()전 panel size 사전입력 필요
		eAttachToLeftTop // panel과 source 크기를 모두 유지. panel의 left-top을 기준으로 출력. Update()전 panel size 사전입력 필요
	};

	// source의 크기가 panel의 크기보다 클 때의 동작
	enum eBiggerSourceOp
	{
		eExpandPanel = 0, // source의 크기에 맞게 panel을 확대해 맞춤. Update()시 panel size 자동계산
		eReduceSource, // panel 크기에 맞게 source를 축소해 맞춤. Update()전 panel size 사전입력 필요
		eCutSource // source를 panel 크기에 맞게 일부만 잘라 보여줌. pixel position 지정 가능. Update()전 panel size 사전입력 필요
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
	// panel size, uv op & resizing(image source와 panel 크기가 다를 경우의 처리)
	//------------------------------------------------------------------------------------------------//

	// panel resizing op : src is smaller than panel
	void SetSmallerSourceOp(eSmallerSourceOp op);
	eSmallerSourceOp GetSmallerSourceOp(void) const;

	// panel resizing op : src is bigger than panel
	void SetBiggerSourceOp(eBiggerSourceOp op);
	eBiggerSourceOp GetBiggerSourceOp(void) const;

	// panel size
	void SetPanelSize(const MkFloat2& size); // resizing type이 eFollowSource일 때는 의미 없음
	inline const MkFloat2& GetPanelSize(void) const { return m_PanelSize; }

	// panel pixel scroll position
	// resizing type이 eExpandOrCut이고 source 크기가 panel보다 클 경우(cut) 상황에만 동작
	// top-down coordinate. (0, 0)이 left-top을 의미
	inline void SetPixelScrollPosition(const MkFloat2& position) { m_PixelScrollPosition = position; }
	inline const MkFloat2& GetPixelScrollPosition(void) const { return m_PixelScrollPosition; }

	// uv reflection. default는 false
	void SetHorizontalReflection(bool enable);
	bool GetHorizontalReflection(void) const;

	void SetVerticalReflection(bool enable);
	bool GetVerticalReflection(void) const;

	// visible. default는 true
	void SetVisible(bool visible);
	bool GetVisible(void) const;

	//------------------------------------------------------------------------------------------------//
	// 그려질 대상 설정
	// - texture(static/dynamic)
	// - text node
	// - masking scene node
	//------------------------------------------------------------------------------------------------//

	// texture 설정
	// sequence 지정의 경우 startTime은 현재 시간, initTime은 sequence 내부에서의 시작 시간을 의미
	// (NOTE) SetTextNode(), SetMaskingNode()와 배타적
	bool SetTexture(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double timeOffset = 0.);
	bool SetTexture(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double timeOffset = 0.);

	// image info
	// (NOTE) 호출 전 texture가 설정되어 있어야 함
	bool SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName, double timeOffset = 0.);
	inline const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }
	
	inline void SetSequenceTimeOffset(double offset) { m_SequenceTimeOffset = offset; }
	inline double GetSequenceTimeOffset(void) const { return m_SequenceTimeOffset; }

	// 현재 지정된 sequence의 진행도(0. ~ 1.)를 반환
	float GetSequenceProgress(double currTime, bool ignoreLoop = false) const;

	// text node 설정
	// 호출시 Build() 수행
	// MkDrawTextNodeStep을 통해 static image를 caching해 사용하므로 draw시 최초 한 번만 부하 발생
	// deep copy로 자체적인 객체를 소유. 이는 GetTextNodePtr()로 얻은 객체의 변경이 원본 손상 없이 가능함을 의미(수정 후 추가적인 Build() 필요)
	// restrictToPanelWidth가 true인 경우 panel size의 x로 가로 폭 제한
	// (NOTE) SetTexture(), SetMaskingNode()와 배타적
	// (NOTE) 저장시 text node 내용을 그대로 출력. 용량이 크기 때문에 수정 내용을 반영할 필요가 없다면 가급적 이름으로 사용하길 권장
	// (TIP) 장문의 경우 restrictToPanelWidth = true, eAttachToLeftTop, eCutSource와 조합하면 일반적인 세로 스크롤 출력이 됨
	void SetTextNode(const MkTextNode& source, bool restrictToPanelWidth = false);

	// 해당 이름의 text node를 static resource container에서 불러와 설정
	// (NOTE) static으로 여겨지기 때문에 GetTextNodePtr()로 얻어온 복사본을 수정하더라도 저장시 수정 내용이 반영되지 않음
	void SetTextNode(const MkArray<MkHashStr>& name, bool restrictToPanelWidth = false);

	// 기본 스타일로 msg를 표기. deco 기능은 전혀 없지만 간편함
	void SetTextMsg(const MkStr& msg, bool restrictToPanelWidth = false);

	// 설정된 text node 정보 반환
	inline const MkArray<MkHashStr>& GetTextNodeName(void) const { return m_TargetTextNodeName; }
	inline MkTextNode* GetTextNodePtr(void) { return m_TargetTextNodePtr; }
	inline const MkTextNode* GetTextNodePtr(void) const { return m_TargetTextNodePtr; }

	// GetTextNodePtr()로 얻어낸 text node 수정이 발생했거나, panel size가 변경되었을 경우 반영을 위해 호출
	// ex> sample test node는 "MkPA_MkTextNode.h"의 헤더 부분 참조
	//	MkPanel* panel = sceneNode->GetPanel(L"Test");
	//	MkTextNode* textNode = panel->GetTextNodePtr();
	//	MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"이번이구나");
	//	targetNode->SetFontStyle(L"Desc:Notice");
	//	targetNode->SetText(L"- 이걸로 바꿨음당 ( ㅡ_-)r");
	//	panel->BuildAndUpdateTextCache();
	void BuildAndUpdateTextCache(void);

	// mask panel 설정
	// (NOTE) SetTexture(), SetTextNode()와 배타적
	// (NOTE) 호출 전 반드시 유효한 panel size가 설정되어 있어야 함
	void SetMaskingNode(const MkSceneNode* sceneNode);

	// 그려질 대상(image, text, mask)의 크기를 반환
	inline const MkFloat2& GetTextureSize(void) const { return m_TextureSize; }

	// 텍스쳐 참조 반환
	inline const MkBaseTexture* GetTexturePtr(void) const { return m_Texture.GetPtr(); }

	//------------------------------------------------------------------------------------------------//
	// shader effect
	//------------------------------------------------------------------------------------------------//

	// effect 설정
	// (NOTE) 변경시 기존 설정되어 있던 technique, texture, UDP값은 모두 삭제 됨
	bool SetShaderEffect(const MkHashStr& name);
	inline const MkHashStr& GetShaderEffect(void) const { return m_ShaderEffectName; }

	// effect에 복수의 technique이 존재 할 경우 대상 technique 지정
	bool SetTechnique(const MkHashStr& name);
	const MkHashStr& GetTechnique(void) const;

	// effect용 texture 설정
	// 기본 texture가 0번으로(TEXTURE0) 인식 됨
	// sequence에 적용되는 time은 기본 texture의 것을 그대로 사용
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

	// UDP 설정. shader에 정의된 변수명과 data type(float, float2~4)이 동일해야 함
	bool SetUserDefinedProperty(const MkHashStr& name, float x);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z);
	bool SetUserDefinedProperty(const MkHashStr& name, float x, float y, float z, float w);

	bool GetUserDefinedProperty(const MkHashStr& name, float& x) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z) const;
	bool GetUserDefinedProperty(const MkHashStr& name, float& x, float& y, float& z, float& w) const;

	// effect 해제
	// (NOTE) 기존 설정되어 있던 technique, texture, UDP값도 모두 삭제 됨
	void ClearShaderEffect(void);

	//------------------------------------------------------------------------------------------------//

	inline MkSceneNode* GetParentNode(void) const { return m_ParentNode; }

	void ClearMainTexture(void);

	// 해제
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

		// 출력 여부
		eVisible = 4, // eBiggerSourceOpPosition + eBiggerSourceOpBandwidth,

		// uv 반전
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
