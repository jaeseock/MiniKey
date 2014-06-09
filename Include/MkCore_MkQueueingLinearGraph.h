#ifndef __MINIKEY_CORE_MKQUEUEINGLINEARGRAPH_H__
#define __MINIKEY_CORE_MKQUEUEINGLINEARGRAPH_H__

//------------------------------------------------------------------------------------------------//
// �ֽ� ���� ���������� �߰��Ǵ� Line ������ 2D �׷���
// �߰��Ǵ� ������(x, y)�� x�� �׻� ���� x���� Ŀ�� ��
// x�� ���� �־� ����/�ڻ��� ������ y ���� ����
//
// �ֽ� x�� ���� �������� ������ ������ �ʿ� ���� ���� ������ �ڵ� �����ϰ� ���鿡 ������ ���� ����
// ex>
//	����Ʈ�� (1.f, 0.f), (2.f, 1.f), (4.f, 3.f), (4.5f, 2.f)�� ��� �ְ� ������ 5.f��,
//	���ο� (8.f, 2.5f)�� �߰��� �� (1.f, 0.f), (2.f, 1.f)���� ���� �ǰ� (3.f, 2.f)�� ���� ��
//	�� (10.f, 0.f)�� �߰��Ǹ� (3.f, 2.f), (4.f, 3.f), (4.5f, 2.f)�� �����ǰ� (5.f, 2.1428571f)�� ���� ��
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStandardLinearGraph.h"


class MkQueueingLinearGraph : public MkStandardLinearGraph
{
public:

	// �ڵ� ������ ������ ����
	// 0.f �����̸� �ڵ� ���� ��� ��� �� ��
	void SetClippingRange(float clippingRange);
 
	// ������(x, y)�� ����
	// ��ȯ���� �߰� ��������
	// (NOTE) x�� �׻� ���� ������ ���� x���� Ŀ�� ��
	virtual void AddPoint(float x, float y);

	// x, y���� ���� ���� ������ ����
	// �ڵ� ���� ������ ���� �� ��� x_weight�� ���߾� �缳����
	virtual void SetAxisWeight(float x_weight = 1.f, float y_weight = 1.f);

	MkQueueingLinearGraph() : MkStandardLinearGraph() { m_ClippingRange = 0.f; }

protected:

	void _ClipListByRange(float range);

protected:

	float m_ClippingRange;
};

#endif
