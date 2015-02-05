#pragma once


//------------------------------------------------------------------------------------------------//
// (PVC)static image를 만들기 위한 draw scene
//
// alpha에 따라 세가지 종류가 존재
// - 알파 없음 : _SetNoneAlphaArguments()로 설정
// - 일반적인 알파채널을 사용 : _SetAlphaChannelArguments()로 설정
// - 특정 컬러키를 알파값으로 사용 : _SetAlphaKeyArguments()로 설정
//
// 실제 그리기 위한 명령은 _DrawContents()를 재정의해 사용 할 것
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkDrawStepInterface.h"


class MkDrawToStaticTextureStep : public MkDrawStepInterface
{
public:

	// 결과 텍스쳐 반환
	virtual void GetTargetTexture(unsigned int index, MkBaseTexturePtr& buffer) const { buffer = m_TargetTexture; }

	// 그리기
	virtual bool Draw(void);

	// 해제
	virtual void Clear(void);

	MkDrawToStaticTextureStep();
	virtual ~MkDrawToStaticTextureStep() { Clear(); }

protected:

	enum eAlphaType
	{
		eNone = 0, // 알파 없음
		eAlphaChannel, // 일반적인 알파채널을 사용
		eAlphaKey // 특정 컬러키를 알파값으로 사용
	};

	void _SetNoneAlphaArguments(const MkInt2& size, DWORD bgColor);
	void _SetAlphaChannelArguments(const MkInt2& size, DWORD bgColor);
	void _SetAlphaKeyArguments(const MkInt2& size, DWORD alphaKey);

	// (PVF) 실제 그릴 명령을 여기서 실행
	// 반환값은 성공 여부
	virtual bool _DrawContents(void) = NULL;

	// 텍스쳐 객체 생성
	// (NOTE) 호출 전 _SetNoneAlphaArguments()/_SetAlphaChannelArguments()/_SetAlphaKeyArguments() 중 하나가 실행된 상태이어야 함
	bool _CreateTexture(void);

protected:

	MkInt2 m_TextureSize;
	eAlphaType m_AlphaType;
	DWORD m_BackgroundColor;
	DWORD m_AlphaKey;

	MkBaseTexturePtr m_TargetTexture;

	bool m_GenerateTexture;
};
