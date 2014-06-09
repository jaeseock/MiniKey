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

	// 초기화
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type이 eTexture일 경우 최대 채널 수
	// size : type이 eTexture일 경우 생성 할 render to texture의 크기
	// texFormat : type이 eTexture일 경우 생성 할 render to texture의 포맷
	bool SetUp(
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 0,
		const MkUInt2& size = MkUInt2(0, 0),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	inline void SetBackgroundColor(const MkColor& color) { m_RenderTarget.SetBackgroundColor(color); }

	// camera 참조 반환
	inline MkOrthogonalCamera& GetCamera(void) { return m_Camera; }

	// 렌더타겟 영역 반환
	inline const MkFloatRect& GetRegionRect(void) const { return m_RenderTarget.GetRegionRect(); }

	// 보이기 설정/반환
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// 렌더 타겟이 eTexture일 경우 텍스쳐 참조 반환
	void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const;

	// 그리기 대상이 될 scene node 추가
	void AddSceneNode(const MkSceneNode* targetNode);

	// 등록된 모든 scene node 제거
	void ClearAllSceneNodes(void);

	// 그리기
	bool Draw(void);

	// 해제
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
