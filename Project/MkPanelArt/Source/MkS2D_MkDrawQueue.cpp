
#include "MkCore_MkCheck.h"

#include "MkS2D_MkRenderStateSetter.h"
#include "MkS2D_MkDrawQueue.h"


//------------------------------------------------------------------------------------------------//

MkDrawStep* MkDrawQueue::CreateStep
(const MkHashStr& stepName, int priority, MkRenderTarget::eTargetType type, unsigned int count, const MkUInt2& size, MkRenderToTexture::eFormat texFormat)
{
	MK_CHECK(!m_NameTable.Exist(stepName), L"DrawQueue에 이미 " + stepName.GetString() + L" 이름을 가진 DrawStep이 존재")
		return NULL;

	if (priority < 0)
	{
		priority = m_Steps.Empty() ? 0 : (m_Steps.GetLastKey() + 1);
	}
	else
	{
		MK_CHECK(!m_Steps.Exist(priority), L"DrawQueue에 이미 " + MkStr(priority) + L" priority를 가진 DrawStep이 존재")
			return NULL;
	}

	MkDrawStep* step = new MkDrawStep(stepName);
	MK_CHECK(step != NULL, stepName.GetString() + L" DrawStep alloc 실패")
		return NULL;

	if (step->SetUp(type, count, size, texFormat))
	{
		m_NameTable.Create(stepName, priority);
		m_Steps.Create(priority, step);
	}
	else
	{
		delete step;
		step = NULL;
	}

	return step;
}

MkDrawStep* MkDrawQueue::GetStep(const MkHashStr& stepName)
{
	return m_NameTable.Exist(stepName) ? m_Steps[m_NameTable[stepName]] : NULL;
}

bool MkDrawQueue::RemoveStep(const MkHashStr& stepName)
{
	bool ok = m_NameTable.Exist(stepName);
	if (ok)
	{
		int priority = m_NameTable[stepName];
		delete m_Steps[priority];
		m_Steps.Erase(priority);
		m_NameTable.Erase(stepName);
	}
	return ok;
}

bool MkDrawQueue::Draw(void)
{
	bool drawn = false;

	if (!m_Steps.Empty())
	{
		MK_RENDER_STATE.ClearWorldTransform();

		MkMapLooper<int, MkDrawStep*> looper(m_Steps);
		MK_STL_LOOP(looper)
		{
			if (looper.GetCurrentField()->Draw())
			{
				drawn = true;
			}
		}
	}

	return drawn;
}

void MkDrawQueue::Clear(void)
{
	m_NameTable.Clear();

	MkMapLooper<int, MkDrawStep*> looper(m_Steps);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Steps.Clear();
}

//------------------------------------------------------------------------------------------------//
