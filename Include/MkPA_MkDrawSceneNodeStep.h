#pragma once


//------------------------------------------------------------------------------------------------//
// �־��� scene node(�� ��� ����)�� �׸��� ���� draw step
// backbuffer, Ȥ�� render to texture�� �׸� �� ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseResetableResource.h"
#include "MkPA_MkRenderTarget.h"
#include "MkPA_MkOrthogonalCamera.h"
#include "MkPA_MkDrawStepInterface.h"


class MkSceneNode;

class MkDrawSceneNodeStep : public MkBaseResetableResource, public MkDrawStepInterface
{
public:

	// �ʱ�ȭ
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type�� eTexture�� ��� �ִ� ä�� ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ũ��. (0, 0)�� ��� screen size�� ����
	// camOffset : camera ��ġ �������� offset
	// texFormat : type�� eTexture�� ��� ���� �� render to texture�� ����
	bool SetUp(
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 1,
		const MkInt2& size = MkInt2(0, 0),
		const MkFloat2& camOffset = MkFloat2(0.f, 0.f),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// �׸��� ����� �� scene node �߰�
	inline void SetSceneNode(const MkSceneNode* targetNode) { m_TargetSceneNode = targetNode; }

	// ���� ����
	inline void SetBackgroundColor(const MkColor& color) { m_RenderTarget.SetBackgroundColor(color); }

	// camera ���� ��ȯ
	inline MkOrthogonalCamera& GetCamera(void) { return m_Camera; }
	inline const MkOrthogonalCamera& GetCamera(void) const { return m_Camera; }

	// camera ��ġ �������� offset
	void SetCameraOffset(const MkFloat2& camOffset);
	inline const MkFloat2& GetCameraOffset(void) { return m_CameraOffset; }

	// ����Ÿ�� ���� ��ȯ
	inline const MkFloatRect& GetRegionRect(void) { return m_RenderTarget.GetRegionRect(); }

	// ���̱� ����/��ȯ
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// ���� Ÿ���� eTexture�� ��� �ؽ��� ���� ��ȯ
	virtual MkBaseTexture* GetTargetTexture(unsigned int index = 0) const;

	// �׸���
	virtual bool Draw(void);

	// ����
	void Clear(void);

	// MkBaseResetableResource
	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	MkDrawSceneNodeStep();
	virtual ~MkDrawSceneNodeStep() { Clear(); }

protected:

	void _UpdateCameraInfo(void);

protected:

	bool m_Visible;
	
	const MkSceneNode* m_TargetSceneNode;
	MkRenderTarget m_RenderTarget;
	MkOrthogonalCamera m_Camera;
	MkFloat2 m_CameraOffset;
};
