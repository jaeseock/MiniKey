#pragma once

//------------------------------------------------------------------------------------------------//
// ������ ���μ��� ó��
//------------------------------------------------------------------------------------------------//

class MkStr;

class MkWindowProcessOp
{
public:
	
	// ���μ��� ���ۿ��� ��ȯ
	static bool Exist(const MkStr& processName);

	// �������� ���μ��� ���� ��ȯ
	// ���и� -1, ������ 0 ��ȯ
	static int Count(const MkStr& processName);

	// �������� ���μ��� ����
	// ���и� -1, �ƴϸ� ���� ���μ��� �� ��ȯ
	static int Kill(const MkStr& processName);
};
