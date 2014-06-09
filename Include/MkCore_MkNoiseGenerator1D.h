#ifndef __MINIKEY_CORE_MKNOISEGENERATOR1D_H__
#define __MINIKEY_CORE_MKNOISEGENERATOR1D_H__

//------------------------------------------------------------------------------------------------//
// 1차원 펄린 노이즈 (Perlin noise) 발생기
// http://www.gpgstudy.com/gpgiki/PerlinNoise
//------------------------------------------------------------------------------------------------//

class MkNoiseGenerator1D
{
public:

	// (in) persistence : 감쇠율 (0.f < p < 1.f). 커질수록 진폭 감소가 낮으므로 합성된 진폭이 커짐
	// (in) numberOfOctaves : 합성할 옥타브 수 (0 < n). 커질수록 파장이 짧아지므로 운동성이 높음
	void SetUp(float persistence = 0.5f, int numberOfOctaves = 4);

	// 노이즈 생성
	// 0을 중심으로 일정 진폭만큼 리턴. 진폭은 감쇠율과 옥타브에 따라 결정
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
