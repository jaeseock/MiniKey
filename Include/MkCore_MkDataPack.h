#ifndef __MINIKEY_CORE_MKDATAPACK_H__
#define __MINIKEY_CORE_MKDATAPACK_H__


//------------------------------------------------------------------------------------------------//
// data pack
// data unit �����
// �� ���� <key, data unit> pair�� ����, ���ٽ� key�� ���
//
// ���ڿ��� �ƴ� native ���·� ����
//
// �ϰ� �ʱ�ȭ, �ϰ� ����, �幮 ������ �߻��� ���̶� ������
//
// �� ������ ���� dynamic_cast�� 1ȸ �Ҹ�ǹǷ� ���� �ǽð� ������ ���� ����� ������
// ���� single data�� �ƴ� array ���·� �Ǿ� �ִٸ� �ϳ��� �����ϴ� ��� �迭�� ���� �ϰ�ó�� ����
//
// �����ϴ� unit�� �ּ��� �ϳ��� data�� ����
//
// MkDataNode�� ���� �����̳������� ��屸���� �ʿ� ���� ��� �ܵ����ε� ��� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkDataTypeDefinition.h"


class MkDataUnitInterface;

class MkDataPack
{
public:

	// unit ���� ����
	bool IsValidKey(const MkHashStr& key) const;

	// unit �� ��ȯ
	unsigned int GetUnitCount(void) const;

	// unit data ���� ��ȯ
	unsigned int GetDataSize(const MkHashStr& key) const;

	// unit data type ��ȯ
	ePrimitiveDataType GetUnitType(const MkHashStr& key) const;
	
	// ��ϵ� ��� unit key ��ȯ
	unsigned int GetUnitKeyList(MkArray<MkHashStr>& keyList) const;

	// size��ŭ�� ũ�⸦ ���� data unit�� ����
	// return : ���� ����. �̹� �����ϴ� unit�� �ְų� ������ �ʴ� type�� ��� ����
	bool CreateUnit(const MkHashStr& key, ePrimitiveDataType type, unsigned int size);

	// �����ϴ� unit�� data ũ�⸦ size��ŭ Ȯ��
	// return : ���� ����. unit�� �������� ������ ����
	bool Expand(const MkHashStr& key, unsigned int size);

	// single data �Ҵ� (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��ų� index�� data ������ ��� ��� ����
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

	// data array �Ҵ� (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��� ����
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

	// single data ��ȯ (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��ų� index�� data ������ ��� ��� ����
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

	// data array ��ȯ (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��� ����
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

	// unit �����ϰ� �������� ��ȯ
	bool RemoveUnit(const MkHashStr& key);

	// single data �����ϰ� �������� ��ȯ
	// �����ϴ� ������ ��(0�� index)���� ��� �����Ǿ��ٸ� ��ü������ unit ����(RemoveUnit()�� ����)
	bool RemoveData(const MkHashStr& key, unsigned int index);

	// �˻� ����ȭ
	// data unit�� ����/������ �Ϸ�Ǹ� ȣ�� ����
	void Rehash(void);

	// ����
	void Clear(void);

	// unit�� ���纻 ��ȯ
	MkDataUnitInterface* GetUnitCopy(const MkHashStr& key) const;

	// ���� unit���� �Ǵ� (dynamic cast 1ȸ)
	bool Equals(const MkHashStr& key, const MkDataUnitInterface& source) const;
	bool Equals(const MkDataPack& source, const MkHashStr& key) const;

	// ���� unit type, key���� �Ǵ�. ���� �޶� ��� ����
	bool SameForm(const MkDataPack& source) const;

	// �Ҵ�
	MkDataPack& operator = (const MkDataPack& source);

	// ��
	bool operator == (const MkDataPack& source) const;
	inline bool operator != (const MkDataPack& source) const { return !operator==(source); }

	// �ڽſ��Ը� �����ϰų� source�� data�� �ٸ� unit key list ��ȯ
	void GetValidUnitList(const MkDataPack& source, MkArray<MkHashStr>& keyList) const;

	MkDataPack() {}
	~MkDataPack() { Clear(); }

protected:

	MkHashMap<MkHashStr, MkDataUnitInterface*> m_UnitTable;
};

//------------------------------------------------------------------------------------------------//

#endif
