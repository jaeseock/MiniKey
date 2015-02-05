
#include "MkCore_MkCheck.h"
#include "MkCore_MkPairArray.h"

#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkResetableResourcePool.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"

#include "MkPA_MkFontManager.h"

#define MKDEF_DEF_PANEL_BUFFER_SIZE 1000


//------------------------------------------------------------------------------------------------//

bool MkDrawSceneNodeStep::SetUp(MkRenderTarget::eTargetType type, unsigned int count, const MkInt2& size, MkRenderToTexture::eFormat texFormat)
{
	bool ok = m_RenderTarget.SetUp(type, count, size, texFormat);
	if (ok)
	{
		_UpdateCameraInfo();

		MK_RESETABLE_RESPOOL.RegisterResource(this);
	}
	return ok;
}

void MkDrawSceneNodeStep::GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const
{
	buffer = (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture) ? const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(index)) : NULL;
}

bool MkDrawSceneNodeStep::Draw(void)
{
	if ((!m_Visible) || (m_TargetSceneNode == NULL))
		return false;

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	bool drawn = false;

	// 유효 panel 리스트
	MkPairArray<float, const MkPanel*> validPanels(MKDEF_DEF_PANEL_BUFFER_SIZE);

	// camera AABR
	const MkFloat2& camSize = m_Camera.GetSize();
	MkFloat2 camPos = m_Camera.GetPosition() - camSize / 2.f;
	MkFloatRect camAABR(camPos, camSize);

	// scene node에 속한 유효 사각형 수집
	m_TargetSceneNode->__GetAllValidPanels(camAABR, validPanels);

	if (!validPanels.Empty())
	{
		// 뒤에 있는 rect부터 먼저 그려야 하기 때문에 내림차순 정렬
		validPanels.SortInDescendingOrder();

		unsigned int validRectCount = validPanels.GetSize();
		MK_DRAWING_MONITOR.IncreaseValidPanelCounter(validRectCount);

		// batch rendering
		// 재질(texture & object alpha)에 따른 panel 집합 구성
		// 다른 재질 사이에 샌드위치처럼 낀 상태가 아니라면 최대한 하나로 모으는 것이 목표
		const MkArray<const MkPanel*>& panelList = validPanels.GetFields();

		typedef struct __MaterialLink
		{
			unsigned int allocCount;
			int currUnionIndex;
			int lastRefIndex;
		}
		_MaterialLink;

		// 재질별 정보 보관소
		MkMap<MkPanel::MaterialKey, _MaterialLink> materialLink; // map은 심히 느리지만 다른 적합한 컨테이너가 없음...;;
		MK_INDEXING_LOOP(panelList, i)
		{
			const MkPanel::MaterialKey& materialKey = panelList[i]->__GetMaterialKey();
			if (materialLink.Exist(materialKey))
			{
				++materialLink[materialKey].allocCount;
			}
			else
			{
				_MaterialLink& ml = materialLink.Create(materialKey);
				ml.allocCount = 1;
				ml.currUnionIndex = -1;
				ml.lastRefIndex = -1;
			}
		}

		MK_DRAWING_MONITOR.IncreaseValidMaterialCounter(materialLink.GetSize());

		// 실제 집합이 저장 될 이중 테이블
		MkArray< MkArray<const MkPanel*> > unions(validRectCount); // 최악의 경우 모든 rect가 독자적으로 잡힐 수 있음

		// 가장 뒤쪽의 rect부터 순회하며 재질별 리스트 구성
		unsigned int j;
		bool newUnion;

		MK_INDEXING_LOOP(panelList, i)
		{
			const MkPanel::MaterialKey& materialKey = panelList[i]->__GetMaterialKey();
			_MaterialLink& ml = materialLink[materialKey];

			// 재질 별 최초 그려지거나 마지막 rect면 바로 등록
			if (ml.currUnionIndex == -1)
			{
				newUnion = true;
			}
			// 그렇지 않으면 샌드위치 상태인지 검사
			else
			{
				newUnion = false;

				const MkFloatRect& currAABR = panelList[i]->GetWorldAABR();

				// 해당 재질의 마지막 집합 시작 rect 이후부터 현 rect 사이 순회
				for (j=(ml.lastRefIndex + 1); j<i; ++j)
				{
					if (panelList[j]->__GetMaterialKey() == materialKey)
					{
						if (ml.lastRefIndex == (i-1)) // 같은 재질이 연속되면 체크위치 갱신
						{
							ml.lastRefIndex = i;
						}
					}
					else if (currAABR.CheckIntersection(panelList[j]->GetWorldAABR())) // 재질이 다르면 충돌 검출
					{
						// 충돌이 발생했다면 현재 집합과 현 rect 사이에 사이에 샌드위치 상태인 다른 재질을 가진 rect가 존재한다는 의미이니 새 집합 시작
						newUnion = true;
						break;
					}
				}
			}

			if (newUnion)
			{
				ml.currUnionIndex = unions.GetSize();
				unions.PushBack().Reserve(ml.allocCount);

				ml.lastRefIndex = i;
			}

			MkArray<const MkPanel*>& currUnion = unions[ml.currUnionIndex];
			currUnion.PushBack(panelList[i]);
			--ml.allocCount;
		}

		// union을 순회하며 text가 존재 할 경우 cache 생성
		MK_INDEXING_LOOP(unions, i)
		{
			MkPanel* currPanel = const_cast<MkPanel*>(unions[i][0]);
			currPanel->__ExcuteCustomDrawStep();
		}
		
		// draw rects
		MK_DRAWING_MONITOR.IncreasePanelSetCounter(unions.GetSize());

		if (m_RenderTarget.__BeginScene())
		{
			m_RenderTarget.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER); // 렌더타겟 공유를 하지 않으므로 그냥 날림

			// camera
			m_Camera.UpdateViewProjectionMatrix();
			MK_RENDER_STATE.UpdateViewProjectionTransform(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

			// union 순회
			MK_INDEXING_LOOP(unions, i)
			{
				MkArray<const MkPanel*>& currUnion = unions[i];

				// vertex buffer 구성
				MkArray<MkPanel::VertexData> vertexData(currUnion.GetSize() * 6);
				MK_INDEXING_LOOP(currUnion, j)
				{
					currUnion[j]->__FillVertexData(vertexData);
				}

				// union은 동일 재질을 참조하므로 아무거나 하나만 적용하면 됨
				currUnion[0]->__AffectTexture();

				// draw primitive UP!!!
				// 사각형 그리는 것 정도는 직접 버텍스버퍼 세심하게 콘트를 하는 것에 비해 거의 차이가 없음
				device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, currUnion.GetSize() * 2, vertexData.GetPtr(), sizeof(MkPanel::VertexData));
			}

			m_RenderTarget.__EndScene();

			drawn = true;
		}
	}

	return drawn;
}

void MkDrawSceneNodeStep::Clear(void)
{
	MK_RESETABLE_RESPOOL.DeleteResource(this);
	SetSceneNode(NULL);
	m_RenderTarget.Clear();
}

void MkDrawSceneNodeStep::UnloadResource(void)
{
	if (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture)
	{
		unsigned int cnt = m_RenderTarget.GetTextureCount();
		for (unsigned int i=0; i<cnt; ++i)
		{
			MkRenderToTexture* texture = const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(i));
			texture->UnloadResource();
		}
	}
}

void MkDrawSceneNodeStep::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	if (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture)
	{
		unsigned int cnt = m_RenderTarget.GetTextureCount();
		for (unsigned int i=0; i<cnt; ++i)
		{
			MkRenderToTexture* texture = const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(i));
			texture->ReloadResource(device);
		}
	}

	_UpdateCameraInfo();
}

MkDrawSceneNodeStep::MkDrawSceneNodeStep() : MkBaseResetableResource()
{
	m_Visible = true;
}

void MkDrawSceneNodeStep::_UpdateCameraInfo(void)
{
	MkInt2 ss = m_RenderTarget.GetScreenSize();
	MkFloat2 fss(static_cast<float>(ss.x), static_cast<float>(ss.y));

	m_Camera.SetSize(fss);
	m_Camera.SetPosition(fss / 2.f);
}

//------------------------------------------------------------------------------------------------//
