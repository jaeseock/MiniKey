#ifndef __MINIKEY_CORE_MKMEMORYTODATATEXTCONVERTER_H__
#define __MINIKEY_CORE_MKMEMORYTODATATEXTCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// memory -> source text
//
// - 가독성에 중점
// - 배열형 unit의 경우 주석으로 크기를 표시
// - 템플릿 적용된 노드의 경우 유닛, 자식 노드들이 원본 템플릿과 동일할 경우 생략됨
// - 출력 순서는,
//   0. 철자 순서대로 정렬된 unit
//   1. 먼저 호출되는 순서대로 정렬된 템플릿 노드
//   2. 철자 순서대로 정렬된 일반 노드
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"

class MkPathName;
class MkInterfaceForDataReading;

class MkMemoryToDataTextConverter
{
public:

	// memory -> text source file
	bool Convert(const MkByteArray& srcArray, const MkPathName& filePath);

protected:

	void _InitiateHeaderMsg(MkStr& strBuffer, const MkPathName& filePath) const;
	bool _BuildText(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const;
	unsigned int _BuildBlock(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const;
};

//------------------------------------------------------------------------------------------------//

#endif
