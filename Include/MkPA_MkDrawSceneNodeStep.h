#pragma once


//------------------------------------------------------------------------------------------------//
// 주어진 scene node(및 모든 하위)를 그리기 위한 draw step
// backbuffer, 혹은 render to texture에 그릴 수 있음
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkBaseResetableResource.h"
#include "MkPA_MkRenderTarget.h"
#include "MkPA_MkOrthogonalCamera.h"
#include "MkPA_MkDrawStepInterface.h"


class MkSceneNode;

class MkDrawSceneNodeStep : public MkBaseResetableResource, public MkDrawStepInterface
{
public:

	// 초기화
	// type : back buffer(eBackbuffer) or render to texture(eTexture)
	// count : type이 eTexture일 경우 최대 채널 수
	// size : type이 eTexture일 경우 생성 할 render to texture의 크기. (0, 0)일 경우 screen size로 설정
	// camOffset : camera 위치 설정시의 offset
	// texFormat : type이 eTexture일 경우 생성 할 render to texture의 포맷
	bool SetUp(
		MkRenderTarget::eTargetType type = MkRenderTarget::eBackbuffer,
		unsigned int count = 1,
		const MkInt2& size = MkInt2(0, 0),
		const MkFloat2& camOffset = MkFloat2(0.f, 0.f),
		MkRenderToTexture::eFormat texFormat = MkRenderToTexture::eRGBA);

	// 그리기 대상이 될 scene node 추가
	inline void SetSceneNode(const MkSceneNode* targetNode) { m_TargetSceneNode = targetNode; }

	// 배경색 지정
	inline void SetBackgroundColor(const MkColor& color) { m_RenderTarget.SetBackgroundColor(color); }

	// camera 참조 반환
	inline MkOrthogonalCamera& GetCamera(void) { return m_Camera; }
	inline const MkOrthogonalCamera& GetCamera(void) const { return m_Camera; }

	// camera 위치 설정시의 offset
	void SetCameraOffset(const MkFloat2& camOffset);
	inline const MkFloat2& GetCameraOffset(void) { return m_CameraOffset; }

	// 렌더타겟 영역 반환
	inline const MkFloatRect& GetRegionRect(void) { return m_RenderTarget.GetRegionRect(); }

	// 보이기 설정/반환
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// 렌더 타겟이 eTexture일 경우 텍스쳐 참조 반환
	virtual MkBaseTexture* GetTargetTexture(unsigned int index = 0) const;

	// 그리기
	virtual bool Draw(void);

	// 해제
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
