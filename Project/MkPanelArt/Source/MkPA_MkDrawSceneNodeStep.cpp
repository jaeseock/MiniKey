
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

	// 유효 object 리스트
	MkPairArray<float, const MkSceneRenderObject*> validObjectList(MKDEF_DEF_OBJECT_BUFFER_SIZE);
	
	// camera AABR
	const MkFloat2& camSize = m_Camera.GetSize();
	MkFloat2 camPos = m_Camera.GetPosition() - camSize / 2.f;
	MkFloatRect camAABR(camPos, camSize);

	// scene node에 속한 유효 object 수집
	m_TargetSceneNode->__GetAllValidObjects(camAABR, validObjectList);

	if (!validObjectList.Empty())
	{
		// 뒤에 있는 object부터 먼저 그려야 하기 때문에 내림차순 정렬
		// 엄밀히 하자면 opaque, line들은 불투명이므로 앞에서부터 뒤로 먼저 그려야 하지만 숫자가 얼마 되지 않을거라 예상되므로 무시
		validObjectList.SortInDescendingOrder();

		unsigned int validObjectCount = validObjectList.GetSize();
		MK_DRAWING_MONITOR.IncreaseValidObjectCounter(validObjectCount);

		// batch rendering
		// 재질(material key)에 따른 object 집합 구성
		// 다른 재질 사이에 샌드위치처럼 낀 상태가 아니라면 최대한 하나로 모으는 것이 목표
		const MkArray<const MkSceneRenderObject*>& objList = validObjectList.GetFields();

		typedef struct __MaterialLink
		{
			unsigned int allocCount;
			int currUnionIndex;
			int lastRefIndex;
		}
		_MaterialLink;

		// 재질별 정보 보관소
		MkMap<MkSceneRenderObject::MaterialKey, _MaterialLink> materialLink; // map은 심히 느리지만 다른 적합한 컨테이너가 없음...;;
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

		// 실제 집합이 저장 될 이중 테이블
		MkArray< MkArray<const MkSceneRenderObject*> > unions(validObjectCount); // 최악의 경우 모든 object가 독자적으로 잡힐 수 있음

		// 가장 뒤쪽의 object부터 순회하며 재질별 리스트 구성
		unsigned int j;
		bool newUnion;

		MK_INDEXING_LOOP(objList, i)
		{
			const MkSceneRenderObject::MaterialKey& materialKey = objList[i]->__GetMaterialKey();
			_MaterialLink& ml = materialLink[materialKey];

			// 재질 별 최초 그려지거나 마지막 object면 바로 등록
			if (ml.currUnionIndex == -1)
			{
				newUnion = true;
			}
			// 그렇지 않으면 샌드위치 상태인지 검사
			else
			{
				newUnion = false;

				const MkFloatRect& currAABR = objList[i]->GetWorldAABR();

				// 해당 재질의 마지막 집합 시작 object 이후부터 현 rect 사이 순회
				for (j=(ml.lastRefIndex + 1); j<i; ++j)
				{
					if (objList[j]->__GetMaterialKey() == materialKey)
					{
						if (ml.lastRefIndex == (i-1)) // 같은 재질이 연속되면 체크위치 갱신
						{
							ml.lastRefIndex = i;
						}
					}
					else if (currAABR.CheckIntersection(objList[j]->GetWorldAABR())) // 재질이 다르면 충돌 검출
					{
						// 충돌이 발생했다면 현재 집합과 현 rect 사이에 사이에 샌드위치 상태인 다른 재질을 가진 object가 존재한다는 의미이니 새 집합 시작
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

		// union을 순회하며 독자적인 draw step이 존재 할 경우 실행
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

			// union 순회
			MK_INDEXING_LOOP(unions, i)
			{
				MkArray<const MkSceneRenderObject*>& currUnion = unions[i];

				// union은 동일 재질을 참조하므로 아무거나 하나만 적용하면 됨
				currUnion[0]->__ApplyRenderState();

				// vertex buffer 구성해 draw call
				// rect나 line 종류는 버텍스버퍼 세심하게 콘트를 하는 것에 비해 DrawPrimitiveUP를 써도 퍼포먼스 차이가 거의 없음
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
