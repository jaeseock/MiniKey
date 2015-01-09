#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - device manager
//
// display device 관련 처리
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkType2.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkPA_MkD3DDefinition.h"


#define MK_DEVICE_MGR MkDeviceManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkStr;
class MkListeningWindow;

class MkDeviceManager : public MkSingletonPattern<MkDeviceManager>
{
public:

	// 장치 생성
	// size 규칙은 MkWin32Application 참조
	bool SetUp(MkListeningWindow* targetWindow, const MkInt2& size, bool fullScreen);

	// 해제
	void Clear(void);

	// 현재 설정 그대로 장치, 리소스 재초기화
	bool ResetDevice(void);

	// 현재 윈도우 클라이언트 크기로 장치, 리소스 재초기화
	bool ResetDeviceByClientSize(void);

	// 주어진 설정으로 디스플레이 모드 재설정
	// size 규칙은 MkWin32Application 참조
	bool ChangeDisplayMode(const MkInt2& size, bool fullScreen);

	// 장치에서 지원되는 모드인지 여부 반환
	// fullScreen일 경우만 의미 있음(fullScreen이 false이면 항상 true 반환)
	bool CheckAvailableDisplayMode(const MkInt2& size, bool fullScreen) const;

	// view 좌표값을 screen 좌표값으로 전환
	MkInt2 ConvertViewPointToScreenPoint(float x, float y) const;

	// 현재 해상도 반환
	MkInt2 GetCurrentResolution(void) const;

	// 풀스크린 여부 반환
	bool IsFullScreen(void) const;

	// 현재 display mode를 문자열로 변환해 반환
	MkStr GetCurrentDisplayModeString(void) const;

	// 사용 가능한 해상도 리스트 반환
	void GetAvailableResolutionList(MkArray<MkInt2>& buffer) const;

	// D3DCAPS9 반환
	inline const D3DCAPS9& GetCaps(void) const { return m_Caps; }

	// D3D device 반환
	inline LPDIRECT3DDEVICE9 GetDevice(void) const { return m_Device; }

	// aspect ratio 반환
	inline float GetAspectRatio(void) const { return m_AspectRatio; }

	// depth-stencil format 반환
	inline D3DFORMAT GetDepthStencilFormat(void) const { return m_CurrentPresentParameters.AutoDepthStencilFormat; }

	// target window 반환
	inline const MkListeningWindow* GetTargetWindow(void) const { return m_TargetWindow; }

	MkDeviceManager();
	virtual ~MkDeviceManager() {}

protected:

	D3DFORMAT _CheckDepthStencilFormat(D3DFORMAT targetFormat) const;

	unsigned int _GetDisplayModeIndex(const MkInt2& size) const;

	bool _Reset(void);
	bool _Reset(const MkInt2& size, bool fullScreen);

protected:

	// 렌더링 대상 윈도우
	MkListeningWindow* m_TargetWindow;

	//------------------------------------------------------------------------------------------------//
	// 장치 생성시 결정되며 변경 없음 (const로 간주)
	//------------------------------------------------------------------------------------------------//

	// D3D 장치
	IDirect3D9* m_Interface;
	LPDIRECT3DDEVICE9 m_Device;

	// caps
	D3DCAPS9 m_Caps;

	// 허용 해상도 리스트
	MkMap<unsigned int, MkInt2> m_AvailableResolutionTable;

	//------------------------------------------------------------------------------------------------//
	// 갱신 가능하지만 main thread에서만 접근
	//------------------------------------------------------------------------------------------------//

	// 현재 D3DPRESENT_PARAMETERS
	D3DPRESENT_PARAMETERS m_CurrentPresentParameters;

	// 현재 종횡비
	float m_AspectRatio;
};
