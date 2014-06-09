#ifndef __MINIKEY_CORE_MKNOISEGENERATOR2D_H__
#define __MINIKEY_CORE_MKNOISEGENERATOR2D_H__

//------------------------------------------------------------------------------------------------//
// 2���� �޸� ������ (Perlin noise) �߻���
// http://www.gpgstudy.com/gpgiki/PerlinNoise
//------------------------------------------------------------------------------------------------//

class MkNoiseGenerator2D
{
public:

	// (in) persistence : ������ (0.f < p < 1.f). Ŀ������ ���� ���Ұ� �����Ƿ� �ռ��� ������ Ŀ��
	// (in) numberOfOctaves : �ռ��� ��Ÿ�� �� (0 < n). Ŀ������ ������ ª�����Ƿ� ����� ����
	void SetUp(float persistence = 0.5f, int numberOfOctaves = 4);

	// ������ ����
	// 0�� �߽����� ���� ������ŭ ����. ������ �������� ��Ÿ�꿡 ���� ����
	// ex> p:0.5, n:4 -> (-1.f ~ 1.f)
	float GetNoise(float x, float y);

	MkNoiseGenerator2D() { SetUp(); }

protected:

	float _MakeRandom(int x, int y);
	float _SmoothedNoise(int x, int y);
	float _InterpolatedNoise(float x, float y);

protected:

	float m_Persistence;
	int m_NumberOfOctaves;
};

#endif
