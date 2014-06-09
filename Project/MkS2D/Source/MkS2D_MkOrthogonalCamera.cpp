
#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkOrthogonalCamera.h"


#define MKDEF_S2D_CAMERA_DEPTH_OFFSET 1.f

//------------------------------------------------------------------------------------------------//

void MkOrthogonalCamera::UpdateViewProjectionMatrix(void)
{
	// view matrix
	D3DXVECTOR3 ep(m_Position.x + 0.5f, m_Position.y - 0.5f, -(MKDEF_S2D_CAMERA_DEPTH_OFFSET)); // magic number
	D3DXVECTOR3 lp(ep.x, ep.y, 0.f);
	D3DXVECTOR3 ud(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &ep, &lp, &ud);

	// projection matrix
	D3DXMatrixOrthoLH(&m_ProjectionMatrix, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y), m_NearClip, m_FarClip);
}

MkOrthogonalCamera::MkOrthogonalCamera()
{
	m_NearClip = MKDEF_S2D_CAMERA_DEPTH_OFFSET - 0.0000001f;
	m_FarClip = MKDEF_S2D_MAX_WORLD_DEPTH + MKDEF_S2D_CAMERA_DEPTH_OFFSET;

	SetSize(MK_DISPLAY_MGR.GetCurrentResolution());
}

//------------------------------------------------------------------------------------------------//
