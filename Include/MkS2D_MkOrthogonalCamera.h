#pragma once


//------------------------------------------------------------------------------------------------//
// orthogonal camera
// 2d에서 perspective camera는 사용하지 않음
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkD3DDefinition.h"


class MkOrthogonalCamera
{
public:

	// near
	inline float GetNearClip(void) const { return m_NearClip; }

	// far
	inline float GetFarClip(void) const { return m_FarClip; }

	// size
	inline void SetSize(const MkInt2& size) { m_Size = size; }
	inline const MkInt2& GetSize(void) const { return m_Size; }

	// position
	inline void SetPosition(const MkFloat2& position) { m_Position = position; }
	inline const MkFloat2& GetPosition(void) const { return m_Position; }

	// 카메라 행렬 반환
	inline const D3DXMATRIX& GetViewMatrix(void) const { return m_ViewMatrix; }
	inline const D3DXMATRIX& GetProjectionMatrix(void) const { return m_ProjectionMatrix; }
	
	// 현 world transfrom을 바탕으로 view projection matrix를 계산
	void UpdateViewProjectionMatrix(void);
	
	MkOrthogonalCamera();
	virtual ~MkOrthogonalCamera() {}

protected:

	float m_NearClip;
	float m_FarClip;

	MkInt2 m_Size;
	MkFloat2 m_Position;

	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjectionMatrix;
};
