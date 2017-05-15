#ifndef __MINIKEY_CORE_MKINTERFACEFORDATAREADING_H__
#define __MINIKEY_CORE_MKINTERFACEFORDATAREADING_H__


//------------------------------------------------------------------------------------------------//
// MkByteArray에서 MkInterfaceForDataWriting로 기록된 데이터를 추출해내기 위한 interface
// 기록시의 동일 순서, 동일 자료형으로 Read()를 호출
//
// ex>
//	// MkByteArray(srcArray) -> data
//	bool boolOut;
//	int intOut;
//	unsigned int uintOut;
//	__int64 dintOut;
//	unsigned __int64 duintOut;
//	float floatOut;
//	MkInt2 ptOut;
//	MkVec2 v2Out;
//	MkVec3 v3Out;
//	MkStr strOut;
//
//	MkInterfaceForDataReading drInterface;
//	drInterface.SetUp(srcArray, 0);
//
//	drInterface.Read(boolOut);
//	drInterface.Read(intOut);
//	drInterface.Read(uintOut);
//	drInterface.Read(dintOut);
//	drInterface.Read(duintOut);
//	drInterface.Read(floatOut);
//	drInterface.Read(ptOut);
//	drInterface.Read(v2Out);
//	drInterface.Read(v3Out);
//	drInterface.Read(strOut);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkArray.h"


class MkVec2;
class MkVec3;
class MkStr;

class MkInterfaceForDataReading
{
public:

	// 초기화
	// sourcePtr은 MkInterfaceForDataWriting로 기록된 데이터를 가지고 있는 유효한 byte array
	// 반환값은 정상 초기화 여부
	bool SetUp(const MkByteArray& srcArray, unsigned int beginPosition = 0);

	// 타입에 따른 값 읽기
	// 반환값은 정상 진행 여부
	// (NOTE) 읽기 수행중 false가 반환되었으면 이후 읽기는 정상수행을 보장할 수 없음
	bool Read(bool& buffer);
	bool Read(int& buffer);
	bool Read(unsigned int& buffer);
	bool Read(__int64& buffer);
	bool Read(unsigned __int64& buffer);
	bool Read(float& buffer);
	bool Read(MkInt2& buffer);
	bool Read(MkVec2& buffer);
	bool Read(MkVec3& buffer);
	bool Read(MkStr& buffer);

	// 호환성을 위한 외부 버퍼에 읽기
	bool Read(unsigned char* buffer, unsigned int size);

	// 최종위치 도달여부
	bool CheckEndOfBlock(void) const;

	// 현재 탐색 위치 이동
	void SetCurrentPosition(unsigned int newPosition);

	// 현재 탐색 위치 반환
	unsigned int GetCurrentPosition(void) const;

	MkInterfaceForDataReading();

protected:

	MkByteArray* m_SourcePtr;
	unsigned int m_BeginPosition;
	unsigned int m_CurrentPosition;
	unsigned int m_EndPosition;
};

//------------------------------------------------------------------------------------------------//

#endif
