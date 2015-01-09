
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
		// ��ȿ �簢�� ����Ʈ
		MkPairArray<float, const MkSRect*> validRects(MKDEF_S2D_DEF_RECT_BUFFER_SIZE);

		// camera AABR
		MkFloat2 camSize = MkFloat2(static_cast<float>(m_Camera.GetSize().x), static_cast<float>(m_Camera.GetSize().y));
		MkFloat2 camPos = m_Camera.GetPosition();
		camPos.x -= camSize.x / 2.f;
		camPos.y -= camSize.y / 2.f;
		MkFloatRect camAABR(camPos, camSize);

		// ��ϵ� ��� scenen node�� ���� ��ȿ �簢�� ����
		MK_INDEXING_LOOP(m_SceneNodes, i)
		{
			m_SceneNodes[i]->__GetAllValidSRects(camAABR, validRects);
		}

		LPDIRECT3DDEVICE9 device = MK_DISPLAY_MGR.GetDevice();
		if ((!validRects.Empty()) && (device != NULL))
		{
			// �ڿ� �ִ� rect���� ���� �׷��� �ϱ� ������ �������� ����
			validRects.SortInDescendingOrder();

			unsigned int validRectCount = validRects.GetSize();
			MK_DRAWING_MONITOR.IncreaseValidSRectCounter(validRectCount);

			// batch rendering. ����(texture, material & object alpha)�� ���� rect ���� ����
			// �ٸ� ���� ���̿� ������ġó�� �� ���°� �ƴ϶�� �ִ��� �ϳ��� ������ ���� ��ǥ
			const MkArray<const MkSRect*>& rectList = validRects.GetFields();

			typedef struct __TexLink
			{
				unsigned int allocCount;
				int currUnionIndex;
				int lastRefIndex;
			}
			_TexLink;

			// ������ ���� ������
			MkMap<MkSRect::MaterialKey, _TexLink> texLink; // map�� ���� �������� �ٸ� ������ �����̳ʰ� ����...;;
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

			// ���� ������ ���� �� ���� ���̺�
			MkArray< MkArray<const MkSRect*> > unions(validRectCount); // �־��� ��� ��� rect�� ���������� ���� �� ����

			// ���� ������ rect���� ��ȸ�ϸ� ������ ����Ʈ ����
			unsigned int j;
			bool newUnion;

			MK_INDEXING_LOOP(rectList, i)
			{
				const MkSRect::MaterialKey& materialKey = rectList[i]->__GetMaterialKey();
				_TexLink& tl = texLink[materialKey];

				// ���� �� ���� �׷����ų� ������ rect�� �ٷ� ���
				if (tl.currUnionIndex == -1)
				{
					newUnion = true;
				}
				// �׷��� ������ ������ġ �������� �˻�
				else
				{
					newUnion = false;

					const MkFloatRect& currAABR = rectList[i]->GetAABR();

					// �ش� ������ ������ ���� ���� rect ���ĺ��� �� rect ���� ��ȸ
					for (j=(tl.lastRefIndex + 1); j<i; ++j)
					{
						if (rectList[j]->__GetMaterialKey() == materialKey)
						{
							if (tl.lastRefIndex == (i-1)) // ���� ������ ���ӵǸ� üũ��ġ ����
							{
								tl.lastRefIndex = i;
							}
						}
						else if (currAABR.CheckIntersection(rectList[j]->GetAABR())) // ������ �ٸ��� �浹 ����
						{
							// �浹�� �߻��ߴٸ� ���� ���հ� �� rect ���̿� ���̿� ������ġ ������ �ٸ� ������ ���� rect�� �����Ѵٴ� �ǹ��̴� �� ���� ����
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

			// union�� ��ȸ�ϸ� text�� ���� �� ��� cache ����
			MK_INDEXING_LOOP(unions, i)
			{
				MkSRect* currRect = const_cast<MkSRect*>(unions[i][0]);
				currRect->__GenerateTextCache();
			}
			
			// draw rects
			MK_DRAWING_MONITOR.IncreaseRectSetCounter(unions.GetSize());

			if (m_RenderTarget.__BeginScene())
			{
				m_RenderTarget.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER); // S2D������ ����Ÿ�� ������ ���� �����Ƿ� �׳� ����

				// camera
				m_Camera.UpdateViewProjectionMatrix();
				MK_RENDER_STATE.UpdateViewProjectionTransform(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

				// union ��ȸ
				MK_INDEXING_LOOP(unions, i)
				{
					MkArray<const MkSRect*>& currUnion = unions[i];

					// vertex buffer ����
					MkArray<MkSRect::VertexData> vertexData(currUnion.GetSize() * 6);
					MK_INDEXING_LOOP(currUnion, j)
					{
						currUnion[j]->__FillVertexData(vertexData);
					}

					// union�� ���� ������ �����ϹǷ� �ƹ��ų� �ϳ��� �����ϸ� ��
					currUnion[0]->__AffectTexture();

					// set FVF
					MK_RENDER_STATE.UpdateGeometryBuffer(MKDEF_SRECT_FVF, NULL, 0);

					// draw primitive UP!!!
					// �簢�� �׸��� �� ������ ���� ���ؽ����� �����ϰ� ��Ʈ�� �ϴ� �Ϳ� ���� ���� ���̰� ����
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
