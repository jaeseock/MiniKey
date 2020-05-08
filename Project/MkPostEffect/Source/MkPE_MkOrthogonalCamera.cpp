
#include "MkPE_MkOrthogonalCamera.h"


//------------------------------------------------------------------------------------------------//

void MkOrthogonalCamera::UpdateViewProjectionMatrix(void)
{
	// view matrix
	D3DXVECTOR3 ep(m_Position.x + 0.5f, m_Position.y - 0.5f, -1.f); // magic number
	D3DXVECTOR3 lp(ep.x, ep.y, 0.f);
	D3DXVECTOR3 ud(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &ep, &lp, &ud);

	// projection matrix
	D3DXMatrixOrthoLH(&m_ProjectionMatrix, m_Size.x, m_Size.y, m_NearClip, m_FarClip);
}

MkOrthogonalCamera::MkOrthogonalCamera()
{
	m_NearClip = 0.9999f;
	m_FarClip = 100.f;
}

//------------------------------------------------------------------------------------------------//
