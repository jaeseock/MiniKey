#ifndef __MINIKEY_CORE_MKDATANODE_H__
#define __MINIKEY_CORE_MKDATANODE_H__


//------------------------------------------------------------------------------------------------//
// data node
//
// primitive type data�� �����ϱ� ���� �ڷ���
// - �� ���� Ʈ�������� �̷�
// - ������ ������ ����(���� �θ� �� ���� ������ ���)�� ���̿��� �����ؾ� ��
// - ���� primitive type data�� unit�̶� �̸��� key-field ���·� �����ϸ� �ʵ�� �迭���°� ����
// - ���ָ��� ������ ����(���� ��忡 ���� ����)�� ���̿��� �����ؾ� ��
// - ������ �����ڰ� �������� ���ָ����� ������ ������� ����
// - �� ���� ���������� �޸𸮸� �����ϱ� ������ �θ𿡼� �и��Ǿ ������
//
// template ���
// - ������ ������ ���� �� ��� ������ ���ø� ����� �̸����� �����س��� ���� ���� ��尡
//   �ʿ��� �� ������ �⺻ Ʋ�� ����
// - ���ø��� ���� ���� ���� �Ұ�
// - ���ø����κ��� �� ���ǵ� ���� �ش� ������ ��Ʈ���� ������ �� �� ����
//   (���� ���/���� �߰� �� ����, ���� �� ���游 ����. �� ������ Ȯ�常 �����)
// - include file ������ �����ϸ� ���������� �̷� ����� �������� ��ư� �����. �� source file -> data node
//   ��ȯ�ÿ��� ������ ������ data node -> source file �������� ��� �������� ��ó�� �䱸�ϰ� �ȴ�.
//   �׷��� �̴� �ּ�ó�� ��뿡�� �ƹ��� �ʿ� ���� ���ǹ��� ���̱� ������ include ��ɰ� �������� ���ÿ�
//   ���������� �������� ���� �ҷ� ����Ű�� �ǹǷ� ���⼭�� �����Ѵ�
//
// ����� ���
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
	// ����, �����ͷκ��� �ʱ�ȭ. ��ȯ���� ��������
	// ���� �ε��� MkFileManager�� ���� ��������� ���� ���, Ȥ�� ��Ű¡(MkFileSystem)���� ����
	// �ٸ� ����� ���� ���� �ý����� �� ��� �����ͷκ��� �ε��ϴ� �������̽� ��� ����
	// (NOTE) ���� unit�� �ڽ� ������ ������� ��
	//------------------------------------------------------------------------------------------------//

	// ���� Ȥ�� ���� �ý��ۻ��� ����, �����ͷκ��� ���ø��� �о�鿩 ���
	static bool RegisterTemplate(const MkPathName& filePath);
	static bool RegisterTemplate(const MkByteArray& fileData);

	// ���� Ȥ�� ���� �ý��ۻ��� text format ����, �����ͷκ��� �ʱ�ȭ
	bool LoadFromText(const MkPathName& filePath);
	bool LoadFromText(const MkByteArray& fileData);

	// ���� Ȥ�� ���� �ý��ۻ��� binary format ����, �����ͷκ��� �ʱ�ȭ
	bool LoadFromBinary(const MkPathName& filePath);
	bool LoadFromBinary(const MkByteArray& fileData);

	// ���� ��λ��� Excel ����(*.xls)�κ��� �ʱ�ȭ
	bool LoadFromExcel(const MkPathName& filePath);

	// ����(text, binary, *.xls), ������(text, binary) format�� �ڵ� �Ǵ��Ͽ� �ʱ�ȭ
	bool Load(const MkPathName& filePath);
	bool Load(const MkByteArray& fileData);

	//------------------------------------------------------------------------------------------------//
	// ���Ϸ� ���. ��ȯ���� ��������
	// ���ø����κ��� �� ���ǵ� ����� ��� �������� ������ ���� unit, node���� ��µ��� ����
	// (NOTE) ���� unit�� �ڽ� ������ ������� ��
	//------------------------------------------------------------------------------------------------//

	// text file�� ���
	bool SaveToText(const MkPathName& filePath) const;

	// binary file�� ���
	bool SaveToBinary(const MkPathName& filePath) const;

	//------------------------------------------------------------------------------------------------//
	// ��ũ ������ ���ø� ������ �����Ǵ� ���̽��� ���
	//------------------------------------------------------------------------------------------------//

	// ��� �̸� ����
	// (NOTE) �ش� ��尡 ���ø��� ���� ���ǵ� ����� ��� ����
	virtual bool ChangeNodeName(const MkHashStr& newName);

	// �ڽ� ��� �̸� ����
	// (NOTE) �ش� �ڽ� ��尡 ���ø��� ���� ���ǵ� ����� ��� ����
	virtual bool ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName);

	// childNodeName�� ���� ���� �ڽ� ��带 ���
	// (NOTE) ���ø��� ���� �� ���ǵ� ����� ��� ����
	virtual bool DetachChildNode(const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// unit & data ����
	// ���ø� �������� �����Ǵ� ���̽��� ���
	//------------------------------------------------------------------------------------------------//

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

	// single data�� data unit ����
	// return : ���� ����. read-only �����̰ų� �̹� �����ϴ� unit�� ������ ����
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

	// data array�� data unit ����
	// return : ���� ����. read-only �����̰ų� �̹� �����ϴ� unit�� ������ ����
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

	// �����ϴ� unit�� data ũ�⸦ size��ŭ Ȯ��
	// return : ���� ����. read-only �����̰ų� unit�� �������� ������ ����
	bool Expand(const MkHashStr& key, unsigned int size);

	// single data �Ҵ� (dynamic cast 1ȸ)
	// return : ���� ����. read-only �����̰ų� unit�� �������� �ʰų� Ÿ���� �ٸ��ų� index�� data ������ ��� ��� ����
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

	// data array �Ҵ� (dynamic cast 1ȸ)
	// return : ���� ����. read-only �����̰ų� unit�� �������� �ʰų� Ÿ���� �ٸ��� ����
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

	// single data ��ȯ (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��ų� index�� data ������ ��� ��� ����
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

	// data array ��ȯ (dynamic cast 1ȸ)
	// return : ���� ����. unit�� �������� �ʰų� Ÿ���� �ٸ��� ����
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

	// unit �����ϰ� �������� ��ȯ
	// (NOTE) read-only �����̰ų� ���ø��� ���� �� ���ǵ� unit�� ��� ����
	bool RemoveUnit(const MkHashStr& key);

	// single data �����ϰ� �������� ��ȯ
	// �����ϴ� ������ ��(0�� index)�� �����Ǿ��ٸ� unit ����(RemoveUnit()�� ����)
	// (NOTE) read-only �����̰ų� ���ø��� ���� �� ���ǵ� unit�� ��� �����ϴ� ������ ���� �����Ǵ� ���̽��� ����
	bool RemoveData(const MkHashStr& key, unsigned int index);

	// unit & child node�� ���� ������ ���� ��ȯ
	bool Empty(void) const;

	// �˻� ����ȭ
	// unit, ���� ����� ����/�߰��� �Ϸ�Ǿ� ������ ���� ��� ȣ��
	// ���� ���� ���� data�� ����� ���� ȣ�� �ʿ� ����
	// read-only �����̸� ����
	virtual void Rehash(void);

	// ����. �Ҽ� unit �� ���� ��� node ����
	// �θ� ������ �̸��� ����
	// read-only �����̸� ����
	virtual void Clear(void);

	// data pack ���� ��ȯ
	inline const MkDataPack& GetDataPack(void) const { return m_DataPack; }

	//------------------------------------------------------------------------------------------------//
	// assign
	//------------------------------------------------------------------------------------------------//

	// source�κ��� �Ҵ�
	// deep copy
	// read-only �����̸� ����
	// ��� �̸��� �θ�, read-only �Ӽ��� ������ ��� ������ �״�� ����
	MkDataNode& operator = (const MkDataNode& source);

	//------------------------------------------------------------------------------------------------//
	// template
	//------------------------------------------------------------------------------------------------//

	// �� ��带 ���ø� ���� �����ϰ� ���� ���� ��ȯ
	// read-only �����̰ų� ���� �̸��� ���� ��尡 �̹� ���ø����� ��ϵǾ� ������ ����
	// checkDuplication : true�� ���� �̸��� ���ø��� �̹� ���� �� ��� ���, false�� ����
	bool DeclareToTemplate(bool checkDuplication = false) const;

	// �� ����� ���� �����͸� �����ϰ� ���ø� ���κ��� �ٽ� ������ �������� ��ȯ
	// read-only �����̰ų� �̹� ���ø��� ����Ǿ� �ְų� ��ϵ��� ���� ���ø� ���� �õ��ϸ� ����
	bool ApplyTemplate(const MkHashStr& templateName);

	// �� ��尡 ���ø� ������� ���� ��ȯ
	bool IsTemplateNode(void) const;

	// ���ø��� ��Ʈ ���뿩�ο� �ش� ���ø� �̸��� ��ȯ(���ø��� ����Ǿ����� ��Ʈ�� �ƴ϶�� false ��ȯ)
	bool GetAppliedTemplateName(MkHashStr& name) const;

	// �ش� ��尡 ���ø��� ���� ���ǵ� ������� ���� ��ȯ
	bool IsDefinedNodeByTemplate(void) const;

	// ���� ��ġ���� ���� ���ø� ��� �ּ� ��ȯ
	inline const MkDataNode* GetTemplateLink(void) const { return m_TemplateLink; }

	// ���� ��ϵǾ� �ִ� ��� ���ø� ���� �켱���� ���� ����
	static void ClearAllTemplate(void);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	// ���ø� ���κ��� ����
	void __CopyFromTemplateNode(const MkDataNode* source);
	
	// ��ȿ�� ��� unit key ��ȯ
	// �ڽſ��Ը� �����ϰų� ����� ���ø� ���� ���� �ٸ� ��쿡 �ش�
	// ���ø� ������ ����� ��� GetUnitKeyList(keyList)�� ����
	void __GetValidUnitList(MkArray<MkHashStr>& keyList) const;

	// �ش� ��� ��ȿ��(���� ���� ����) ����
	// ���ø� ������ ����� ��� �׻� true�̸� ���ø� ���� ���� ������ �������� unit�̳� node�� �־�߸� true
	bool __IsValidNode(void) const;

	// �ڽ� ��� �� ��ȿ�� ������ �̸��� ������/ö�ں��� ������ ��ȯ
	// ���ø� ������ ��ϵ� ������� �տ� ��ġ�ϰ� �Ϲ� ������ ö�ں��� ���ĵǿ� �ڿ� ��ġ
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

	// �� ���� ��ġ�Ǵ� ���� ���� ���ø� ��� �ּ�
	MkDataNode* m_TemplateLink;
};

//------------------------------------------------------------------------------------------------//

#endif
