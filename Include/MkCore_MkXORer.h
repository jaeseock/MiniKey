#ifndef __MINIKEY_CORE_MKXORER_H__
#define __MINIKEY_CORE_MKXORER_H__

//------------------------------------------------------------------------------------------------//
// XORing 인터페이스
// XORing의 효율을 높이기 위해서는(뚫기 힘들게 하기 위해서는) key가 길 것, 예상되는 패턴이 없을 것,
// 변환 후에도 해석이 되지 않는 등의 조건이 많아질수록 좋음
// - key는 많이 쓰이는 ascii를 기피 하기를 권장
// - key가 문자열일 경우 가급적 잘 안쓰이는 문자 형식을 사용하기를 권장(영어는 피하는게 좋음)
// - 언어별로 많이 쓰이는 패턴이 있으므로(영어의 경우 문장에서는 공문자(space)가 가장 많고 그 다음이 'e')
//   가설을 세울 수 있는 여지를 주지 말 것
// - 비용을 각오한다면 2중 이상의 변환을 하되, 각 key의 형식을 다르게 할 것(1차는 한국어, 2차는 일본어 등)
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkArray.h"


class MkStr;

class MkXORer
{
public:

	// 변환
	// (in/out) targetArray : 변환될 배열
	// (in) key : 변환 key
	static void Convert(MkByteArray& targetArray, const MkByteArray& key);
	static void Convert(MkByteArray& targetArray, const MkStr& key);
};

#endif
