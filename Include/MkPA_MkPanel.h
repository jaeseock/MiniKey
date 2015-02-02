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

#include "MkCore_MkBitFieldDW.h"
#include "MkPA_MkSceneTransform.h"
#include "MkPA_MkBaseTexture.h"
//#include "MkS2D_MkTextCacheStep.h"


class MkPanel
{
public:

	enum eSrcType
	{
		eNone = 0,
		eStaticImage,
		eRenderToTexture
	};

	enum eResizingType // source와 panel의 크기가 다를 때의 동작
	{
		eFollowSource = 0, // source의 크기에 panel을 맞춤
		eExpandOrReduce, // panel 크기에 source를 확대/축소해 맞춤
		eExpandOrCut // source가 panel보다 작을 경우 확대, 클 경우 일부만 잘라 보여줌. offset(scrolling) 지정 가능
	};

	enum eAttribute
	{
		// 출력 여부
		eVisible = 1,

		// uv 반전
		eHorizontalReflection = 1 << 1, // u
		eVerticalReflection = 1 << 2 // v
	};

public:

	// MkDataNode로 구성. 기존 설정값이 존재하면 덮어씀
	//void Load(const MkDataNode& node);

	// MkDataNode로 출력
	//void Save(MkDataNode& node);

	// transform
	inline MkSceneTransform& GetTransform(void) { return m_Transform; }
	inline const MkSceneTransform& GetTransform(void) const { return m_Transform; }

	// set & get object alpha(0.f ~ 1.f)
	void SetObjectAlpha(float alpha);
	float GetObjectAlpha(void) const;

	// panel resizing type
	inline void SetResizingType(eResizingType type) { m_ResizingType = type; }
	inline eResizingType GetResizingType(void) const { return m_ResizingType; }

	// panel size
	inline void SetPanelSize(const MkFloat2& size) { m_PanelSize = size; } // resizing type이 eFollowSource일 때는 의미 없음
	inline const MkFloat2& GetPanelSize(void) const { return m_PanelSize; }

	// panel scroll offset
	// resizing type이 eExpandOrCut이고 source 크기가 panel보다 클 경우(cut) 상황에만 동작
	inline void SetScrollOffset(const MkFloat2& offset) { m_ScrollOffset = offset; }
	inline const MkFloat2& GetScrollOffset(void) const { return m_ScrollOffset; }

	// get axis-aligned minimum bounding rect
	inline const MkFloatRect& GetAABR(void) const { return m_AABR; }

	// 강제 font state 설정. 이후 적용되는 모든 deco string은 해당 설정이 강제로 적용 됨
	//void SetFocedFontState(const MkHashStr& fontState);
	//inline bool CheckFocedFontTypeAndState(void) const { return (!m_ForcedFontState.Empty()); }

	// texture
	// (NOTE) SetDecoString()과 배타적. 기존 설정 된 original/scene deco text가 있을 경우 text 무효화
	void SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double initTime = 0.);
	void SetTexture(const MkPathName& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double initTime = 0.);

	// image info
	inline void SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName) { m_SubsetOrSequenceName = subsetOrSequenceName; }
	inline const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }
	unsigned int GetAllSubsets(MkArray<MkHashStr>& keyList) const;
	unsigned int GetAllSequences(MkArray<MkHashStr>& keyList) const;

	// original deco text 설정
	// deco string 문법으로 구성되어 있으면 그대로 적용되고, 일반 문자열이면 DSF가 적용되어 출력됨
	// (NOTE) SetTexture()와 배타적. 설정이 성공하면 캐쉬 텍스쳐가 기존 설정 된 texture를 교체(기존 texture 무효화)
	//bool SetDecoString(const MkStr& decoStr);

	// scene deco text table에서 nodeNameAndKey에 해당하는 deco text 설정
	// (NOTE) SetTexture()와 배타적. 설정이 성공하면 캐쉬 텍스쳐가 기존 설정 된 texture를 교체(기존 texture 무효화)
	//bool SetDecoString(const MkArray<MkHashStr>& nodeNameAndKey);

	// 설정된 deco text 반환
	//inline const MkDecoStr& GetDecoString(void) const { return m_TextCacheStep.GetDecoStr(); }

	// 기존 설정된 deco string이 있다면 재로딩
	//void RestoreDecoString(void);

	// uv reflection
	inline void SetHorizontalReflection(bool enable) { m_Attribute.Assign(eHorizontalReflection, enable); }
	inline bool GetHorizontalReflection(void) const { return m_Attribute[eHorizontalReflection]; }

	inline void SetVerticalReflection(bool enable) { m_Attribute.Assign(eVerticalReflection, enable); }
	inline bool GetVerticalReflection(void) const { return m_Attribute[eVerticalReflection]; }

	// visible
	inline void SetVisible(bool visible) { m_Attribute.Assign(eVisible, visible); }
	inline bool GetVisible(void) const { return m_Attribute[eVisible]; }

	// 그려질 수 있는지 여부 반환
	bool CheckDrawable(const MkFloatRect& cameraAABR) const;

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
			m_ObjectAlpha = 0;
		}

		ID64 m_TextureID;
		DWORD m_ObjectAlpha;
	};

	//static void __GenerateBuildingTemplate(void);

	inline const MaterialKey& __GetMaterialKey(void) const { return m_MaterialKey; }

	//void __GenerateTextCache(void);

	void __UpdateTransform(const MkSceneTransform* parentTransform, double elapsed);

	void __FillVertexData(MkArray<VertexData>& buffer) const;

	void __AffectTexture(void) const;

	//eSrcType __GetSrcInfo(MkPathName& imagePath, MkHashStr& subsetName, MkStr& decoStr, MkArray<MkHashStr>& nodeNameAndKey) const;

	MkPanel();
	virtual ~MkPanel() { Clear(); }

protected:

	void _FillVertexData(MkFloatRect::ePointName pn, bool hr, bool vr, MkArray<VertexData>& buffer) const;

	//bool _SetDecoString(const MkDecoStr& decoStr);

protected:

	// transform
	MkSceneTransform m_Transform;

	// panel size
	eResizingType m_ResizingType;
	MkFloat2 m_PanelSize;
	MkFloat2 m_ScrollOffset;
	
	// transform result
	MkFloat2 m_WorldVertice[MkFloatRect::eMaxPointName];
	MkFloat2 m_UV[MkFloatRect::eMaxPointName];
	MkFloatRect m_AABR; // axis-aligned bounding rect

	//MkHashStr m_ForcedFontState;

	// texture
	MkBaseTexturePtr m_Texture;
	MkHashStr m_SubsetOrSequenceName;
	double m_SequenceInitTime;
	MaterialKey m_MaterialKey;

	//MkTextCacheStep m_TextCacheStep;
	//MkArray<MkHashStr> m_SceneDecoTextNodeNameAndKey;
	//MkStr m_OriginalDecoStr;

	// flag
	MkBitFieldDW m_Attribute;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPanel::VertexData)
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkPanel::MaterialKey)
