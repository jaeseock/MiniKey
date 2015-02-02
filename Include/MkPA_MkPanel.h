#pragma once


//------------------------------------------------------------------------------------------------//
// ������ data node ����
//
//	Node "SampleRect" : "T_SRect" // ������ ���� MKDEF_S2D_BT_SRECT_TEMPLATE_NAME ��� ����
//	{
//		uint Alpha = 128;
//		float Depth = 0.000;
//		vec2 Position = (100.000, 200.000);
//		bool Reflection = // [2]
//			Yes / No;
//		//str Resource = "map" / "Image\s03.jpg" / ""; // �̹��� ���
//		str Resource = "odt" / "<%T:����12%><%S:����02%>���� �޼����ٿ�"; // original deco text ���
//		//str Resource = "sdt" / "SampleCategory" / "ErrorMsg" / "SampleMsg"; // scene deco text ���
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

	enum eResizingType // source�� panel�� ũ�Ⱑ �ٸ� ���� ����
	{
		eFollowSource = 0, // source�� ũ�⿡ panel�� ����
		eExpandOrReduce, // panel ũ�⿡ source�� Ȯ��/����� ����
		eExpandOrCut // source�� panel���� ���� ��� Ȯ��, Ŭ ��� �Ϻθ� �߶� ������. offset(scrolling) ���� ����
	};

	enum eAttribute
	{
		// ��� ����
		eVisible = 1,

		// uv ����
		eHorizontalReflection = 1 << 1, // u
		eVerticalReflection = 1 << 2 // v
	};

public:

	// MkDataNode�� ����. ���� �������� �����ϸ� ���
	//void Load(const MkDataNode& node);

	// MkDataNode�� ���
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
	inline void SetPanelSize(const MkFloat2& size) { m_PanelSize = size; } // resizing type�� eFollowSource�� ���� �ǹ� ����
	inline const MkFloat2& GetPanelSize(void) const { return m_PanelSize; }

	// panel scroll offset
	// resizing type�� eExpandOrCut�̰� source ũ�Ⱑ panel���� Ŭ ���(cut) ��Ȳ���� ����
	inline void SetScrollOffset(const MkFloat2& offset) { m_ScrollOffset = offset; }
	inline const MkFloat2& GetScrollOffset(void) const { return m_ScrollOffset; }

	// get axis-aligned minimum bounding rect
	inline const MkFloatRect& GetAABR(void) const { return m_AABR; }

	// ���� font state ����. ���� ����Ǵ� ��� deco string�� �ش� ������ ������ ���� ��
	//void SetFocedFontState(const MkHashStr& fontState);
	//inline bool CheckFocedFontTypeAndState(void) const { return (!m_ForcedFontState.Empty()); }

	// texture
	// (NOTE) SetDecoString()�� ��Ÿ��. ���� ���� �� original/scene deco text�� ���� ��� text ��ȿȭ
	void SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double initTime = 0.);
	void SetTexture(const MkPathName& imagePath, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY, double initTime = 0.);

	// image info
	inline void SetSubsetOrSequenceName(const MkHashStr& subsetOrSequenceName) { m_SubsetOrSequenceName = subsetOrSequenceName; }
	inline const MkHashStr& GetSubsetOrSequenceName(void) const { return m_SubsetOrSequenceName; }
	unsigned int GetAllSubsets(MkArray<MkHashStr>& keyList) const;
	unsigned int GetAllSequences(MkArray<MkHashStr>& keyList) const;

	// original deco text ����
	// deco string �������� �����Ǿ� ������ �״�� ����ǰ�, �Ϲ� ���ڿ��̸� DSF�� ����Ǿ� ��µ�
	// (NOTE) SetTexture()�� ��Ÿ��. ������ �����ϸ� ĳ�� �ؽ��İ� ���� ���� �� texture�� ��ü(���� texture ��ȿȭ)
	//bool SetDecoString(const MkStr& decoStr);

	// scene deco text table���� nodeNameAndKey�� �ش��ϴ� deco text ����
	// (NOTE) SetTexture()�� ��Ÿ��. ������ �����ϸ� ĳ�� �ؽ��İ� ���� ���� �� texture�� ��ü(���� texture ��ȿȭ)
	//bool SetDecoString(const MkArray<MkHashStr>& nodeNameAndKey);

	// ������ deco text ��ȯ
	//inline const MkDecoStr& GetDecoString(void) const { return m_TextCacheStep.GetDecoStr(); }

	// ���� ������ deco string�� �ִٸ� ��ε�
	//void RestoreDecoString(void);

	// uv reflection
	inline void SetHorizontalReflection(bool enable) { m_Attribute.Assign(eHorizontalReflection, enable); }
	inline bool GetHorizontalReflection(void) const { return m_Attribute[eHorizontalReflection]; }

	inline void SetVerticalReflection(bool enable) { m_Attribute.Assign(eVerticalReflection, enable); }
	inline bool GetVerticalReflection(void) const { return m_Attribute[eVerticalReflection]; }

	// visible
	inline void SetVisible(bool visible) { m_Attribute.Assign(eVisible, visible); }
	inline bool GetVisible(void) const { return m_Attribute[eVisible]; }

	// �׷��� �� �ִ��� ���� ��ȯ
	bool CheckDrawable(const MkFloatRect& cameraAABR) const;

	// ����
	//void AlignRect(const MkFloat2& anchorSize, eRectAlignmentPosition alignment, const MkFloat2& border, float heightOffset);

	// ����
	void Clear(void);

public:
	//------------------------------------------------------------------------------------------------//

	typedef struct _VertexData
	{
		float x, y, z, u, v;
	}
	VertexData;

	class MaterialKey // std::map�� key�� �� �� �ִ� �ּ� ����(operator <) ����
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
