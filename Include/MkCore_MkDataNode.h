#ifndef __MINIKEY_CORE_MKDATANODE_H__
#define __MINIKEY_CORE_MKDATANODE_H__


//------------------------------------------------------------------------------------------------//
// data node
//
// primitive type data를 저장하기 위한 자료형
// - 각 노드는 트리구조를 이룸
// - 노드명의 조건은 형제(동일 부모를 둔 동일 깊이의 노드)들 사이에서 유일해야 함
// - 노드는 primitive type data를 unit이란 이름의 key-field 형태로 저장하며 필드는 배열행태가 가능
// - 유닛명의 조건은 형제(동일 노드에 속한 유닛)들 사이에서 유일해야 함
// - 노드명은 공문자가 허용되지만 유닛명에서는 공문자 허용하지 않음
// - 각 노드는 독립적으로 메모리를 소유하기 때문에 부모에서 분리되어도 유지됨
//
// template 기능
// - 정해진 포맷이 존재 할 경우 포맷을 템플릿 노드라는 이름으로 정의해놓고 동일 폼의 노드가
//   필요할 때 복사해 기본 틀을 만듬
// - 템플릿은 선언 이후 변경 불가
// - 템플릿으로부터 선 정의된 노드는 해당 포맷을 깨트리는 행위를 할 수 없음
//   (고유 노드/유닛 추가 및 삭제, 유닛 값 변경만 가능. 값 수정과 확장만 허용함)
// - include file 개념을 도입하면 유용하지만 이런 기능은 복원성을 어렵게 만든다. 즉 source file -> data node
//   변환시에는 문제가 없지만 data node -> source file 과정에서 모든 구문들의 출처를 요구하게 된다.
//   그런데 이는 주석처럼 사용에는 아무런 필요 없는 무의미한 값이기 때문에 include 기능과 복원성을 동시에
//   가져가려면 데이터의 낭비를 불러 일으키게 되므로 여기서는 포기한다
//
// 입출력 기능
// - node <-> binary data <-> binary file(real/file system)
//                        <-> text data <-> text file(real/file system)
//                        <-  excel file(real)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingleTypeTreePattern.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataPack.h"


class MkDataNode : public MkSingleTypeTreePattern<MkDataNode>
{
public:

	//------------------------------------------------------------------------------------------------//
	// 파일, 데이터로부터 초기화. 반환값은 성공여부
	// 파일 로딩은 MkFileManager를 통해 실행됨으로 실제 경로, 혹은 패키징(MkFileSystem)에서 추출
	// 다른 방식의 파일 관리 시스템을 쓸 경우 데이터로부터 로딩하는 인터페이스 사용 가능
	// (NOTE) 하위 unit과 자식 노드들을 대상으로 함
	//------------------------------------------------------------------------------------------------//

	// 실제 혹은 파일 시스템상의 파일, 데이터로부터 템플릿만 읽어들여 등록
	static bool RegisterTemplate(const MkPathName& filePath);
	static bool RegisterTemplate(const MkByteArray& fileData);

	// 실제 혹은 파일 시스템상의 text format 파일, 데이터로부터 초기화
	bool LoadFromText(const MkPathName& filePath);
	bool LoadFromText(const MkByteArray& fileData);

	// 실제 혹은 파일 시스템상의 binary format 파일, 데이터로부터 초기화
	bool LoadFromBinary(const MkPathName& filePath);
	bool LoadFromBinary(const MkByteArray& fileData);

	// 실제 경로상의 Excel 파일(*.xls)로부터 초기화
	bool LoadFromExcel(const MkPathName& filePath);

	// 파일(text, binary, *.xls), 데이터(text, binary) format을 자동 판단하여 초기화
	bool Load(const MkPathName& filePath);
	bool Load(const MkByteArray& fileData);

	//------------------------------------------------------------------------------------------------//
	// 파일로 출력. 반환값은 성공여부
	// 템플릿으로부터 선 정의된 노드일 경우 고유값을 가지지 못한 unit, node들은 출력되지 않음
	// (NOTE) 하위 unit과 자식 노드들을 대상으로 함
	//------------------------------------------------------------------------------------------------//

	// text file로 출력
	bool SaveToText(const MkPathName& filePath) const;

	// binary file로 출력
	bool SaveToBinary(const MkPathName& filePath) const;

	//------------------------------------------------------------------------------------------------//
	// 링크 수정은 템플릿 포맷이 보존되는 케이스만 허용
	//------------------------------------------------------------------------------------------------//

	// 노드 이름 변경
	// (NOTE) 해당 노드가 템플릿에 의해 정의된 노드일 경우 실패
	virtual bool ChangeNodeName(const MkHashStr& newName);

	// 자식 노드 이름 변경
	// (NOTE) 해당 자식 노드가 템플릿에 의해 정의된 노드일 경우 실패
	virtual bool ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName);

	// childNodeName을 가진 직계 자식 노드를 떼어냄
	// (NOTE) 템플릿에 의해 선 정의된 노드일 경우 실패
	virtual bool DetachChildNode(const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// unit & data 접근
	// 템플릿 완전성이 보존되는 케이스만 허용
	//------------------------------------------------------------------------------------------------//

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

	// single data로 data unit 생성
	// return : 성공 여부. read-only 상태이거나 이미 존재하는 unit이 있으면 실패
	bool CreateUnit(const MkHashStr& key, bool value);
	bool CreateUnit(const MkHashStr& key, int value);
	bool CreateUnit(const MkHashStr& key, unsigned int value);
	bool CreateUnit(const MkHashStr& key, float value);
	bool CreateUnit(const MkHashStr& key, const MkInt2& value);
	bool CreateUnit(const MkHashStr& key, const MkVec2& value);
	bool CreateUnit(const MkHashStr& key, const MkVec3& value);
	bool CreateUnit(const MkHashStr& key, const MkStr& value);
	bool CreateUnitEx(const MkHashStr& key, const MkFloat2& value);
	bool CreateUnitEx(const MkHashStr& key, const MkHashStr& value);

	// data array로 data unit 생성
	// return : 성공 여부. read-only 상태이거나 이미 존재하는 unit이 있으면 실패
	bool CreateUnit(const MkHashStr& key, const MkArray<bool>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<int>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<unsigned int>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<float>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<MkInt2>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<MkVec2>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<MkVec3>& values);
	bool CreateUnit(const MkHashStr& key, const MkArray<MkStr>& values);
	bool CreateUnitEx(const MkHashStr& key, const MkArray<MkFloat2>& values);
	bool CreateUnitEx(const MkHashStr& key, const MkArray<MkHashStr>& values);

	// 존재하는 unit의 data 크기를 size만큼 확장
	// return : 성공 여부. read-only 상태이거나 unit이 존재하지 않으면 실패
	bool Expand(const MkHashStr& key, unsigned int size);

	// single data 할당 (dynamic cast 1회)
	// return : 성공 여부. read-only 상태이거나 unit이 존재하지 않거나 타입이 다르거나 index가 data 범위를 벗어난 경우 실패
	bool SetData(const MkHashStr& key, bool value, unsigned int index);
	bool SetData(const MkHashStr& key, int value, unsigned int index);
	bool SetData(const MkHashStr& key, unsigned int value, unsigned int index);
	bool SetData(const MkHashStr& key, float value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkInt2& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkVec2& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkVec3& value, unsigned int index);
	bool SetData(const MkHashStr& key, const MkStr& value, unsigned int index);
	bool SetDataEx(const MkHashStr& key, const MkFloat2& value, unsigned int index);
	bool SetDataEx(const MkHashStr& key, const MkHashStr& value, unsigned int index);

	// data array 할당 (dynamic cast 1회)
	// return : 성공 여부. read-only 상태이거나 unit이 존재하지 않거나 타입이 다르면 실패
	bool SetData(const MkHashStr& key, const MkArray<bool>& values);
	bool SetData(const MkHashStr& key, const MkArray<int>& values);
	bool SetData(const MkHashStr& key, const MkArray<unsigned int>& values);
	bool SetData(const MkHashStr& key, const MkArray<float>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkInt2>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkVec2>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkVec3>& values);
	bool SetData(const MkHashStr& key, const MkArray<MkStr>& values);
	bool SetDataEx(const MkHashStr& key, const MkArray<MkFloat2>& values);
	bool SetDataEx(const MkHashStr& key, const MkArray<MkHashStr>& values);

	// single data 반환 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르거나 index가 data 범위를 벗어난 경우 실패
	bool GetData(const MkHashStr& key, bool& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, int& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, unsigned int& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, float& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkInt2& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkVec2& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkVec3& buffer, unsigned int index) const;
	bool GetData(const MkHashStr& key, MkStr& buffer, unsigned int index) const;
	bool GetDataEx(const MkHashStr& key, MkFloat2& buffer, unsigned int index) const;
	bool GetDataEx(const MkHashStr& key, MkHashStr& buffer, unsigned int index) const;

	// data array 반환 (dynamic cast 1회)
	// return : 성공 여부. unit이 존재하지 않거나 타입이 다르면 실패
	bool GetData(const MkHashStr& key, MkArray<bool>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<int>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<unsigned int>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<float>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkInt2>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkVec2>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkVec3>& buffers) const;
	bool GetData(const MkHashStr& key, MkArray<MkStr>& buffers) const;
	bool GetDataEx(const MkHashStr& key, MkArray<MkFloat2>& buffers) const;
	bool GetDataEx(const MkHashStr& key, MkArray<MkHashStr>& buffers) const;

	// unit 제거하고 성공여부 반환
	// (NOTE) read-only 상태이거나 템플릿에 의해 선 정의된 unit일 경우 실패
	bool RemoveUnit(const MkHashStr& key);

	// single data 제거하고 성공여부 반환
	// 존재하는 유일한 값(0번 index)이 삭제되었다면 unit 제거(RemoveUnit()과 동일)
	// (NOTE) read-only 상태이거나 템플릿에 의해 선 정의된 unit일 경우 존재하는 유일한 값이 삭제되는 케이스는 실패
	bool RemoveData(const MkHashStr& key, unsigned int index);

	// unit & child node가 전혀 없는지 여부 반환
	bool Empty(void) const;

	// 검색 최적화
	// unit, 하위 노드의 삭제/추가가 완료되어 변동이 없을 경우 호출
	// 구조 변경 없이 data만 변경될 경우는 호출 필요 없음
	// read-only 상태이면 무시
	virtual void Rehash(void);

	// 해제. 소속 unit 및 하위 모든 node 삭제
	// 부모 정보와 이름은 유지
	// read-only 상태이면 무시
	virtual void Clear(void);

	// data pack 참조 반환
	inline const MkDataPack& GetDataPack(void) const { return m_DataPack; }

	//------------------------------------------------------------------------------------------------//
	// assign
	//------------------------------------------------------------------------------------------------//

	// source로부터 할당
	// deep copy
	// read-only 상태이면 무시
	// 노드 이름과 부모, read-only 속성을 제외한 모든 정보를 그대로 복사
	MkDataNode& operator = (const MkDataNode& source);

	//------------------------------------------------------------------------------------------------//
	// template
	//------------------------------------------------------------------------------------------------//

	// 현 노드를 템플릿 노드로 선언하고 성공 여부 반환
	// read-only 상태이거나 동일 이름을 가진 노드가 이미 템플릿으로 등록되어 있으면 실패
	// checkDuplication : true면 동일 이름의 템플릿이 이미 존재 할 경우 경고, false면 무시
	bool DeclareToTemplate(bool checkDuplication = false) const;

	// 현 노드의 기존 데이터를 삭제하고 템플릿 노드로부터 다시 구성해 성공여부 반환
	// read-only 상태이거나 이미 템플릿이 적용되어 있거나 등록되지 않은 템플릿 노드로 시도하면 실패
	bool ApplyTemplate(const MkHashStr& templateName);

	// 현 노드가 템플릿 노드인지 여부 반환
	bool IsTemplateNode(void) const;

	// 템플릿의 루트 적용여부와 해당 템플릿 이름을 반환(템플릿이 적용되었더라도 루트가 아니라면 false 반환)
	bool GetAppliedTemplateName(MkHashStr& name) const;

	// 해당 노드가 템플릿에 의해 정의된 노드인지 여부 반환
	bool IsDefinedNodeByTemplate(void) const;

	// 현재 매치중인 하위 템플릿 노드 주소 반환
	inline const MkDataNode* GetTemplateLink(void) const { return m_TemplateLink; }

	// 현재 등록되어 있는 모든 템플릿 노드와 우선순위 정보 해제
	static void ClearAllTemplate(void);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	// 템플릿 노드로부터 복사
	void __CopyFromTemplateNode(const MkDataNode* source);
	
	// 유효한 모든 unit key 반환
	// 자신에게만 존재하거나 적용된 템플릿 노드와 값이 다른 경우에 해당
	// 템플릿 미적용 노드인 경우 GetUnitKeyList(keyList)와 동일
	void __GetValidUnitList(MkArray<MkHashStr>& keyList) const;

	// 해당 노드 유효성(고유 정보 소유) 여부
	// 템플릿 미적용 노드인 경우 항상 true이며 템플릿 적용 노드는 하위에 독자적인 unit이나 node가 있어야만 true
	bool __IsValidNode(void) const;

	// 자식 노드 중 유효한 노드들의 이름을 종류별/철자별로 정렬해 반환
	// 템플릿 노드들이 등록된 순서대로 앞에 위치하고 일반 노드들이 철자별로 정렬되에 뒤에 위치
	void __GetClassifiedChildNodeNameList(MkArray<MkHashStr>& templateNodeList, MkArray<MkHashStr>& normalNodeList) const;

	//------------------------------------------------------------------------------------------------//

	MkDataNode();
	MkDataNode(const MkHashStr& name);
	virtual ~MkDataNode()
	{
		m_ReadOnly = false;
		Clear();
	}

protected:

	static MkDataNode& _GetTemplateRoot(void);
	static MkHashMap<MkHashStr, unsigned int>& _GetPriorityTable(void);

	bool _PredefinedUnitByTemplate(const MkHashStr& key) const;
	bool _PredefinedChildNodeByTemplate(const MkHashStr& nodeName) const;

protected:

	MkDataPack m_DataPack;

	// 현 노드와 매치되는 동일 레벨 템플릿 노드 주소
	MkDataNode* m_TemplateLink;
};

//------------------------------------------------------------------------------------------------//

#endif
