#ifndef __MINIKEY_CORE_MKSTANDARDLINEARGRAPH_H__
#define __MINIKEY_CORE_MKSTANDARDLINEARGRAPH_H__

//------------------------------------------------------------------------------------------------//
// ������ Line ������ 2D �׷���
// �ʱ�ȭ�� ������(x, y), Ȥ�� ���������� ����Ʈ
// x�� ���� �־� ����/�ڻ��� ������ y ���� ����
// ���� ����� ū ��� �� ������ ȿ������
//
// ex>
//	MkStandardLinearGraph lg;
//	lg.AddPoint(1, 10.f);
//	lg.AddPoint(2, 20.f);
//	lg.AddPoint(3, 30.f);
//	lg.AddPoint(4, 40.f);
//
//	float val = lg.GetValue(2.3f);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"


class MkStandardLinearGraph
{
public:

	// ������(x, y)�� ����
	virtual void AddPoint(float x, float y);

	// ������(x, y) ����Ʈ�� ����
	void AddPointList(MkMap<float, float>& inputList);

	// x, y���� ���� ���� ������ ����
	virtual void SetAxisWeight(float x_weight = 1.f, float y_weight = 1.f);

	// ����
	void Clear(void);

	// x���� ������ ���� ����
	// �� ����Ʈ�� ����� ��� 0.f ��ȯ
	float GetLastX(void) const;

	// �Էµ� x���� ���� ���� ������ y���� ��ȯ
	// �� ����Ʈ�� ����� ��� 0.f ��ȯ
	// useCosineInterpolation : true�� ��� �ڻ��� ������ �ε巯�� ���� ��ȯ, false�̸� �������� ���� ��ȯ
	float GetValue(float x, bool useCosineInterpolation = false) const;

	// empty
	inline bool Empty(void) const { return m_Table.Empty(); }

protected:

	MkMap<float, float> m_Table;
};

#endif
