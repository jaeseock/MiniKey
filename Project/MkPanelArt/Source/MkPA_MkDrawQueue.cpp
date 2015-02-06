
#include "MkCore_MkCheck.h"

#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkDrawQueue.h"


//------------------------------------------------------------------------------------------------//

MkDrawSceneNodeStep* MkDrawQueue::CreateDrawSceneNodeStep
(const MkHashStr& stepName, int priority, MkRenderTarget::eTargetType type, unsigned int count, const MkInt2& size, const MkFloat2& camOffset, MkRenderToTexture::eFormat texFormat)
{
	MK_CHECK(!m_NameTable.Exist(stepName), L"MkDrawQueue에 이미 " + stepName.GetString() + L" 이름을 가진 draw step이 존재")
		return NULL;

	if (priority < 0)
	{
		priority = m_Steps.Empty() ? 0 : (m_Steps.GetLastKey() + 1);
	}
	else
	{
		MK_CHECK(!m_Steps.Exist(priority), L"MkDrawQueue에 이미 " + MkStr(priority) + L" priority를 가진 draw step이 존재")
			return NULL;
	}

	MkDrawSceneNodeStep* step = new MkDrawSceneNodeStep;
	MK_CHECK(step != NULL, stepName.GetString() + L" MkDrawSceneNodeStep alloc 실패")
		return NULL;

	if (step->SetUp(type, count, size, camOffset, texFormat))
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

MkDrawStepInterface* MkDrawQueue::GetStep(const MkHashStr& stepName)
{
	return m_NameTable.Exist(stepName) ? m_Steps[m_NameTable[stepName]] : NULL;
}

void MkDrawQueue::RemoveStep(const MkHashStr& stepName)
{
	if (m_NameTable.Exist(stepName))
	{
		int priority = m_NameTable[stepName];
		delete m_Steps[priority];
		m_Steps.Erase(priority);
		m_NameTable.Erase(stepName);
	}
}

bool MkDrawQueue::Draw(void)
{
	bool drawn = false;

	if (!m_Steps.Empty())
	{
		MkMapLooper<int, MkDrawStepInterface*> looper(m_Steps);
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

	MkMapLooper<int, MkDrawStepInterface*> looper(m_Steps);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Steps.Clear();
}

//------------------------------------------------------------------------------------------------//
