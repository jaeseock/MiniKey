#ifndef __MINIKEY_CORE_MKCOLOR_H__
#define __MINIKEY_CORE_MKCOLOR_H__

//------------------------------------------------------------------------------------------------//
// float 기반의 color(r, g, b, a)
// - 범위는 0.f ~ 1.f
// - COLORREF, D3DCOLOR 타입과의 상호 변환
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkContainerDefinition.h"
#include "MkCore_MkGlobalFunction.h"


class MkColor
{
public:

	//------------------------------------------------------------------------------------------------//
	// 생성자
	//------------------------------------------------------------------------------------------------//

	inline MkColor()
	{
		r = 0.f;
		g = 0.f;
		b = 0.f;
		a = 1.f;
	}

	inline MkColor(const float& red, const float& green, const float& blue) { _Set(red, green, blue, 1.f); }
	inline MkColor(const float& red, const float& green, const float& blue, const float& alpha) { _Set(red, green, blue, alpha); }
	inline MkColor(const MkColor& color) { *this = color; }

	inline MkColor& operator = (const MkColor& color)
	{
		 _Set(color.r, color.g, color.b, color.a);
		return *this;
	}

	//------------------------------------------------------------------------------------------------//
	
	inline bool operator == (const MkColor& color) const { return ((r == color.r) && (g == color.g) && (b == color.b) && (a == color.a)); }
	inline bool operator != (const MkColor& color) const { return ((r != color.r) || (g != color.g) || (b != color.b) || (a != color.a)); }

	inline MkColor operator + (const MkColor& color) const { return MkColor(r + color.r, g + color.g, b + color.b, a + color.a); }
	inline MkColor operator - (const MkColor& color) const { return MkColor(r - color.r, g - color.g, b - color.b, a - color.a); }
	inline MkColor operator * (const float& value) const { return MkColor(r * value, g * value, b * value, a); }
	inline friend MkColor operator * (const float& value, const MkColor& color) { return MkColor(value * color.r, value * color.g, value * color.b, value * color.a); }
	inline MkColor operator / (const float& value) const { return MkColor(r / value, g / value, b / value, a / value); }

	inline MkColor& operator += (const MkColor& color)
	{
		r = Clamp<float>(r + color.r, 0.f, 1.f);
		g = Clamp<float>(g + color.g, 0.f, 1.f);
		b = Clamp<float>(b + color.b, 0.f, 1.f);
		a = Clamp<float>(a + color.a, 0.f, 1.f);
		return *this;
	}

	inline MkColor& operator -= (const MkColor& color)
	{
		r = Clamp<float>(r - color.r, 0.f, 1.f);
		g = Clamp<float>(g - color.g, 0.f, 1.f);
		b = Clamp<float>(b - color.b, 0.f, 1.f);
		a = Clamp<float>(a - color.a, 0.f, 1.f);
		return *this;
	}

	inline MkColor& operator *= (const float& value)
	{
		r = Clamp<float>(r * value, 0.f, 1.f);
		g = Clamp<float>(g * value, 0.f, 1.f);
		b = Clamp<float>(b * value, 0.f, 1.f);
		a = Clamp<float>(a * value, 0.f, 1.f);
		return *this;
	}

	inline MkColor& operator /= (const float& value)
	{
		r = Clamp<float>(r / value, 0.f, 1.f);
		g = Clamp<float>(g / value, 0.f, 1.f);
		b = Clamp<float>(b / value, 0.f, 1.f);
		a = Clamp<float>(a / value, 0.f, 1.f);
		return *this;
	}

	//------------------------------------------------------------------------------------------------//

	inline void SetRGB(const MkColor& color)
	{
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline void SetAlpha(const float& alpha) { a = Clamp<float>(alpha, 0.f, 1.f); }
	inline void SetAlpha(const MkColor& color) { a = color.a; }

	//------------------------------------------------------------------------------------------------//

	// COLORREF[X|B|G|R]로 초기화
	inline void SetByCOLORREF(unsigned int color)
	{
		r = static_cast<float>(color & 0xff) * _DivFactor;
		g = static_cast<float>((color >> 8) & 0xff) * _DivFactor;
		b = static_cast<float>((color >> 16) & 0xff) * _DivFactor;
	}

	// COLORREF[X|B|G|R]로 변환된 값 반환
	inline unsigned int ConvertToCOLORREF(void) const
	{
		return
			(static_cast<unsigned int>(r * 255.f) |
			(static_cast<unsigned int>(g * 255.f) << 8) |
			(static_cast<unsigned int>(b * 255.f) << 16));
	}

	// D3DCOLOR[A|R|G|B]로 초기화
	inline void SetByD3DCOLOR(unsigned int color)
	{
		r = static_cast<float>((color >> 16) & 0xff) * _DivFactor;
		g = static_cast<float>((color >> 8) & 0xff) * _DivFactor;
		b = static_cast<float>(color & 0xff) * _DivFactor;
		a = static_cast<float>((color >> 24) & 0xff) * _DivFactor;
	}

	// D3DCOLOR[A|R|G|B]로 변환된 값 반환
	inline unsigned int ConvertToD3DCOLOR(void) const
	{
		return
			((static_cast<unsigned int>(a * 255.f) << 24) |
			(static_cast<unsigned int>(r * 255.f) << 16) |
			(static_cast<unsigned int>(g * 255.f) << 8) |
			static_cast<unsigned int>(b * 255.f));
	}

	//------------------------------------------------------------------------------------------------//

protected:

	inline void _Set(const float& red, const float& green, const float& blue, const float& alpha)
	{
		r = Clamp<float>(red, 0.f, 1.f);
		g = Clamp<float>(green, 0.f, 1.f);
		b = Clamp<float>(blue, 0.f, 1.f);
		a = Clamp<float>(alpha, 0.f, 1.f);
	}

public:

	//------------------------------------------------------------------------------------------------//

	float r, g, b, a;

	static const float _DivFactor;

	// 상수
	static const MkColor Empty;
	static const MkColor Black;
	static const MkColor White;
	static const MkColor LightGray;
	static const MkColor DarkGray;
	static const MkColor Red;
	static const MkColor Green;
	static const MkColor Blue;
	static const MkColor Yellow;
	static const MkColor Cian;
	static const MkColor Pink;
	static const MkColor Violet;
	static const MkColor Orange;
};

//------------------------------------------------------------------------------------------------//

// 고정크기
MKDEF_DECLARE_FIXED_SIZE_TYPE(MkColor)

//------------------------------------------------------------------------------------------------//

#endif
