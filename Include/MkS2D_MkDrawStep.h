#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkS2D_MkBaseResetableResource.h"
#include "MkS2D_MkRenderTarget.h"
#include "MkS2D_MkOrthogonalCamera.h"


class MkSceneNode;

class MkDrawStep : public MkBaseResetableResource
{
public:

	// �ʱ�ȭ
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type�� eTexture�� ��� �ִ� ä�� ��
	// size : type�� eTexture�� ��� ���� �� render to texture�� ũ��
	// texFormat : type�� eTexture�� ��� ���� �� render to texture�� ����
	bool SetUp(
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 0,
		const MkUInt2& size = MkUInt2(0, 0),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	inline void SetBackgroundColor(const MkColor& color) { m_RenderTarget.SetBackgroundColor(color); }

	// camera ���� ��ȯ
	inline MkOrthogonalCamera& GetCamera(void) { return m_Camera; }

	// ����Ÿ�� ���� ��ȯ
	inline const MkFloatRect& GetRegionRect(void) const { return m_RenderTarget.GetRegionRect(); }

	// ���̱� ����/��ȯ
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// ���� Ÿ���� eTexture�� ��� �ؽ��� ���� ��ȯ
	void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const;

	// �׸��� ����� �� scene node �߰�
	void AddSceneNode(const MkSceneNode* targetNode);

	// ��ϵ� ��� scene node ����
	void ClearAllSceneNodes(void);

	// �׸���
	bool Draw(void);

	// ����
	void Clear(void);

	// MkBaseResetableResource
	virtual void UnloadResource(void);
	virtual void ReloadResource(LPDIRECT3DDEVICE9 device);

	MkDrawStep(const MkHashStr& name);
	virtual ~MkDrawStep() { Clear(); }

protected:

	MkInt2 _UpdateCameraInfo(void);

protected:

	// set info
	MkHashStr m_StepName;
	bool m_Visible;
	bool m_FollowScreenSize;

	MkArray<const MkSceneNode*> m_SceneNodes;

	MkRenderTarget m_RenderTarget;

	MkOrthogonalCamera m_Camera;
};
