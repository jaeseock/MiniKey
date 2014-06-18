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
//		//str Resource = "map" / "Image\s03.jpg" / "0" / ""; // �̹��� ���
//		str Resource = "odt" / "<%T:����12%><%S:����02%>���� �޼����ٿ�"; // original deco text ���
//		//str Resource = "sdt" / "SampleCategory" / "ErrorMsg" / "SampleMsg"; // scene deco text ���
//		bool Visible = Yes;
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkS2D_MkBaseTexture.h"
#include "MkS2D_MkTextCacheStep.h"


class MkVec3;

class MkSRect
{
public:

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

public:

	// MkDataNode�� ����. ���� �������� �����ϸ� ���
	void Load(const MkDataNode& node);

	// MkDataNode�� ���
	void Save(MkDataNode& node);

	// set & get local rect
	inline void SetLocalRect(const MkFloatRect& rect) { m_LocalRect = rect; }
	inline const MkFloatRect& GetLocalRect(void) const { return m_LocalRect; }

	// set & get local position
	inline void SetLocalPosition(const MkFloat2& position) { m_LocalRect.position = position; }
	inline const MkFloat2& GetLocalPosition(void) const { return m_LocalRect.position; }

	// set & get local size
	inline void SetLocalSize(const MkFloat2& size) { m_LocalRect.size = size; }
	inline const MkFloat2& GetLocalSize(void) const { return m_LocalRect.size; }
	
	// set & get local depth
	inline void SetLocalDepth(float depth) { m_LocalDepth = depth; }
	inline float GetLocalDepth(void) const { return m_LocalDepth; }

	// set & get object alpha
	void SetObjectAlpha(float alpha);
	float GetObjectAlpha(void) const;
	
	// get axis-aligned minimum bounding rect
	inline const MkFloatRect& GetAABR(void) const { return m_AABR; }

	// world depth
	inline float GetWorldDepth(void) const { return m_WorldDepth; }

	// uv
	inline void SetUV(MkFloatRect::ePointName pn, const MkFloat2& uv) { m_UV[pn] = uv; }
	inline const MkFloat2& GetUV(MkFloatRect::ePointName pn) const { return m_UV[pn]; }

	// reflection
	inline void SetHorizontalReflection(bool enable) { m_HorizontalReflection = enable; }
	inline bool GetHorizontalReflection(void) const { return m_HorizontalReflection; }

	inline void SetVerticalReflection(bool enable) { m_VerticalReflection = enable; }
	inline bool GetVerticalReflection(void) const { return m_VerticalReflection; }

	// texture
	// (NOTE) SetDecoString()�� ��Ÿ��. ���� ���� �� original/scene deco text�� ���� ��� text ��ȿȭ
	void SetTexture(const MkBaseTexturePtr& texture);
	void SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetName);

	// original deco text ����
	// deco string �������� �����Ǿ� ������ �״�� ����ǰ�, �Ϲ� ���ڿ��̸� DSF�� ����Ǿ� ��µ�
	// (NOTE) SetTexture()�� ��Ÿ��. ������ �����ϸ� ĳ�� �ؽ��İ� ���� ���� �� texture�� ��ü(���� texture ��ȿȭ)
	bool SetDecoString(const MkStr& decoStr);

	// scene deco text table���� nodeNameAndKey�� �ش��ϴ� deco text ����
	// (NOTE) SetTexture()�� ��Ÿ��. ������ �����ϸ� ĳ�� �ؽ��İ� ���� ���� �� texture�� ��ü(���� texture ��ȿȭ)
	bool SetDecoString(const MkArray<MkHashStr>& nodeNameAndKey);

	// image subset
	void SetSubset(const MkHashStr& name);
	inline const MkHashStr& GetSubset(void) const { return m_CurrentSubsetName; }

	// set & get visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// �׷��� �� �ִ��� ���� ��ȯ
	bool CheckValidation(const MkFloatRect& cameraAABR) const;

	// ����
	void Clear(void);

	//------------------------------------------------------------------------------------------------//

	static void __GenerateBuildingTemplate(void);

	inline const MaterialKey& __GetMaterialKey(void) const { return m_MaterialKey; }

	void __GenerateTextCache(void);

	void __UpdateTransform(const MkVec3& position, float rotation, float cosR, float sinR, float scale);

	void __FillVertexData(MkArray<VertexData>& buffer) const;

	void __AffectTexture(void) const;

	MkSRect();
	virtual ~MkSRect() { Clear(); }

protected:

	void _UpdateWorldVertex(MkFloatRect::ePointName pn, const MkVec3& position, float rotation, float cosR, float sinR, float scale);

	void _FillVertexData(MkFloatRect::ePointName pn, MkArray<VertexData>& buffer) const;

	bool _SetDecoString(const MkDecoStr& decoStr);

protected:

	MkFloatRect m_LocalRect;
	float m_LocalDepth;

	MkFloat2 m_WorldVertex[MkFloatRect::eMaxPointName];
	float m_WorldDepth;
	MkFloatRect m_AABR; // axis-aligned minimum bounding rect

	MkFloat2 m_UV[MkFloatRect::eMaxPointName];

	MkBaseTexturePtr m_Texture;
	MkHashStr m_CurrentSubsetName;

	MkTextCacheStep m_TextCacheStep;
	MkArray<MkHashStr> m_SceneDecoTextNodeNameAndKey;
	MkStr m_OriginalDecoStr;

	MaterialKey m_MaterialKey;

	bool m_HorizontalReflection;
	bool m_VerticalReflection;

	bool m_Visible;
};

MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSRect::VertexData)
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkSRect::MaterialKey)
