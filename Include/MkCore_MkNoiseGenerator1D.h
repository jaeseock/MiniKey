#ifndef __MINIKEY_CORE_MKNOISEGENERATOR1D_H__
#define __MINIKEY_CORE_MKNOISEGENERATOR1D_H__

//------------------------------------------------------------------------------------------------//
// 1���� �޸� ������ (Perlin noise) �߻���
// http://www.gpgstudy.com/gpgiki/PerlinNoise
//------------------------------------------------------------------------------------------------//

class MkNoiseGenerator1D
{
public:

	// (in) persistence : ������ (0.f < p < 1.f). Ŀ������ ���� ���Ұ� �����Ƿ� �ռ��� ������ Ŀ��
	// (in) numberOfOctaves : �ռ��� ��Ÿ�� �� (0 < n). Ŀ������ ������ ª�����Ƿ� ����� ����
	void SetUp(float persistence = 0.5f, int numberOfOctaves = 4);

	// ������ ����
	// 0�� �߽����� ���� ������ŭ ����. ������ �������� ��Ÿ�꿡 ���� ����
	// ex> p:0.5, n:4 -> (-1.f ~ 1.f)
	float GetNoise(float x);

	MkNoiseGenerator1D() { SetUp(); }

protected:

	float _SmoothedNoise(int x);
	float _InterpolatedNoise(float x);

protected:

	float m_Persistence;
	int m_NumberOfOctaves;
};

#endif
