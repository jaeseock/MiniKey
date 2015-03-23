#pragma once


//------------------------------------------------------------------------------------------------//
// 구성용 data node 샘플
//
//	Node "SampleRect" : "T_SRect" // 안전을 위해 MKDEF_S2D_BT_SRECT_TEMPLATE_NAME 상속 권장
//	{
//		uint Alpha = 128;
//		float Depth = 0.000;
//		vec2 Position = (100.000, 200.000);
//		bool Reflection = // [2]
//			Yes / No;
//		//str Resource = "map" / "Image\s03.jpg" / ""; // 이미지 출력
//		str Resource = "odt" / "<%T:굴림12%><%S:샘플02%>샘플 메세지다요"; // original deco text 출력
//		//str Resource = "sdt" / "SampleCategory" / "ErrorMsg" / "SampleMsg"; // scene deco text 출력
//		bool Visible = Yes;
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBitField32.h"
#include "MkPA_MkSceneTransform.h"
#include "MkPA_MkBaseTexture.h"


class MkDrawStepInterface;
class MkTextNode;
class MkSceneNode;

class MkPanel
{
public:

	//enum eSrcType
	//{
	//	eNone = 0,
	//	eStaticImage,
	//	eRenderToTexture
	//};

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

	// MkDataNode로 구성. 기존 설정값이 존재하면 덮어씀
	//void Load(const MkDataNode& node);

	// MkDataNode로 출력
	//void Save(MkDataNode& node);

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

	// get axis-aligned minimum bounding rect
	inline const MkFloatRect& GetWorldAABR(void) const { return m_AABR; }

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
	inline void SetPanelSize(const MkFloat2& size) { m_PanelSize = size; } // resizing type이 eFollowSource일 때는 의미 없음
	inline const MkFloat2& GetPanelSize(void) const { return m_PanelSize; }

	// panel pixel scroll position
	// resizing type이 eExpandOrCut이고 source 크기가 panel보다 클 경우(cut) 상황에만 동작
	// top-down coordinate. (0, 0)이 left-top을 의미
	inline void SetPixelScrollPosition(const MkFloat2& position) { m_PixelScrollPosition = position; }
	inline const MkFloat2& GetPixelScrollPosition(void) const { return m_PixelScrollPosition; }

	// uv reflection
	void SetHorizontalReflection(bool enable);
	bool GetHorizontalReflection(void) const;

	void SetVerticalReflection(bool enable);
	bool GetVerticalReflection(void) const;

	// visible
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
	bool SetTexture(const MkBaseTexture* texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double startTime = 0., double initTime = 0.);
	bool SetTexture(const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double startTime = 0., double initTime = 0.);

	// image info
	// (NOTE) 호출 전 texture가 설정되어 있어야 함
	bool SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName, double startTime = 0., double initTime = 0.);
	inline const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }
	unsigned int GetAllSubsets(MkArray<MkHashStr>& keyList) const;
	unsigned int GetAllSequences(MkArray<MkHashStr>& keyList) const;
	inline double GetSequenceStartTime(void) const { return m_SequenceStartTime; }

	// text node 설정
	// 호출시 Build() 수행
	// MkDrawTextNodeStep을 통해 static image를 caching해 사용하므로 draw시 최초 한 번만 부하 발생
	// deep copy로 자체적인 객체를 소유. 이는 GetTextNodePtr()로 얻은 객체의 변경이 원본 손상 없이 가능함을 의미(수정 후 추가적인 Build() 필요)
	// restrictToPanelWidth가 true인 경우 panel size의 x로 가로 폭 제한
	// (NOTE) SetTexture(), SetMaskingNode()와 배타적
	// (NOTE) 기존 GetTextNodePtr() / BuildAndUpdateTextCache()로 세팅된 휘발성 정보는 모두 사라짐
	// (TIP) 장문의 경우 restrictToPanelWidth = true, eAttachToLeftTop, eCutSource와 조합하면 일반적인 세로 스크롤 출력이 됨
	void SetTextNode(const MkTextNode& source, bool restrictToPanelWidth = false);

	// 해당 이름의 text node를 static resource container에서 불러와 설정
	// (NOTE) static으로 여겨지기 때문에 GetTextNodePtr()로 얻어온 복사본을 수정하더라도 저장시 수정 내용이 반영되지 않음
	void SetTextNode(const MkArray<MkHashStr>& name, bool restrictToPanelWidth = false);

	// 설정된 text node 정보 반환
	inline const MkArray<MkHashStr>& GetTextNodeName(void) const { return m_TargetTextNodeName; }
	inline MkTextNode* GetTextNodePtr(void) { return m_TargetTextNodePtr; }
	inline const MkTextNode* GetTextNodePtr(void) const { return m_TargetTextNodePtr; }

	// GetTextNodePtr()로 얻어낸 text node 수정 후 반영을 위해 호출
	// ex> sample test node는 "MkPA_MkTextNode.h"의 헤더 부분 참조
	//	MkPanel* panel = sceneNode->GetPanel(L"Test");
	//	MkTextNode* textNode = panel->GetTextNodePtr();
	//	MkTextNode* targetNode = textNode->GetChildNode(L"1st")->GetChildNode(L"Sub list")->GetChildNode(L"이번이구나");
	//	targetNode->SetFontStyle(L"Desc:Notice");
	//	targetNode->SetText(L"- 이걸로 바꿨음당 ( ㅡ_-)r");
	//	panel->BuildAndUpdateTextCache();
	void BuildAndUpdateTextCache(void);

	// 기존 설정된 deco string이 있다면 재로딩
	//void RestoreDecoString(void);

	// mask panel 설정
	// (NOTE) SetTexture(), SetTextNode()와 배타적
	// (NOTE) 호출 전 반드시 유효한 panel size가 설정되어 있어야 함
	void SetMaskingNode(const MkSceneNode* sceneNode);

	// 그려질 대상(image, text, mask)의 크기를 반환
	inline const MkFloat2& GetTextureSize(void) const { return m_TextureSize; }

	//------------------------------------------------------------------------------------------------//

	inline MkSceneNode* GetParentNode(void) const { return m_ParentNode; }

	// 정렬
	//void AlignRect(const MkFloat2& anchorSize, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset);

	// 해제
	void Clear(void);

public:

	//------------------------------------------------------------------------------------------------//

	typedef struct _VertexData
	{
		float x, y, z, u, v;
	}
	VertexData;

	class MaterialKey // std::map의 key가 될 수 있는 최소 조건(operator <) 만족
	{
	public:
		inline bool operator == (const MaterialKey& key) const { return ((m_TextureID == key.m_TextureID) && (m_ObjectAlpha == key.m_ObjectAlpha)); }
		inline bool operator < (const MaterialKey& key) const { return ((m_TextureID < key.m_TextureID) ? true : ((m_TextureID == key.m_TextureID) ? (m_ObjectAlpha < key.m_ObjectAlpha) : false)); }

		MaterialKey()
		{
			m_TextureID = 0;
			m_ObjectAlpha = 0xff;
		}

		ID64 m_TextureID;
		DWORD m_ObjectAlpha;
	};

	inline void __SetParentNode(MkSceneNode* parentNode) { m_ParentNode = parentNode; }

	//static void __GenerateBuildingTemplate(void);

	// 활성화 여부 반환
	bool __CheckDrawable(void) const;
	bool __CheckDrawable(const MkFloatRect& cameraAABR) const;

	inline const MaterialKey& __GetMaterialKey(void) const { return m_MaterialKey; }

	void __ExcuteCustomDrawStep(void);

	void __Update(const MkSceneTransform* parentTransform, double currTime);

	void __FillVertexData(MkArray<VertexData>& buffer) const;

	void __AffectTexture(void) const;

	bool __CheckWorldIntersection(const MkFloat2& worldPoint) const;

	//eSrcType __GetSrcInfo(MkPathName& imagePath, MkHashStr& subsetName, MkStr& decoStr, MkArray<MkHashStr>& nodeNameAndKey) const;

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

	void _FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkArray<VertexData>& buffer) const;

	float _GetCrossProduct(MkFloatRect::ePointName from, MkFloatRect::ePointName to, const MkFloat2& point) const;

protected:

	// parent
	MkSceneNode* m_ParentNode;

	// transform
	MkSceneTransform m_Transform;

	// panel size
	MkFloat2 m_PanelSize;
	MkFloat2 m_PixelScrollPosition;
	
	// transform result
	MkFloat2 m_WorldVertice[MkFloatRect::eMaxPointName];
	MkFloat2 m_UV[MkFloatRect::eMaxPointName];
	MkFloatRect m_AABR; // axis-aligned bounding rect

	// texture
	MkBaseTexturePtr m_Texture;
	MkHashStr m_SubsetOrSequenceName;
	double m_SequenceStartTime;
	double m_SequenceInitTime;
	MaterialKey m_MaterialKey;
	MkFloat2 m_TextureSize;

	// text node
	MkArray<MkHashStr> m_TargetTextNodeName;
	MkTextNode* m_TargetTextNodePtr;

	// masking node
	const MkSceneNode* m_TargetMaskingNodePtr;

	// custom draw step
	MkDrawStepInterface* m_DrawStep;

	// flag
	MkBitField32 m_Attribute;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPanel::VertexData)
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPanel::MaterialKey)
