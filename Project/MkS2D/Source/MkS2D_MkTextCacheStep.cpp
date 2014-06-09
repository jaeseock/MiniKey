
#include "MkCore_MkCheck.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkDrawingMonitor.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkRenderTarget.h"
#include "MkS2D_MkTextCacheStep.h"


//------------------------------------------------------------------------------------------------//

bool MkTextCacheStep::SetUp(const MkDecoStr& decoStr)
{
	Clear();

	m_DecoStr = decoStr;
	if (m_DecoStr.Empty())
		return false;

	MkBaseTexture* texture = new MkBaseTexture;
	MK_CHECK(texture != NULL, L"MkBaseTexture alloc 실패")
		return false;

	if (texture->SetUp(MkUInt2(static_cast<int>(m_DecoStr.GetDrawingSize().x), static_cast<int>(m_DecoStr.GetDrawingSize().y)), 1, 0, D3DFMT_A8R8G8B8, MkBaseTexture::eNone))
	{
		m_TargetTexture = texture;
		m_DrawText = true;
		return true;
	}

	delete texture;
	return false;
}

void MkTextCacheStep::Draw(void)
{
	if (m_DrawText)
	{
		LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
		if (device != NULL)
		{
			// 알파 없는 임시 렌더타겟 생성
			MkRenderTarget sourceRT;
			sourceRT.SetBackgroundColor(MKDEF_S2D_DECOSTR_COLOR_KEY);

			MkUInt2 size(static_cast<int>(m_DecoStr.GetDrawingSize().x), static_cast<int>(m_DecoStr.GetDrawingSize().y));
			if (sourceRT.SetUp(MkRenderTarget::eTexture, 1, size, MkRenderToTexture::eRGB) &&
				sourceRT.__BeginScene())
			{
				sourceRT.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER);

				MK_DRAWING_MONITOR.IncreaseValidDecoStrCounter();

				// 텍스트를 찍고
				bool ok = MK_FONT_MGR.DrawMessage(MkInt2(0, 0), m_DecoStr);

				sourceRT.__EndScene();

				// 이상이 없으면 알파 텍스쳐에 컬러키로 복사
				if (ok && (D3DXLoadSurfaceFromSurface
					(m_TargetTexture->GetSurface(), NULL, NULL, sourceRT.GetTargetTexture(0)->GetSurface(), NULL, NULL, D3DX_FILTER_POINT, MKDEF_S2D_DECOSTR_COLOR_KEY) == D3D_OK))
				{
					m_DrawText = false;
				}
			}
		}
	}
}

void MkTextCacheStep::Clear(void)
{
	m_DecoStr.Clear();
	m_TargetTexture = NULL;
	m_DrawText = false;
}

MkTextCacheStep::MkTextCacheStep()
{
	m_DrawText = false;
}

//------------------------------------------------------------------------------------------------//
