#pragma once


//------------------------------------------------------------------------------------------------//
// scene������ ���� ����
// 2d�� ���� 3d�� ó������ ���� �ٸ�
// - position�� depth�� ���Ե��� ����(���ĵ� �ٸ�)
// - 2d rotation
// - 2d scaling
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkRect.h"
#include "MkPA_MkProjectDefinition.h"


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

	inline void SetLocalScale(float scale) { m_LocalScale = (scale < 0.f) ? 0.f : scale; }
	inline float GetLocalScale(void) const { return m_LocalScale; }

	// world
	inline const MkFloat2& GetWorldPosition(void) const { return m_WorldPosition; }
	inline float GetWorldDepth(void) const { return m_WorldDepth; }
	inline float GetWorldRotation(void) const { return m_WorldRotation; }
	inline float GetWorldScale(void) const { return m_WorldScale; }

	// update
	void Update(const MkSceneTransform* parentTransform);

	// identify
	void Clear(void);

	// local rect -> world rect vertices
	void GetWorldRectVertices(const MkFloat2& rectSize, MkFloat2 (&vertices)[MkFloatRect::eMaxPointName]) const;

	MkSceneTransform() { Clear(); }
	~MkSceneTransform() {}

protected:

	MkFloat2 m_LocalPosition;
	float m_LocalDepth;
	float m_LocalRotation;
	float m_LocalScale;

	MkFloat2 m_WorldPosition;
	float m_WorldDepth;
	float m_WorldRotation;
	float m_WorldScale;
};
