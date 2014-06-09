#ifndef __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// source text -> memory
//
// 텍스트를 파싱해 바이너리 형태로 전환
// 일부 문법 검사만 수행한 다음 키워드를 1:1로 변환
// 따라서 만들어진 데이터는 최적화가 되지 않은 상태임
// 최적화 완료된 데이터를 원할 경우 노드로 로딩 한 후 memory로 변환 할 것
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStringTableForParser.h"


class MkPathName;
class MkInterfaceForDataWriting;

class MkDataTextToMemoryConverter
{
public:

	// text source file -> memory
	bool Convert(const MkPathName& filePath, MkByteArray& destination);
	bool Convert(const MkByteArray& source, MkByteArray& destination);

	void Clear(void);

	~MkDataTextToMemoryConverter() { Clear(); }

protected:

	bool _Convert(MkStr& textSource, const MkPathName& filePath, MkByteArray& destination);
	unsigned int _SplitBlock(const MkStr& source, unsigned int startPosition, int& blockType, MkStr& blockName, MkStr& templateName, MkStr& body) const;
	bool _ParseText(const MkStr& source, MkInterfaceForDataWriting& dwInterface) const;
	unsigned int _ParseBlock(const MkStr& source, MkInterfaceForDataWriting& dwInterface, unsigned int startPosition) const;

protected:

	MkStr m_TargetFilePath;
	MkStringTableForParser m_StringTable;
};

//------------------------------------------------------------------------------------------------//

#endif
