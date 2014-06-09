#ifndef __MINIKEY_CORE_MKM128OPERATOR_H__
#define __MINIKEY_CORE_MKM128OPERATOR_H__

//------------------------------------------------------------------------------------------------//
// __m128 operator
//------------------------------------------------------------------------------------------------//

#include <xmmintrin.h>

__forceinline __m128 operator + (__m128 left, __m128 right) { return _mm_add_ps(left, right); }
__forceinline __m128 operator - (__m128 left, __m128 right) { return _mm_sub_ps(left, right); }
__forceinline __m128 operator * (__m128 left, __m128 right) { return _mm_mul_ps(left, right); }
__forceinline __m128 operator / (__m128 left, __m128 right) { return _mm_div_ps(left, right); }

__forceinline __m128 operator + (__m128 left, float right) { return _mm_add_ps(left, _mm_set_ps1(right)); }
__forceinline __m128 operator - (__m128 left, float right) { return _mm_sub_ps(left, _mm_set_ps1(right)); }
__forceinline __m128 operator * (__m128 left, float right) { return _mm_mul_ps(left, _mm_set_ps1(right)); }
__forceinline __m128 operator / (__m128 left, float right) { return _mm_div_ps(left, _mm_set_ps1(right)); }

__forceinline __m128 operator & (__m128 left, __m128 right) { return _mm_and_ps(left, right); }
__forceinline __m128 operator | (__m128 left, __m128 right) { return _mm_or_ps(left, right); }

__forceinline __m128 operator < (__m128 left, __m128 right) { return _mm_cmplt_ps(left, right); }
__forceinline __m128 operator > (__m128 left, __m128 right) { return _mm_cmpgt_ps(left, right); }
__forceinline __m128 operator <= (__m128 left, __m128 right) { return _mm_cmple_ps(left, right); }
__forceinline __m128 operator >= (__m128 left, __m128 right) { return _mm_cmpge_ps(left, right); }
__forceinline __m128 operator != (__m128 left, __m128 right) { return _mm_cmpneq_ps(left, right); }
__forceinline __m128 operator == (__m128 left, __m128 right) { return _mm_cmpeq_ps(left, right); }

//------------------------------------------------------------------------------------------------//

#endif
