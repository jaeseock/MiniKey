#pragma once


//------------------------------------------------------------------------------------------------//
// scene������ ���� ����
// 2d�� ���� 3d�� ó������ ���� �ٸ�
// - position�� depth�� ���Ե��� ����(���ĵ� �ٸ�)
// - 2d rotation
// - 2d scaling
// - transform�� �ƴ����� alpha ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkSceneTransform
{
public:

	// local
	inline void SetLocalPosition(const MkFloat2& position) { m_LocalPosition = position; }
	inline const MkFloat2& GetLocalPosition(void) const { return m_LocalPosition; }

	inline void SetLocalDepth(float depth) { m_LocalDepth = depth; }
	inline float GetLocalDepth(void) const { return m_LocalDepth; }

	void SetLocalRotation(float rotation);
	inline float GetLocalRotation(void) const { return m_LocalRotation; }

	inline void SetLocalScale(float scale) { m_LocalScale = GetMax<float>(scale, 0.f); }
	inline float GetLocalScale(void) const { return m_LocalScale; }

	inline void SetLocalAlpha(float alpha) { m_LocalAlpha = Clamp<float>(alpha, 0.f, 1.f); }
	inline float GetLocalAlpha(void) const { return m_LocalAlpha; }

	// world
	inline const MkFloat2& GetWorldPosition(void) const { return m_WorldPosition; }
	inline float GetWorldDepth(void) const { return m_WorldDepth; }
	inline float GetWorldRotation(void) const { return m_WorldRotation; }
	inline float GetWorldScale(void) const { return m_WorldScale; }
	inline float GetWorldAlpha(void) const { return m_WorldAlpha; }

	// update
	void Update(const MkSceneTransform* parentTransform);

	// identify
	void ClearLocalTransform(void);

	// local rect -> world rect vertices
	void GetWorldRectVertices(const MkFloatRect& rect, MkFloat2 (&vertices)[MkFloatRect::eMaxPointName]) const;

	// in & out
	static void SetObjectTemplate(MkDataNode& node);
	void Load(const MkDataNode& node);
	void Save(MkDataNode& node) const;

	MkSceneTransform();
	~MkSceneTransform() {}

protected:

	MkFloat2 m_LocalPosition;
	float m_LocalDepth;
	float m_LocalRotation;
	float m_LocalScale;
	float m_LocalAlpha;

	MkFloat2 m_WorldPosition;
	float m_WorldDepth;
	float m_WorldRotation;
	float m_WorldScale;
	float m_WorldAlpha;

public:

	static const MkHashStr ObjKey_LocalPosition;
	static const MkHashStr ObjKey_LocalDepth;
	static const MkHashStr ObjKey_LocalRotation;
	static const MkHashStr ObjKey_LocalScale;
	static const MkHashStr ObjKey_LocalAlpha;
};
