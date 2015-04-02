#pragma once

//------------------------------------------------------------------------------------------------//
// global instance - keyword filter
//
// - slang�� �ش� keyword�� ��ü��(MKDEF_SLANG_REPLACEMENT_TAG)�� ġȯ�� ��ȯ
//   keyword�� �������� ��� ��ҹ��� �������� ����
//   ex>
//		// ��Ӿ� ���
//		MK_KEYWORD_FILTER.RegisterSlang(L"FUCK");
//		MK_KEYWORD_FILTER.RegisterSlang(L"Fucking");
//		...
//
//		// ��Ӿ� �˻�
//		MkStr inputMsg = L"�� fuck ���� fucking �ƴ�";
//		MkStr dest;
//		if (MK_KEYWORD_FILTER.CheckSlang(inputMsg, dest)) // dest == L"�� **** ���� ******* �ƴ�"
//		{ ... }
//
// - tag�� �ش� keyword�� ������ tag�� ġȯ�� ��ȯ
//   keyword�� �������� ��� ��ҹ��� ������
//   ex>
//		// tag ���
//		MK_KEYWORD_FILTER.SetTag(L"_%MyName%_", L"�̸�� ��ǰ");
//		...
//
//		// tag ����
//		MkStr inputMsg = L"������ �������� _%MyName%_ �̵���";
//		MkStr dest;
//		MK_KEYWORD_FILTER.CheckTag(inputMsg, dest); // dest == L"������ �������� �̸�� ��ǰ �̵���"
//
// ǥ�� �ܾ ������� �� ������ �˰���. ���� ��Ȯ�� ���͸��� �ʿ��ϸ� 
// http://galab-work.cs.pusan.ac.kr/TRBoard/datafile/REP0903VULGAR.pdf ������ �߰� �� ��
//
// (NOTE) thread-safe���� ����
//

//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"

#define MK_KEYWORD_FILTER MkKeywordFilter::Instance()


class MkKeywordFilter : public MkSingletonPattern<MkKeywordFilter>
{
public:

	//------------------------------------------------------------------------------------------------//
	// ��Ӿ�
	//------------------------------------------------------------------------------------------------//

	// Ű���� ���
	void RegisterSlang(const MkStr& keyword);

	// src�� ��Ӿ� ���Կ��� ��ȯ
	bool CheckSlang(const MkStr& src) const;

	// src�� ��Ӿ� ���Կ��θ� ��ȯ�ϰ� MKDEF_SLANG_REPLACEMENT_TAG�� ġȯ�� ���ڿ��� dest�� �Ҵ�
	bool CheckSlang(const MkStr& src, MkStr& dest) const;

	//------------------------------------------------------------------------------------------------//
	// tag
	//------------------------------------------------------------------------------------------------//

	// keyword�� �ش��ϴ� tag ����
	void SetTag(const MkHashStr& keyword, const MkStr& tag);

	// src�� �˻��� keyword�� �ش��ϴ� tag�� ġȯ�� ���ڿ��� dest�� �Ҵ�
	void CheckTag(const MkStr& src, MkStr& dest) const;

	//------------------------------------------------------------------------------------------------//

	MkKeywordFilter() {}
	virtual ~MkKeywordFilter();

protected:

	bool _CheckString(const MkStr& src, MkStr& dest, bool slang) const;

protected:

	//------------------------------------------------------------------------------------------------//

	class CNode
	{
	public:

		void Clear(void);

		CNode() { m_EOK = false; }
		~CNode() { Clear(); }

		//----------------------------------------------------------------------------------//
		// ��Ͽ�
		//----------------------------------------------------------------------------------//

		// keyword���ڿ��� index��ġ�� �ִ� ���ڸ� end of keyword �÷��׿� �Բ� ���
		CNode* RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok);

		// keyword���ڿ��� Ʈ���� ����
		void RegisterKeyword(const MkStr& keyword);

		//----------------------------------------------------------------------------------//
		// �˻���
		//----------------------------------------------------------------------------------//

		// src�� index��ġ�� �ִ� ���ڸ� �˻��� �ڽ����� �������� ������ NULL��, �����ϸ� �ش� ��带 ��ȯ 
		const CNode* CheckCharacter(const MkStr& src, unsigned int index) const;

		// src�� ó������ keyword ���� ���θ� �˻��� ���ԵǾ��� ��� ũ�⸦ ��ȯ
		// keyword�� ������ 0 ��ȯ(���� �ϳ��� �˻�)
		unsigned int FindKeyword(const MkStr& src, unsigned int beginPos) const;

	public:

		bool m_EOK; // end of keyword

		typedef MkHashMap<wchar_t, CNode*> ChildTableType;
		ChildTableType m_Children; // �ڽ� ��� ���̺�
	};

	//------------------------------------------------------------------------------------------------//

protected:

	// slang
	CNode m_SlangRootNode;

	// tag
	CNode m_TagRootNode;
	MkHashMap<MkHashStr, MkStr> m_TagMap;
};
