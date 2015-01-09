
#include "MkCore_MkCheck.h"
#include "MkCore_MkPairArray.h"

#include "MkS2D_MkRenderStateSetter.h"
#include "MkS2D_MkResetableResourcePool.h"
#include "MkS2D_MkDrawingMonitor.h"
#include "MkS2D_MkDisplayManager.h"
#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkDrawStep.h"

#include "MkS2D_MkFontManager.h"


#define MKDEF_S2D_DEF_RECT_BUFFER_SIZE 1000

#define MKDEF_SRECT_FVF (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))


//------------------------------------------------------------------------------------------------//

bool MkDrawStep::SetUp(MkRenderTarget::eTargetType type, unsigned int count, const MkUInt2& size, MkRenderToTexture::eFormat texFormat)
{
	bool ok = m_RenderTarget.SetUp(type, count, size, texFormat);
	if (ok)
	{
		MkInt2 currentScreenSize = _UpdateCameraInfo();

		if (type == MkRenderTarget::eBackbuffer)
		{
			m_FollowScreenSize = true;
		}
		else if (type == MkRenderTarget::eTexture)
		{
			m_FollowScreenSize = (currentScreenSize == MK_DISPLAY_MGR.GetCurrentResolution());
		}
		
		MK_RESETABLE_RESPOOL.RegisterResource(this);
	}
	return ok;
}

void MkDrawStep::GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const
{
	buffer = (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture) ? const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(index)) : NULL;
}

void MkDrawStep::AddSceneNode(const MkSceneNode* targetNode)
{
	m_SceneNodes.PushBack(targetNode);
}

void MkDrawStep::ClearAllSceneNodes(void)
{
	m_SceneNodes.Clear();
}

bool MkDrawStep::Draw(void)
{
	bool drawn = false;

	if (m_Visible && (!m_SceneNodes.Empty()))
	{
		// 유효 사각형 리스트
		MkPairArray<float, const MkSRect*> validRects(MKDEF_S2D_DEF_RECT_BUFFER_SIZE);

		// camera AABR
		MkFloat2 camSize = MkFloat2(static_cast<float>(m_Camera.GetSize().x), static_cast<float>(m_Camera.GetSize().y));
		MkFloat2 camPos = m_Camera.GetPosition();
		camPos.x -= camSize.x / 2.f;
		camPos.y -= camSize.y / 2.f;
		MkFloatRect camAABR(camPos, camSize);

		// 등록된 모든 scenen node에 속한 유효 사각형 수집
		MK_INDEXING_LOOP(m_SceneNodes, i)
		{
			m_SceneNodes[i]->__GetAllValidSRects(camAABR, validRects);
		}

		LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
		if ((!validRects.Empty()) && (device != NULL))
		{
			// 뒤에 있는 rect부터 먼저 그려야 하기 때문에 내림차순 정렬
			validRects.SortInDescendingOrder();

			unsigned int validRectCount = validRects.GetSize();
			MK_DRAWING_MONITOR.IncreaseValidSRectCounter(validRectCount);

			// batch rendering. 재질(texture, material & object alpha)에 따른 rect 집합 구성
			// 다른 재질 사이에 샌드위치처럼 낀 상태가 아니라면 최대한 하나로 모으는 것이 목표
			const MkArray<const MkSRect*>& rectList = validRects.GetFields();

			typedef struct __TexLink
			{
				unsigned int allocCount;
				int currUnionIndex;
				int lastRefIndex;
			}
			_TexLink;

			// 재질별 정보 보관소
			MkMap<MkSRect::MaterialKey, _TexLink> texLink; // map은 심히 느리지만 다른 적합한 컨테이너가 없음...;;
			MK_INDEXING_LOOP(rectList, i)
			{
				const MkSRect::MaterialKey& materialKey = rectList[i]->__GetMaterialKey();
				if (texLink.Exist(materialKey))
				{
					++texLink[materialKey].allocCount;
				}
				else
				{
					_TexLink& tl = texLink.Create(materialKey);
					tl.allocCount = 1;
					tl.currUnionIndex = -1;
					tl.lastRefIndex = -1;
				}
			}

			MK_DRAWING_MONITOR.IncreaseValidTextureCounter(texLink.GetSize());

			// 실제 집합이 저장 될 이중 테이블
			MkArray< MkArray<const MkSRect*> > unions(validRectCount); // 최악의 경우 모든 rect가 독자적으로 잡힐 수 있음

			// 가장 뒤쪽의 rect부터 순회하며 재질별 리스트 구성
			unsigned int j;
			bool newUnion;

			MK_INDEXING_LOOP(rectList, i)
			{
				const MkSRect::MaterialKey& materialKey = rectList[i]->__GetMaterialKey();
				_TexLink& tl = texLink[materialKey];

				// 재질 별 최초 그려지거나 마지막 rect면 바로 등록
				if (tl.currUnionIndex == -1)
				{
					newUnion = true;
				}
				// 그렇지 않으면 샌드위치 상태인지 검사
				else
				{
					newUnion = false;

					const MkFloatRect& currAABR = rectList[i]->GetAABR();

					// 해당 재질의 마지막 집합 시작 rect 이후부터 현 rect 사이 순회
					for (j=(tl.lastRefIndex + 1); j<i; ++j)
					{
						if (rectList[j]->__GetMaterialKey() == materialKey)
						{
							if (tl.lastRefIndex == (i-1)) // 같은 재질이 연속되면 체크위치 갱신
							{
								tl.lastRefIndex = i;
							}
						}
						else if (currAABR.CheckIntersection(rectList[j]->GetAABR())) // 재질이 다르면 충돌 검출
						{
							// 충돌이 발생했다면 현재 집합과 현 rect 사이에 사이에 샌드위치 상태인 다른 재질을 가진 rect가 존재한다는 의미이니 새 집합 시작
							newUnion = true;
							break;
						}
					}
				}

				if (newUnion)
				{
					tl.currUnionIndex = unions.GetSize();
					unions.PushBack().Reserve(tl.allocCount);

					tl.lastRefIndex = i;
				}

				MkArray<const MkSRect*>& currUnion = unions[tl.currUnionIndex];
				currUnion.PushBack(rectList[i]);
				--tl.allocCount;
			}

			// union을 순회하며 text가 존재 할 경우 cache 생성
			MK_INDEXING_LOOP(unions, i)
			{
				MkSRect* currRect = const_cast<MkSRect*>(unions[i][0]);
				currRect->__GenerateTextCache();
			}
			
			// draw rects
			MK_DRAWING_MONITOR.IncreaseRectSetCounter(unions.GetSize());

			if (m_RenderTarget.__BeginScene())
			{
				m_RenderTarget.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER); // S2D에서는 렌더타겟 공유를 하지 않으므로 그냥 날림

				// camera
				m_Camera.UpdateViewProjectionMatrix();
				MK_RENDER_STATE.UpdateViewProjectionTransform(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

				// union 순회
				MK_INDEXING_LOOP(unions, i)
				{
					MkArray<const MkSRect*>& currUnion = unions[i];

					// vertex buffer 구성
					MkArray<MkSRect::VertexData> vertexData(currUnion.GetSize() * 6);
					MK_INDEXING_LOOP(currUnion, j)
					{
						currUnion[j]->__FillVertexData(vertexData);
					}

					// union은 동일 재질을 참조하므로 아무거나 하나만 적용하면 됨
					currUnion[0]->__AffectTexture();

					// set FVF
					MK_RENDER_STATE.UpdateGeometryBuffer(MKDEF_SRECT_FVF, NULL, 0);

					// draw primitive UP!!!
					// 사각형 그리는 것 정도는 직접 버텍스버퍼 세심하게 콘트를 하는 것에 비해 거의 차이가 없음
					device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, currUnion.GetSize() * 2, vertexData.GetPtr(), sizeof(MkSRect::VertexData));

#ifdef MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
					int rtHeight = m_RenderTarget.GetScreenSize().y;
					MK_INDEXING_LOOP(currUnion, j)
					{
						float d = currUnion[j]->GetWorldDepth();
						MkFloat2 p = currUnion[j]->GetAABR().GetAbsolutePosition(MkFloatRect::eLeftTop);
						MK_FONT_MGR.DrawMessage(MkInt2(static_cast<int>(p.x), rtHeight - static_cast<int>(p.y)), MkStr(i) + L"-" + MkStr(j) + L" " + MkStr(d));
					}
#endif // MKDEF_S2D_DEBUG_SHOW_SRECT_SET_INFO
				}

				m_RenderTarget.__EndScene();

				drawn = true;
			}
		}
	}

	return drawn;
}

void MkDrawStep::Clear(void)
{
	MK_RESETABLE_RESPOOL.DeleteResource(this);

	ClearAllSceneNodes();

	m_RenderTarget.Clear();
}

void MkDrawStep::UnloadResource(void)
{
	if (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture)
	{
		unsigned int texCount = m_RenderTarget.GetTextureCount();
		for (unsigned int i=0; i<texCount; ++i)
		{
			MkRenderToTexture* texture = const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(i));
			texture->UnloadResource();
		}
	}
}

void MkDrawStep::ReloadResource(LPDIRECT3DDEVICE9 device)
{
	if (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture)
	{
		unsigned int texCount = m_RenderTarget.GetTextureCount();
		for (unsigned int i=0; i<texCount; ++i)
		{
			MkRenderToTexture* texture = const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(i));
			texture->ReloadResource(device);
		}
	}

	if (m_FollowScreenSize)
	{
		_UpdateCameraInfo();
	}
}

MkDrawStep::MkDrawStep(const MkHashStr& name) : MkBaseResetableResource()
{
	m_StepName = name;
	m_Visible = true;
	m_FollowScreenSize = false;
}

MkInt2 MkDrawStep::_UpdateCameraInfo(void)
{
	MkUInt2 ss = m_RenderTarget.GetScreenSize();
	MkInt2 currentScreenSize(ss.x, ss.y);
	m_Camera.SetSize(currentScreenSize);
	m_Camera.SetPosition(MkFloat2(static_cast<float>(currentScreenSize.x) / 2.f, static_cast<float>(currentScreenSize.y) / 2.f));

	return currentScreenSize;
}

//------------------------------------------------------------------------------------------------//
