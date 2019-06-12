#ifndef __MINIKEY_CORE_MKDATAPACK_H__
#define __MINIKEY_CORE_MKDATAPACK_H__


//------------------------------------------------------------------------------------------------//
// data pack
// data unit 저장소
// 각 값은 <key, data unit> pair로 존재, 접근시 key로 사용
//
// 문자열이 아닌 native 형태로 저장
//
// 일괄 초기화, 일괄 참조, 드문 수정이 발생할 것이라 간주함
//
// 값 참조시 마다 dynamic_cast가 1회 소모되므로 잦은 실시간 참조는 높은 비용을 수반함
// 따라서 single data가 아닌 array 형태로 되어 있다면 하나씩 접근하는 대신 배열을 통한 일괄처리 권장
//
// 존재하는 unit은 최소한 하나의 data를 가짐
//
// MkDataNode를 위한 컨테이너이지만 노드구조가 필요 없을 경우 단독으로도 사용 가능
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkDataTypeDefinition.h"


class MkDataUnitInterface;

class MkDataPack
{
public:

	// unit 존재 여부
	bool IsValidKey(const MkHashStr& key) const;

	// unit 수 반환
	unsigned int GetUnitCount(void) const;

	// unit data 갯수 반환
	unsigned int GetDataSize(const MkHashStr& key) const;

	// unit data type 반환
	ePrimitiveDataType GetUnitType(const MkHashStr& key) const;
	
	// 등록된 모든 unit key 반환
	unsigned int GetUnitKeyList(MkArray<MkHashStr>& keyList) const;

	// size만큼의 크기를 가진 data unit을 생성
	// return : 성공 여부. 이미 존재하는 unit이 있거나 허용되지 않는 type일 경우 실패
	bool CreateUnit(const MkHashStr& key, ePrimitiveDataType type, unsigned int size);

	// 존재하는 unit의 data 크기를 size만큼 확장
	// return : 성공 여부. unit이 존재하지 않으면 실패
	bool Expand(const MkHashStr& key, unsigned int size);

	// single data 할당 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르거나 index가 data 범위를 벗어난 경우 실패
	bool SetData(const MkHashStr& key, bool value, unsigned int index);
	bool SetData(const MkHashStr& key, int value, unsigned int index);
	bool SetData(const MkHashStr& key, unsigned int value, unsigned int index);
	bool SetData(const MkHashStr& key, __int64 value, unsigned int index);
	bool SetData(const MkHashStr& key, unsigned __int64 value, unsigned int index);
	bool SetData(const MkHashStr& key, float value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkInt2& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkVec2& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkVec3& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkStr& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkByteArray& value, unsigned int index);

	// data array 할당 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르면 실패
	bool SetData(const MkHashStr& key, const MkArray<bool>& values);
	bool SetData(const MkHashStr& key, const MkArray<int>& values);
	bool SetData(const MkHashStr& key, const MkArray<unsigned int>& values);
	bool SetData(const MkHashStr& key, const MkArray<__int64>& values);
	bool SetData(const MkHashStr& key, const MkArray<unsigned __int64>& values);
	bool SetData(const MkHashStr& key, const MkArray<float>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkInt2>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkVec2>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkVec3>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkStr>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkByteArray>& values);

	// single data 반환 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르거나 index가 data 범위를 벗어난 경우 실패
	bool GetData(const MkHashStr& key, bool& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, int& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, unsigned int& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, __int64& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, unsigned __int64& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, float& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkInt2& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkVec2& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkVec3& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkStr& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkByteArray& buffer, unsigned int index) const;

	// data array 반환 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르면 실패
	bool GetData(const MkHashStr& key, MkArray<bool>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<int>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<unsigned int>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<__int64>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<unsigned __int64>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<float>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkInt2>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkVec2>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkVec3>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkStr>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkByteArray>& buffers) const;

	// unit 제거하고 성공여부 반환
	bool RemoveUnit(const MkHashStr& key);

	// single data 제거하고 성공여부 반환
	// 존재하는 유일한 값(0번 index)가지 모두 삭제되었다면 자체적으로 unit 제거(RemoveUnit()과 동일)
	bool RemoveData(const MkHashStr& key, unsigned int index);

	// 검색 최적화
	// data unit의 생성/삭제가 완료되면 호출 권장
	void Rehash(void);

	// 해제
	void Clear(void);

	// unit의 복사본 반환
	MkDataUnitInterface* GetUnitCopy(const MkHashStr& key) const;

	// 동일 unit여부 판단 (dynamic cast 1회)
	bool Equals(const MkHashStr& key, const MkDataUnitInterface& source) const;
	bool Equals(const MkDataPack& source, const MkHashStr& key) const;

	// 동일 unit type, key인지 판단. 값은 달라도 상관 없음
	bool SameForm(const MkDataPack& source) const;

	// 할당
	MkDataPack& operator = (const MkDataPack& source);

	// 비교
	bool operator == (const MkDataPack& source) const;
	inline bool operator != (const MkDataPack& source) const { return !operator==(source); }

	// 자신에게만 존재하거나 source와 data가 다른 unit key list 반환
	void GetValidUnitList(const MkDataPack& source, MkArray<MkHashStr>& keyList) const;

	MkDataPack() {}
	~MkDataPack() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkDataUnitInterface*> m_UnitTable;
};

//------------------------------------------------------------------------------------------------//

#endif
