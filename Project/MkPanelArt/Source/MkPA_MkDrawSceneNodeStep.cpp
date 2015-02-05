
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

	// ��ȿ panel ����Ʈ
	MkPairArray<float, const MkPanel*> validPanels(MKDEF_DEF_PANEL_BUFFER_SIZE);

	// camera AABR
	const MkFloat2& camSize = m_Camera.GetSize();
	MkFloat2 camPos = m_Camera.GetPosition() - camSize / 2.f;
	MkFloatRect camAABR(camPos, camSize);

	// scene node�� ���� ��ȿ �簢�� ����
	m_TargetSceneNode->__GetAllValidPanels(camAABR, validPanels);

	if (!validPanels.Empty())
	{
		// �ڿ� �ִ� rect���� ���� �׷��� �ϱ� ������ �������� ����
		validPanels.SortInDescendingOrder();

		unsigned int validRectCount = validPanels.GetSize();
		MK_DRAWING_MONITOR.IncreaseValidPanelCounter(validRectCount);

		// batch rendering
		// ����(texture & object alpha)�� ���� panel ���� ����
		// �ٸ� ���� ���̿� ������ġó�� �� ���°� �ƴ϶�� �ִ��� �ϳ��� ������ ���� ��ǥ
		const MkArray<const MkPanel*>& panelList = validPanels.GetFields();

		typedef struct __MaterialLink
		{
			unsigned int allocCount;
			int currUnionIndex;
			int lastRefIndex;
		}
		_MaterialLink;

		// ������ ���� ������
		MkMap<MkPanel::MaterialKey, _MaterialLink> materialLink; // map�� ���� �������� �ٸ� ������ �����̳ʰ� ����...;;
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

		// ���� ������ ���� �� ���� ���̺�
		MkArray< MkArray<const MkPanel*> > unions(validRectCount); // �־��� ��� ��� rect�� ���������� ���� �� ����

		// ���� ������ rect���� ��ȸ�ϸ� ������ ����Ʈ ����
		unsigned int j;
		bool newUnion;

		MK_INDEXING_LOOP(panelList, i)
		{
			const MkPanel::MaterialKey& materialKey = panelList[i]->__GetMaterialKey();
			_MaterialLink& ml = materialLink[materialKey];

			// ���� �� ���� �׷����ų� ������ rect�� �ٷ� ���
			if (ml.currUnionIndex == -1)
			{
				newUnion = true;
			}
			// �׷��� ������ ������ġ �������� �˻�
			else
			{
				newUnion = false;

				const MkFloatRect& currAABR = panelList[i]->GetWorldAABR();

				// �ش� ������ ������ ���� ���� rect ���ĺ��� �� rect ���� ��ȸ
				for (j=(ml.lastRefIndex + 1); j<i; ++j)
				{
					if (panelList[j]->__GetMaterialKey() == materialKey)
					{
						if (ml.lastRefIndex == (i-1)) // ���� ������ ���ӵǸ� üũ��ġ ����
						{
							ml.lastRefIndex = i;
						}
					}
					else if (currAABR.CheckIntersection(panelList[j]->GetWorldAABR())) // ������ �ٸ��� �浹 ����
					{
						// �浹�� �߻��ߴٸ� ���� ���հ� �� rect ���̿� ���̿� ������ġ ������ �ٸ� ������ ���� rect�� �����Ѵٴ� �ǹ��̴� �� ���� ����
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

		// union�� ��ȸ�ϸ� text�� ���� �� ��� cache ����
		MK_INDEXING_LOOP(unions, i)
		{
			MkPanel* currPanel = const_cast<MkPanel*>(unions[i][0]);
			currPanel->__ExcuteCustomDrawStep();
		}
		
		// draw rects
		MK_DRAWING_MONITOR.IncreasePanelSetCounter(unions.GetSize());

		if (m_RenderTarget.__BeginScene())
		{
			m_RenderTarget.__Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER); // ����Ÿ�� ������ ���� �����Ƿ� �׳� ����

			// camera
			m_Camera.UpdateViewProjectionMatrix();
			MK_RENDER_STATE.UpdateViewProjectionTransform(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

			// union ��ȸ
			MK_INDEXING_LOOP(unions, i)
			{
				MkArray<const MkPanel*>& currUnion = unions[i];

				// vertex buffer ����
				MkArray<MkPanel::VertexData> vertexData(currUnion.GetSize() * 6);
				MK_INDEXING_LOOP(currUnion, j)
				{
					currUnion[j]->__FillVertexData(vertexData);
				}

				// union�� ���� ������ �����ϹǷ� �ƹ��ų� �ϳ��� �����ϸ� ��
				currUnion[0]->__AffectTexture();

				// draw primitive UP!!!
				// �簢�� �׸��� �� ������ ���� ���ؽ����� �����ϰ� ��Ʈ�� �ϴ� �Ϳ� ���� ���� ���̰� ����
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
