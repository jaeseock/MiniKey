#ifndef __MINIKEY_CORE_MKCURVEINTERPOLATOR_H__
#define __MINIKEY_CORE_MKCURVEINTERPOLATOR_H__


//------------------------------------------------------------------------------------------------//
// � ������
// ���� ���� data type�� ��ȭ �� �� �־�� �ϱ� ������ template ���
//
// �⺻������ float, MkVec2, MkVec3 ���(�� ���� type�� SetUp�� ������ ����)
// �߰����� data type�� ����ϰ� ������ �Լ� Ư��ȭ ���� �ʿ�
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkArray.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"


//------------------------------------------------------------------------------------------------//

template <class DataType>
class MkCurveInterpolator
{
public:

	// �ʱ�ȭ
	// keys : Ű�� ����� ����Ʈ
	// values : ���� ����� ����Ʈ
	// interpolationType : ���� ���
	// outOfRangeAction : �Է� ������ ���� �� �ൿ���
	// (NOTE) keys�� values�� ������ ���� �ּ� 2�� �̻� �Ǿ�� �ϰ� ������ �����ؾ� ��
	bool SetUp
		(const MkArray<float>& keys, const MkArray<DataType>& values,
		eCurveInterpolationType interpolationType, eCurveOutOfRangeAction frontsideCORA, eCurveOutOfRangeAction rearsideCORA)
	{
		if (!__AvailableType())
			return false;

		unsigned int inputCount = keys.GetSize();
		if ((inputCount == 0) || (inputCount != values.GetSize()))
			return false;

		// �Է°� �Ҵ�
		m_Keys = keys;
		m_Values = values;

		if (inputCount == 1)
		{
			m_InterpolationType = eCIT_Constant; // �Է°��� 1���� 2, 3�� ������ �� �� ����
		}
		else if ((inputCount == 2) && (interpolationType == eCIT_Cubic))
		{
			m_InterpolationType = eCIT_Linear; // �Է°��� 2���� 3�� ������ �� �� ����
		}
		else
		{
			m_InterpolationType = interpolationType;
		}
		m_FrontsideOutOfRangeAction = frontsideCORA;
		m_RearsideOutOfRangeAction = rearsideCORA;

		m_StartKey = m_Keys[0];
		m_EndKey = m_Keys[inputCount-1];

		// ���� ���� ��� ����
		if (m_InterpolationType == eCIT_Linear)
		{
			unsigned int lastIndex = inputCount - 1;
			m_Factor.Reserve(lastIndex);
			for (unsigned int i=0; i<lastIndex; ++i)
			{
				m_Factor.PushBack(_GetDelta(i));
			}
		}
		// ���ö��� ���� ��� ����
		else if (m_InterpolationType == eCIT_Cubic)
		{
			unsigned int last1 = inputCount - 1;
			unsigned int last2 = inputCount - 2;
			m_Factor.Fill(inputCount);

			m_Factor[0] = _GetDelta(0);
			m_Factor[last1] = _GetDelta(last2);

			for (unsigned int i=1; i<last1; ++i)
			{
				m_Factor[i] =
					(m_Values[i] - m_Values[i-1]) / (2.f * (m_Keys[i] - m_Keys[i-1])) +
					(m_Values[i+1] - m_Values[i]) / (2.f * (m_Keys[i+1] - m_Keys[i]));
			}

			for (unsigned int i=0; i<last1; ++i)
			{
				__UpdateFactorForCube(i);
			}
		}

		return true;
	}

	// ����
	void Clear(void)
	{
		m_Keys.Clear();
		m_Values.Clear();
		m_Factor.Clear();
		m_StartKey = m_EndKey = 0.f;
	}

	// ���� ���� ���α�Ģ ����
	inline void SetFrontsideOutOfRangeAction(eCurveOutOfRangeAction frontsideCORA) { m_FrontsideOutOfRangeAction = frontsideCORA; }

	// ���� ���� ���α�Ģ ����
	inline void SetRearsideOutOfRangeAction(eCurveOutOfRangeAction rearsideCORA) { m_RearsideOutOfRangeAction = rearsideCORA; }

	// �ش� ������ �ش��ϴ� ���� ��ȯ
	DataType GetValue(float currentKey) const
	{
		DataType resultValue;
		__ClearValue(resultValue);
		if (m_Keys.Empty())
			return resultValue;

		if (currentKey < m_StartKey) // �������� ����
		{
			switch (m_FrontsideOutOfRangeAction)
			{
			case eCORA_NearValue: resultValue = _GetFirstValue(); break;
			case eCORA_Repetition: resultValue = _GetValue(_ConvertRepetitionKeyInRange(currentKey)); break;
			case eCORA_Mirror: resultValue = _GetValue(_ConvertMirrorKeyInRange(currentKey)); break;
			}
		}
		else if (currentKey > m_EndKey) // �������� ����
		{
			switch (m_RearsideOutOfRangeAction)
			{
			case eCORA_NearValue: resultValue = _GetLastValue(); break;
			case eCORA_Repetition: resultValue = _GetValue(_ConvertRepetitionKeyInRange(currentKey)); break;
			case eCORA_Mirror: resultValue = _GetValue(_ConvertMirrorKeyInRange(currentKey)); break;
			}
		}
		else // ���� ��
		{
			resultValue = _GetValue(currentKey);
		}
		return resultValue;
	}

	// ��밡�ɿ��� ��ȯ
	inline bool GetAvailable(void) const { return (!m_Keys.Empty()); }

	// Ư��ȭ�� ����Լ�
	bool __AvailableType(void) { return false; }
	void __UpdateFactorForCube(unsigned int index) {}
	void __ClearValue(T& value) const {}

	MkCurveInterpolator()
	{
		m_InterpolationType = eCIT_Constant;
		m_FrontsideOutOfRangeAction = eCORA_NearValue;
		m_RearsideOutOfRangeAction = eCORA_NearValue;

		m_EndKey = m_StartKey = 0.f;
	}

protected:

	inline DataType _GetDelta(unsigned int index) const
	{
		return (m_Values[index+1] - m_Values[index]) / (m_Keys[index+1] - m_Keys[index]);
	}

	inline void _UpdateSingleFactorForCube(float delta, float& currFactor, float& nextFactor)
	{
		if(MkFloatOp::CloseToZero(delta))
		{
            currFactor = 0.f;
			nextFactor = 0.f;
        }
		else
		{
			float invDelta = 1.f / delta;
			float cd = currFactor * invDelta;
			float nd = nextFactor * invDelta;
			float squared = cd * cd + nd * nd;
			if (squared > 9.f)
			{
				float t = 3.f / sqrtf(squared);
				currFactor = t * cd * delta;
				nextFactor = t * nd * delta;
			}
		}
	}

	inline const DataType& _GetFirstValue(void) const { return m_Values[0]; }
	inline const DataType& _GetLastValue(void) const { return m_Values[m_Values.GetSize() - 1]; }

	inline float _ConvertRepetitionKeyInRange(float currentKey) const
	{
		return (BsFloatOperation::GetRemainder(currentKey, m_EndKey - m_StartKey) + m_StartKey);
	}

	inline float _ConvertMirrorKeyInRange(float currentKey) const
	{
		float singleDuration = m_EndKey - m_StartKey;
		float doubleDuration = singleDuration * 2.f;
		float newKey = BsFloatOperation::GetRemainder(currentKey, doubleDuration);
		if (newKey >= singleDuration)
		{
			newKey = doubleDuration - newKey;
		}
		return (newKey + m_StartKey);
	}

	DataType _GetValue(float currentKey) const
	{
		DataType resultValue;
		__ClearValue(resultValue);

		if (currentKey <= m_StartKey)
		{
			resultValue = _GetFirstValue();
		}
		else if (currentKey >= m_EndKey)
		{
			resultValue = _GetLastValue();
		}
		else
		{
			unsigned int lowerBoundIndex = m_Keys.FindLowerBound(currentKey);
			switch (m_InterpolationType)
			{
			case eCIT_Constant:
				resultValue = m_Values[lowerBoundIndex];
				break;

			case eCIT_Linear:
				resultValue = m_Values[lowerBoundIndex] + m_Factor[lowerBoundIndex] * (currentKey - m_Keys[lowerBoundIndex]);
				break;

			case eCIT_Cubic:
				{
					unsigned int upperBoundIndex = lowerBoundIndex + 1;
					float stepLength = m_Keys[upperBoundIndex] - m_Keys[lowerBoundIndex];
					float keyRate = (currentKey - m_Keys[lowerBoundIndex]) / stepLength;
					float invKeyRate = 1.f - keyRate;
					float f0 = 2.f * keyRate * keyRate * keyRate - 3.f * keyRate * keyRate + 1.f;
					float f1 = keyRate * invKeyRate * invKeyRate;
					float f2 = keyRate * keyRate * (3.f - 2.f * keyRate);
					float f3 = keyRate * keyRate * (keyRate - 1.f);
					resultValue =
						m_Values[lowerBoundIndex] * f0 +
						stepLength * m_Factor[lowerBoundIndex] * f1 +
						m_Values[upperBoundIndex] * f2 +
						stepLength * m_Factor[upperBoundIndex] * f3;
				}
				break;
			}
		}
		
		return resultValue;
	}

protected:

	MkArray<float> m_Keys;
	MkArray<DataType> m_Values;
	MkArray<DataType> m_Factor;

	eCurveInterpolationType m_InterpolationType;
	eCurveOutOfRangeAction m_FrontsideOutOfRangeAction;
	eCurveOutOfRangeAction m_RearsideOutOfRangeAction;

	float m_StartKey;
	float m_EndKey;
};

//------------------------------------------------------------------------------------------------//
// float Ư��ȭ
//------------------------------------------------------------------------------------------------//

template <> bool MkCurveInterpolator<float>::__AvailableType(void) { return true; }

template <> void MkCurveInterpolator<float>::__UpdateFactorForCube(unsigned int index)
{
	_UpdateSingleFactorForCube(_GetDelta(index), m_Factor[index], m_Factor[index+1]);
}

template <> void MkCurveInterpolator<float>::__ClearValue(float& value) const { value = 0.f; }

//------------------------------------------------------------------------------------------------//
// MkVec2 Ư��ȭ
//------------------------------------------------------------------------------------------------//

template <> bool MkCurveInterpolator<MkVec2>::__AvailableType(void) { return true; }

template <> void MkCurveInterpolator<MkVec2>::__UpdateFactorForCube(unsigned int index)
{
	MkVec2 delta = _GetDelta(index);
	MkVec2& currFactor = m_Factor[index];
	MkVec2& nextFactor = m_Factor[index+1];
	_UpdateSingleFactorForCube(delta.x, currFactor.x, nextFactor.x);
	_UpdateSingleFactorForCube(delta.y, currFactor.y, nextFactor.y);
}

//------------------------------------------------------------------------------------------------//
// MkVec3 Ư��ȭ
//------------------------------------------------------------------------------------------------//

template <> bool MkCurveInterpolator<MkVec3>::__AvailableType(void) { return true; }

template <> void MkCurveInterpolator<MkVec3>::__UpdateFactorForCube(unsigned int index)
{
	MkVec3 delta = _GetDelta(index);
	MkVec3& currFactor = m_Factor[index];
	MkVec3& nextFactor = m_Factor[index+1];
	_UpdateSingleFactorForCube(delta.x, currFactor.x, nextFactor.x);
	_UpdateSingleFactorForCube(delta.y, currFactor.y, nextFactor.y);
	_UpdateSingleFactorForCube(delta.z, currFactor.z, nextFactor.z);
}

//------------------------------------------------------------------------------------------------//

#endif
