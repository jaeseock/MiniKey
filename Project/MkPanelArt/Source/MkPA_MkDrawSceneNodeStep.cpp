
#include "MkCore_MkCheck.h"
#include "MkCore_MkPairArray.h"

#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkResetableResourcePool.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkSceneNode.h"
#include "MkPA_MkDrawSceneNodeStep.h"

#include "MkPA_MkFontManager.h"

#define MKDEF_DEF_OBJECT_BUFFER_SIZE 1000


//------------------------------------------------------------------------------------------------//

bool MkDrawSceneNodeStep::SetUp
(MkRenderTarget::eTargetType type, unsigned int count, const MkInt2& size, const MkFloat2& camOffset, MkRenderToTexture::eFormat texFormat)
{
	bool ok = m_RenderTarget.SetUp(type, count, size, texFormat);
	if (ok)
	{
		SetCameraOffset(camOffset);

		MK_RESETABLE_RESPOOL.RegisterResource(this);
	}
	return ok;
}

void MkDrawSceneNodeStep::SetCameraOffset(const MkFloat2& camOffset)
{
	m_CameraOffset = camOffset;
	_UpdateCameraInfo();
}

MkBaseTexture* MkDrawSceneNodeStep::GetTargetTexture(unsigned int index) const
{
	return (m_RenderTarget.GetTargetType() == MkRenderTarget::eTexture) ? const_cast<MkRenderToTexture*>(m_RenderTarget.GetTargetTexture(index)) : NULL;
}

bool MkDrawSceneNodeStep::Draw(void)
{
	if ((!m_Visible) || (m_TargetSceneNode == NULL))
		return false;

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return false;

	bool drawn = false;

	// ��ȿ object ����Ʈ
	MkPairArray<float, const MkSceneRenderObject*> validObjectList(MKDEF_DEF_OBJECT_BUFFER_SIZE);
	
	// camera AABR
	const MkFloat2& camSize = m_Camera.GetSize();
	MkFloat2 camPos = m_Camera.GetPosition() - camSize / 2.f;
	MkFloatRect camAABR(camPos, camSize);

	// scene node�� ���� ��ȿ object ����
	m_TargetSceneNode->__GetAllValidObjects(camAABR, validObjectList);

	if (!validObjectList.Empty())
	{
		// �ڿ� �ִ� object���� ���� �׷��� �ϱ� ������ �������� ����
		// ������ ���ڸ� opaque, line���� �������̹Ƿ� �տ������� �ڷ� ���� �׷��� ������ ���ڰ� �� ���� �����Ŷ� ����ǹǷ� ����
		validObjectList.SortInDescendingOrder();

		unsigned int validObjectCount = validObjectList.GetSize();
		MK_DRAWING_MONITOR.IncreaseValidObjectCounter(validObjectCount);

		// batch rendering
		// ����(material key)�� ���� object ���� ����
		// �ٸ� ���� ���̿� ������ġó�� �� ���°� �ƴ϶�� �ִ��� �ϳ��� ������ ���� ��ǥ
		const MkArray<const MkSceneRenderObject*>& objList = validObjectList.GetFields();

		typedef struct __MaterialLink
		{
			unsigned int allocCount;
			int currUnionIndex;
			int lastRefIndex;
		}
		_MaterialLink;

		// ������ ���� ������
		MkMap<MkSceneRenderObject::MaterialKey, _MaterialLink> materialLink; // map�� ���� �������� �ٸ� ������ �����̳ʰ� ����...;;
		MK_INDEXING_LOOP(objList, i)
		{
			const MkSceneRenderObject::MaterialKey& materialKey = objList[i]->__GetMaterialKey();
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
		MkArray< MkArray<const MkSceneRenderObject*> > unions(validObjectCount); // �־��� ��� ��� object�� ���������� ���� �� ����

		// ���� ������ object���� ��ȸ�ϸ� ������ ����Ʈ ����
		unsigned int j;
		bool newUnion;

		MK_INDEXING_LOOP(objList, i)
		{
			const MkSceneRenderObject::MaterialKey& materialKey = objList[i]->__GetMaterialKey();
			_MaterialLink& ml = materialLink[materialKey];

			// ���� �� ���� �׷����ų� ������ object�� �ٷ� ���
			if (ml.currUnionIndex == -1)
			{
				newUnion = true;
			}
			// �׷��� ������ ������ġ �������� �˻�
			else
			{
				newUnion = false;

				const MkFloatRect& currAABR = objList[i]->GetWorldAABR();

				// �ش� ������ ������ ���� ���� object ���ĺ��� �� rect ���� ��ȸ
				for (j=(ml.lastRefIndex + 1); j<i; ++j)
				{
					if (objList[j]->__GetMaterialKey() == materialKey)
					{
						if (ml.lastRefIndex == (i-1)) // ���� ������ ���ӵǸ� üũ��ġ ����
						{
							ml.lastRefIndex = i;
						}
					}
					else if (currAABR.CheckIntersection(objList[j]->GetWorldAABR())) // ������ �ٸ��� �浹 ����
					{
						// �浹�� �߻��ߴٸ� ���� ���հ� �� rect ���̿� ���̿� ������ġ ������ �ٸ� ������ ���� object�� �����Ѵٴ� �ǹ��̴� �� ���� ����
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

			MkArray<const MkSceneRenderObject*>& currUnion = unions[ml.currUnionIndex];
			currUnion.PushBack(objList[i]);
			--ml.allocCount;
		}

		// union�� ��ȸ�ϸ� �������� draw step�� ���� �� ��� ����
		MK_INDEXING_LOOP(unions, i)
		{
			MkSceneRenderObject* currPanel = const_cast<MkSceneRenderObject*>(unions[i][0]);
			currPanel->__ExcuteCustomDrawStep();
		}
		
		// draw object
		MK_DRAWING_MONITOR.IncreaseObjectUnionCounter(unions.GetSize());

		if (m_RenderTarget.__BeginScene())
		{
			DWORD rtClearFlag = D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
			if (m_ClearLastRenderTarget)
			{
				rtClearFlag |= D3DCLEAR_TARGET;
			}
			m_RenderTarget.__Clear(rtClearFlag);

			// camera
			m_Camera.UpdateViewProjectionMatrix();
			MK_RENDER_STATE.UpdateViewProjectionTransform(m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix());

			// union ��ȸ
			MK_INDEXING_LOOP(unions, i)
			{
				MkArray<const MkSceneRenderObject*>& currUnion = unions[i];

				// union�� ���� ������ �����ϹǷ� �ƹ��ų� �ϳ��� �����ϸ� ��
				currUnion[0]->__ApplyRenderState();

				// vertex buffer ������ draw call
				// rect�� line ������ ���ؽ����� �����ϰ� ��Ʈ�� �ϴ� �Ϳ� ���� DrawPrimitiveUP�� �ᵵ �����ս� ���̰� ���� ����
				unsigned int dataSize = 0;
				D3DPRIMITIVETYPE primType;
				unsigned int primCount;
				unsigned int stride;

				switch (currUnion[0]->GetObjectType())
				{
				case ePA_SOT_Panel:
					dataSize = currUnion.GetSize() * sizeof(MkPanel::VertexData) * 6;
					primType = D3DPT_TRIANGLELIST;
					primCount = currUnion.GetSize() * 2;
					stride = sizeof(MkPanel::VertexData);
					break;

				case ePA_SOT_LineShape:
					{
						unsigned int dataBlockCnt = 0;
						MK_INDEXING_LOOP(currUnion, i)
						{
							dataBlockCnt += currUnion[i]->__GetVertexDataBlockSize();
						}

						dataSize = dataBlockCnt * sizeof(MkLineShape::SegmentData);
						primType = D3DPT_LINELIST;
						primCount = dataBlockCnt;
						stride = sizeof(MkLineShape::PointData);
					}
					break;
				}

				if (dataSize > 0)
				{
					MkByteArray vertexData(dataSize);

					MK_INDEXING_LOOP(currUnion, j)
					{
						currUnion[j]->__FillVertexData(vertexData);
					}

					device->DrawPrimitiveUP(primType, primCount, vertexData.GetPtr(), stride);
				}
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
	m_ClearLastRenderTarget = false;
}

void MkDrawSceneNodeStep::_UpdateCameraInfo(void)
{
	MkInt2 ss = m_RenderTarget.GetScreenSize();
	MkFloat2 fss(static_cast<float>(ss.x), static_cast<float>(ss.y));

	m_Camera.SetSize(fss);
	m_Camera.SetPosition(fss / 2.f + m_CameraOffset);
}

//------------------------------------------------------------------------------------------------//
