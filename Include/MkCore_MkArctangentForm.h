#ifndef __MINIKEY_CORE_MKARCTANGENTFORM_H__
#define __MINIKEY_CORE_MKARCTANGENTFORM_H__

//------------------------------------------------------------------------------------------------//
// arctangent graph
// ���������� ���� �׷��� ����, ũ�� ����
//
// ex> -1.0, 1.0 -> full graph
// ex> 0.0, 1.0 -> �Ĺݺθ�
// ex> -0.2, 0.2 -> �Ϲ����� ��ũź��Ʈ �׷��� (�⺻ ��)
//------------------------------------------------------------------------------------------------//

class MkArctangentForm
{
public:

	// frontRange (in) : ���ݺ� ��� ����(-1.0 ~ 0.0)
	// backRange (in) : �Ĺݺ� ��� ����(0.0 ~ 1.0)
	bool SetUp(float frontRange = -0.2f, float backRange = 0.2f);

	// x�� �ش��ϴ� y�� ����
	// x : ���� �� �׷����� ����(0.0)�� ��(1.0) ������ ��. ������ �Ѿ ��� Ŭ����
	// (out) : 0.0 ~ 1.0 �� �׷��� ��. �׷��� ���̰� 0�� ���(frontRange�� backRange���� ���� 0�� ���) 0 ����
	float GetGraph(float x) const;

	MkArctangentForm() { SetUp(); }

protected:

	float m_FrontRange;
	float m_LengthOfRange;
	float m_BeginWeight;
	float m_EndWeight;
};

#endif
