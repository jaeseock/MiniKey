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
//		//str Resource = "map" / "Image\s03.jpg" / "0" / ""; // 이미지 출력
//		str Resource = "odt" / "<%T:굴림12%><%S:샘플02%>샘플 메세지다요"; // original deco text 출력
//		//str Resource = "sdt" / "SampleCategory" / "ErrorMsg" / "SampleMsg"; // scene deco text 출력
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

public:

	// MkDataNode로 구성. 기존 설정값이 존재하면 덮어씀
	void Load(const MkDataNode& node);

	// MkDataNode로 출력
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
	// (NOTE) SetDecoString()과 배타적. 기존 설정 된 original/scene deco text가 있을 경우 text 무효화
	void SetTexture(const MkBaseTexturePtr& texture);
	void SetTexture(const MkBaseTexturePtr& texture, const MkHashStr& subsetName);

	// original deco text 설정
	// deco string 문법으로 구성되어 있으면 그대로 적용되고, 일반 문자열이면 DSF가 적용되어 출력됨
	// (NOTE) SetTexture()와 배타적. 설정이 성공하면 캐쉬 텍스쳐가 기존 설정 된 texture를 교체(기존 texture 무효화)
	bool SetDecoString(const MkStr& decoStr);

	// scene deco text table에서 nodeNameAndKey에 해당하는 deco text 설정
	// (NOTE) SetTexture()와 배타적. 설정이 성공하면 캐쉬 텍스쳐가 기존 설정 된 texture를 교체(기존 texture 무효화)
	bool SetDecoString(const MkArray<MkHashStr>& nodeNameAndKey);

	// image subset
	void SetSubset(const MkHashStr& name);
	inline const MkHashStr& GetSubset(void) const { return m_CurrentSubsetName; }

	// set & get visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// 그려질 수 있는지 여부 반환
	bool CheckValidation(const MkFloatRect& cameraAABR) const;

	// 해제
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
