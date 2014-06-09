#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - display manager
//
// device�� ������ display ��ġ ���� ó��
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkType2.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkS2D_MkD3DDefinition.h"


#define MK_DISPLAY_MGR MkDisplayManager::Instance()


//------------------------------------------------------------------------------------------------//

class MkStr;
class MkListeningWindow;

class MkDisplayManager : public MkSingletonPattern<MkDisplayManager>
{
public:

	// ��ġ ����
	// size ��Ģ�� MkWin32Application ����
	bool SetUp(MkListeningWindow* targetWindow, const MkInt2& size, bool fullScreen);

	// ����
	void Clear(void);

	// ���� ���� �״�� ��ġ, ���ҽ� ���ʱ�ȭ
	bool ResetDevice(void);

	// ���� ������ Ŭ���̾�Ʈ ũ��� ��ġ, ���ҽ� ���ʱ�ȭ
	bool ResetDeviceByClientSize(void);

	// �־��� �������� ���÷��� ��� �缳��
	// size ��Ģ�� MkWin32Application ����
	bool ChangeDisplayMode(const MkInt2& size, bool fullScreen);

	// ��ġ���� �����Ǵ� ������� ���� ��ȯ
	// fullScreen�� ��츸 �ǹ� ����(fullScreen�� false�̸� �׻� true ��ȯ)
	bool CheckAvailableDisplayMode(const MkInt2& size, bool fullScreen) const;

	// view ��ǥ���� screen ��ǥ������ ��ȯ
	MkInt2 ConvertViewPointToScreenPoint(float x, float y) const;

	// ���� �ػ� ��ȯ
	MkInt2 GetCurrentResolution(void) const;

	// Ǯ��ũ�� ���� ��ȯ
	bool IsFullScreen(void) const;

	// ���� display mode�� ���ڿ��� ��ȯ�� ��ȯ
	MkStr GetCurrentDisplayModeString(void) const;

	// ��� ������ �ػ� ����Ʈ ��ȯ
	void GetAvailableResolutionList(MkArray<MkInt2>& buffer) const;

	// D3DCAPS9 ��ȯ
	inline const D3DCAPS9& GetCaps(void) const { return m_Caps; }

	// D3D device ��ȯ
	inline LPDIRECT3DDEVICE9 GetDevice(void) const { return m_Device; }

	// aspect ratio ��ȯ
	inline float GetAspectRatio(void) const { return m_AspectRatio; }

	// depth-stencil format ��ȯ
	inline D3DFORMAT GetDepthStencilFormat(void) const { return m_CurrentPresentParameters.AutoDepthStencilFormat; }

	MkDisplayManager();
	virtual ~MkDisplayManager() {}

protected:

	D3DFORMAT _CheckDepthStencilFormat(D3DFORMAT targetFormat) const;

	unsigned int _GetDisplayModeIndex(const MkInt2& size) const;

	bool _Reset(void);
	bool _Reset(const MkInt2& size, bool fullScreen);

protected:

	// ������ ��� ������
	MkListeningWindow* m_TargetWindow;

	//------------------------------------------------------------------------------------------------//
	// ��ġ ������ �����Ǹ� ���� ���� (const�� ����)
	//------------------------------------------------------------------------------------------------//

	// D3D ��ġ
	IDirect3D9* m_Interface;
	LPDIRECT3DDEVICE9 m_Device;

	// caps
	D3DCAPS9 m_Caps;

	// ��� �ػ� ����Ʈ
	MkMap<unsigned int, MkInt2> m_AvailableResolutionTable;

	//------------------------------------------------------------------------------------------------//
	// ���� ���������� main thread������ ����
	//------------------------------------------------------------------------------------------------//

	// ���� D3DPRESENT_PARAMETERS
	D3DPRESENT_PARAMETERS m_CurrentPresentParameters;

	// ���� ��Ⱦ��
	float m_AspectRatio;
};
