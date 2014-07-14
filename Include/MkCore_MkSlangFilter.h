#ifndef __MINIKEY_CORE_MKSLANGFILTER_H__
#define __MINIKEY_CORE_MKSLANGFILTER_H__

//------------------------------------------------------------------------------------------------//
// global instance - slang filter
//
// ��Ӿ� ����
//
// thread-safe������ ������ �ʱ�ȭ�� �ϰ� ���(produce), ��Ÿ���߿��� ���(consume)�� �ϴ�
// �����̱� ������ ������ ����
//
// �������� ��� �ҹ��ڷ� �ڵ� ��ȯ�Ǿ� �˻�(�빮�� ��Ͻÿ� �ҹ��ڷ� ��ȯ�Ǿ� ����)
//
// ǥ�� �ܾ ������� �� ������ �˰���. ���� ��Ȯ�� ��Ӿ� ���͸��� �ʿ��ϸ� 
// http://galab-work.cs.pusan.ac.kr/TRBoard/datafile/REP0903VULGAR.pdf ������ �߰� �� ��
//
// ex>
//	// Ű���� ���
//	MK_SLANG_FILTER.RegisterKeyword(L"fuck");
//	MK_SLANG_FILTER.RegisterKeyword(L"fucking");
//	...
//
//	// ���ڿ� �˻�
//	MkStr inputMsg = L"�� fuck ���� fucking �ƴ�";
//	MkStr dest;
//	if (MK_SLANG_FILTER.CheckString(inputMsg, dest)) { ... }
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"

#define MK_SLANG_FILTER MkSlangFilter::Instance()


class MkSlangFilter : public MkSingletonPattern<MkSlangFilter>
{
public:

	// ��Ӿ� Ű���� ���
	void RegisterKeyword(const MkStr& keyword);

	// src�� ��Ӿ� ���Կ��� ��ȯ
	bool CheckString(const MkStr& src) const;

	// src�� ��Ӿ� ���Կ��θ� ��ȯ�ϰ� MKDEF_SLANG_REPLACEMENT_TAG�� ġȯ�� ���ڿ��� dest�� �Ҵ�
	bool CheckString(const MkStr& src, MkStr& dest) const;

protected:

	//------------------------------------------------------------------------------------------------//

	class CNode
	{
	public:

		void Clear(void);

		CNode() { m_EOK = false; }
		~CNode() { Clear(); }

		//----------------------------------------------------------------------------------//
		// ��Ӿ� ��Ͽ� �޼ҵ�
		//----------------------------------------------------------------------------------//

		// keyword���ڿ��� index��ġ�� �ִ� ���ڸ� end of keyword �÷��׿� �Բ� ���
		CNode* RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok);

		// keyword���ڿ��� Ʈ���� ����
		void RegisterKeyword(const MkStr& keyword);

		//----------------------------------------------------------------------------------//
		// ��Ӿ� �˻��� �޼ҵ�
		//----------------------------------------------------------------------------------//

		// keyword���ڿ��� index��ġ�� �ִ� ���ڸ� �˻��� �ڽ����� �������� ������ NULL��,
		// �����ϸ� �ش� ��带 ��ȯ 
		const CNode* CheckCharacter(const MkStr& keyword, unsigned int index) const;

		// keyword���ڿ��� ó������ ��Ӿ� ���� ���θ� �˻��� ���ԵǾ��� ��� ũ�⸦ ��ȯ
		// ��Ӿ ������ 0 ��ȯ(���� �ϳ��� �˻�)
		unsigned int FindSlang(const MkStr& keyword, unsigned int beginPos) const;

		// src ���ڿ��� �˻��� ��Ӿ ����� ��� '*'�� ġȯ�� dest�� ��ȯ
		// ��ȯ���� ��Ӿ� ���� ����
		bool CheckString(const MkStr& src, MkStr& dest) const;

	public:

		bool m_EOK; // end of keyword

		typedef MkHashMap<wchar_t, CNode*> ChildTableType;
		ChildTableType m_Children; // �ڽ� ��� ���̺�
	};

	//------------------------------------------------------------------------------------------------//

	void _ConvertTextToLower(MkStr& msg, MkArray<bool>& converting) const;
	void _ConvertTextToOriginal(MkStr& msg, const MkArray<bool>& converting) const;

public:

	MkSlangFilter() {}
	virtual ~MkSlangFilter() { m_RootNode.Clear(); }

protected:

	CNode m_RootNode;
};

//------------------------------------------------------------------------------------------------//

#endif
