
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkOrthogonalCamera.h"


#define MKDEF_CAMERA_DEPTH_OFFSET 1.f

//------------------------------------------------------------------------------------------------//

void MkOrthogonalCamera::UpdateViewProjectionMatrix(void)
{
	// view matrix
	D3DXVECTOR3 ep(m_Position.x + 0.5f, m_Position.y - 0.5f, -(MKDEF_CAMERA_DEPTH_OFFSET)); // magic number
	D3DXVECTOR3 lp(ep.x, ep.y, 0.f);
	D3DXVECTOR3 ud(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &ep, &lp, &ud);

	// projection matrix
	D3DXMatrixOrthoLH(&m_ProjectionMatrix, m_Size.x, m_Size.y, m_NearClip, m_FarClip);
}

MkOrthogonalCamera::MkOrthogonalCamera()
{
	m_NearClip = MKDEF_CAMERA_DEPTH_OFFSET - 0.0000001f;
	m_FarClip = MKDEF_PA_MAX_WORLD_DEPTH + MKDEF_CAMERA_DEPTH_OFFSET;

	MkInt2 ss = MK_DEVICE_MGR.GetCurrentResolution();
	SetSize(MkFloat2(static_cast<float>(ss.x), static_cast<float>(ss.y)));
}

//------------------------------------------------------------------------------------------------//
