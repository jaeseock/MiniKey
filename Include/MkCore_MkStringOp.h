#pragma once

//---------------------------------------------------------------------------------//
// std::string�� ���̽��� �� ��Ǽ�
// MkStr�� ���ڿ� ó������ �� ���������� ������� �� �� ��쵵 �ֱ⿡ �����Ѵٴ� �ǹ�
// - Ű���� ���� �� ��ġ �˻�
// - Ű���� �˻� �� ����
// - ��ϴ��� ó��
// - ��ūȭ
// - ��ȯ
// - ���� �� ��� ó��
// - std::wstring���� ��ȯ
//
// (NOTE!!!) ��Ƽ����Ʈ����̹Ƿ� �ٱ��� ó���� ���� ���!!!
//---------------------------------------------------------------------------------//

#include <string>
#include <vector>
#include <atlcoll.h>
#include <windef.h>


class MkStringOp
{
public:

	//---------------------------------------------------------------------------------//
	// Ű���� ���� �� ��ġ �˻�
	//---------------------------------------------------------------------------------//

	// Ư�� ���ڿ��� ���Ե� ���� ��ȯ
	// (in) source : �˻� ��� ���ڿ�
	// (in) keyword : �˻��� ���ڿ�
	// return : ���� ����
	// ex> MkStringOp::CountKeyword("abc##def##gh", "##") -> 2
	static int CountKeyword(const std::string& source, const std::string& keyword);

	// prefix ���� �˻�
	// (in) source : �˻� ��� ���ڿ�
	// (in) prefix : �˻��� ���ڿ�
	// return : ���� ����
	// ex> MkStringOp::CheckPrefix("abc_ka", "abc_") -> true
	static bool CheckPrefix(const std::string& source, const std::string& prefix);

	// postfix ���� �˻�
	// (in) source : �˻� ��� ���ڿ�
	// (in) postfix : �˻��� ���ڿ�
	// return : ���� ����
	// ex> MkStringOp::CheckPostfix("ka_abc", "_abc") -> true
	static bool CheckPostfix(const std::string& source, const std::string& postfix);

	// ���� �˻��� Ư�� ���ڿ� ��ġ �˻� (�տ������� �ڷ� Ž��)
	// (in) source : �˻� ��� ���ڿ�
	// (in) keyword : �˻��� ���ڿ�
	// return : ���Ե��� �ʾ����� -1, ���ԵǾ����� ��ġ(0 �̻�) ����
	// ex> MkStringOp::GetFirstKeywordPosition("012##5##89", "##") -> 3
	static int GetFirstKeywordPosition(const std::string& source, const std::string& keyword);

	// ���������� �˻��� Ư�� ���ڿ� ��ġ �˻� (�ڿ������� ������ Ž��)
	// (in) source : �˻� ��� ���ڿ�
	// (in) keyword : �˻��� ���ڿ�
	// return : ���Ե��� �ʾ����� -1, ���ԵǾ����� ��ġ(0 �̻�) ����
	// ex> MkStringOp::GetLastKeywordPosition("012##5##89", "##") -> 6
	static int GetLastKeywordPosition(const std::string& source, const std::string& keyword);

	// startPosition���� �����ϴ� ���� ��ȿ����(�����ڰ� �ƴ�) ��ġ�� ��ȯ
	// (in) source : �˻� ��� ���ڿ�
	// (in) startPosition : source�� �˻� ���� ��ġ
	// return : ���Ե��� �ʾ����� -1, ���ԵǾ����� ��ġ(0 �̻�) ����
	static int GetFirstValidPosition(const std::string& source, int startPosition = 0);

	// startPosition���� �����ϴ� ������ ��ȿ����(�����ڰ� �ƴ�) ��ġ�� ��ȯ
	// (in) source : �˻� ��� ���ڿ�
	// (in) startPosition : source�� �˻� ���� ��ġ
	// return : ���Ե��� �ʾ����� -1, ���ԵǾ����� ��ġ(0 �̻�) ����
	static int GetLastValidPosition(const std::string& source, int startPosition = 0);

	//---------------------------------------------------------------------------------//
	// Ű���� �˻� �� ����
	//---------------------------------------------------------------------------------//

	// keyword_a�� keyword_b�� ġȯ
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) keyword_a : ġȯ ��� ���ڿ�
	// (in) keyword_b : ġȯ�� ���ڿ�
	// (in) startPosition : source�� �˻� ���� ��ġ
	// return : ġȯ�� ����
	// ex>
	//   tmpBuffer = "12abc3abc456";
	//   MkStringOp::ReplaceKeyword(tmpBuffer, "abc", "@", 0) -> tmpBuffer == "12@3@456"
	static int ReplaceKeyword(std::string& source, const std::string& keyword_a, const std::string& keyword_b, int startPosition = 0);

	// Ű���� ����
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) keyword : ���� ��� ���ڿ�
	// (in) startPosition : source�� �˻� ���� ��ġ
	// ex>
	//   tmpBuffer = "12abc3abc456";
	//   MkStringOp::RemoveKeyword(tmpBuffer, "abc", 0) -> tmpBuffer == "123456"
	static void RemoveKeyword(std::string& source, const std::string& keyword, int startPosition = 0);

	// ��� ��ũ(tab, space, line feed, return)�� ����
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) startPosition : source�� �˻� ���� ��ġ
	static void RemoveBlank(std::string& source, int startPosition = 0);

	// ù ��ȿ���ڱ����� ��ũ(tab, space, line feed, return)�� ����
	// (in/out) source : �˻� ��� ���ڿ�
	static void RemoveFrontSideBlank(std::string& source);

	// ������ ��ȿ���� ������ ��ũ(tab, space, line feed, return)�� ����
	// (in/out) source : �˻� ��� ���ڿ�
	static void RemoveRearSideBlank(std::string& source);

	//---------------------------------------------------------------------------------//
	// ��ϴ��� ó��
	//---------------------------------------------------------------------------------//

	// startPosition���� endKeyword ������ ���� ����� �˻��� endKeyword�� ���ܵ� ��� ���ڿ��� buffer�� �Ҵ�
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// (in) startPosition : �˻� ������ġ
	// (in) cut : buffer�� ��� ���ڿ��� �Ҵ� �� ��, source���� Ű������� ���Ե� �ش� ����� ���������� ����
	// return : �Ҵ� ��������
	// ex>
	//   std::string tmpBuffer = "abc#123";
	//   std::string buffer;
	//   MkStringOp::GetFirstBlock(tmpBuffer, "#", buffer, 1, true) -> tmpBuffer == "a123", buffer == "bc"
	static bool GetFirstBlock(std::string& source, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition���� beginKeyword �� endKeyword ������ ���� ����� �˻��� Ű���尡 ���ܵ� ��� ���ڿ��� buffer�� �Ҵ�
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// (in) startPosition : �˻� ������ġ
	// (in) cut : buffer�� ��� ���ڿ��� �Ҵ� �� ��, source���� Ű������� ���Ե� �ش� ����� ���������� ����
	// return :
	//	- �ش� ���ڿ��� �����ϰ� cut�� false�� beginKeyword ���� ��ġ�� ��ȯ
	//	- �ش� ���ڿ��� �����ϰ� cut�� true�� ���� beginKeyword ������ġ�� ��ȯ
	//	- startPosition�� source�� ũ�⸦ �Ѿ�ų� ����� �������� ������ -1 ����
	// ex>
	//   std::string tmpBuffer = "abc{123}de";
	//   std::string buffer;
	//   MkStringOp::GetFirstBlock(tmpBuffer, "{", "}", buffer, 0, true) -> tmpBuffer == "abcde", buffer == "123"
	static int GetFirstBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition���� ���� depth�� beginKeyword �� endKeyword ������ ����� �˻��� Ű���尡 ���ܵ� ��� ���ڿ��� buffer�� �Ҵ�
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// (in) startPosition : �˻� ������ġ
	// (in) cut : buffer�� ��� ���ڿ��� �Ҵ� �� ��, source���� Ű������� ���Ե� �ش� ����� ���������� ����
	// return :
	//	- �ش� ���ڿ��� �����ϰ� cut�� false�� beginKeyword ���� ��ġ�� ��ȯ
	//	- �ش� ���ڿ��� �����ϰ� cut�� true�� ���� beginKeyword ������ġ�� ��ȯ
	//	- startPosition�� source�� ũ�⸦ �Ѿ�ų� ����� �������� ������ -1 ����
	// ex>
	//   std::string tmpBuffer = "abc{12{##}3}de";
	//   std::string buffer;
	//   MkStringOp::GetFirstStackBlock(tmpBuffer, "{", "}", buffer, 0, true) -> tmpBuffer == "abcde", buffer == "12{##}3"
	//   (NOTE!!!) ���� ���÷� GetFirstBlock()�� �����ϸ�, tmpBuffer == "abc3}de", buffer == "12{##" �� ����
	static int GetFirstStackBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition���� beginKeyword �� endKeyword ������ ����� keyword �����Ͽ� replaceBlock���� ��ü
	// replaceKeyword�� �����ڿ��� ��� ������ ����
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (in) replaceBlock : ġȯ�� ���ڿ�
	// (in) startPosition : �˻� ������ġ
	// return : ġȯ ���� ����
	// ex>
	//   std::string tmpBuffer = "abc{123}de";
	//   ġȯ : ReplaceBlock(tmpBuffer, "{", "}", "##", 0) -> tmpBuffer == "abc##de"
	//   ���� : ReplaceBlock(tmpBuffer, "{", "}", "", 0) -> tmpBuffer == "abcde"
	static bool ReplaceBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, const std::string& replaceBlock, int startPosition = 0);

	// �ּ� ����. ���� �ڸ�Ʈ("//" ~ line feed)�� ûũ �ڸ�Ʈ("/*" ~ "*/")�� ���� ��
	// (in/out) source : �˻� ��� ���ڿ�
	static void RemoveAllComment(std::string& source);

	//---------------------------------------------------------------------------------//
	// ��ūȭ
	//---------------------------------------------------------------------------------//

	// ������ separator�� �������� ���ڿ��� ��ū��� �ڸ�
	// separator�� �������� ���� ��� �ڽ� ����
	// (in) source : �˻� ��� ���ڿ�
	// (out) tokens : �и��� ��ū���� ��� vector container
	// (in) separator : ������
	// return : ��ū ���� ��ȯ
	// ex>
	//   std::string tmpBuffer = "12;34;;;5;678;;9";
	//   MkStringOp::Tokenize(tmpBuffer, tokens, ";") -> tokens == "12", "34", "5", "678", "9"
	static int Tokenize(const std::string& source, std::vector<std::string>& tokens, const std::string& separator);

	// ������(tab, space, line feed, return)�� separator�� �����Ͽ� ��ū��� �ڸ�
	// �����ڰ� �������� ���� ��� �ڽ� ����
	// (in) source : �˻� ��� ���ڿ�
	// (out) tokens : �и��� ��ū���� ��� vector container
	// return : ��ū ���� ��ȯ
	static int Tokenize(const std::string& source, std::vector<std::string>& tokens);

	//---------------------------------------------------------------------------------//
	// ��ȯ
	//---------------------------------------------------------------------------------//

	// �־��� ������ŭ �����κ��� ������
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) count : ������ ���� ��
	static void PopFront(std::string& source, int count);

	// �־��� ������ŭ �ڷκ��� ������
	// (in/out) source : �˻� ��� ���ڿ�
	// (in) count : ������ ���� ��
	static void BackSpace(std::string& source, int count);

	// �빮�ڷ� ��ȯ
	// (in/out) source : �˻� ��� ���ڿ�
	static void ToUpper(std::string& source);

	// �ҹ��ڷ� ��ȯ
	// (in/out) source : �˻� ��� ���ڿ�
	static void ToLower(std::string& source);

	// bool������ ��ȯ
	// ��ҹ��� ��� ���� "true"�ų� "yes"�� ��� ��, �ƴϸ� ����
	// (in) source : �˻� ��� ���ڿ�
	// return : ��ȯ�� bool ��
	static bool ToBool(std::string& source);

	// int������ ��ȯ
	// (in) source : �˻� ��� ���ڿ�
	// return : ��ȯ�� int ��
	static int ToInteger(std::string& source);

	// float���� ��ȯ
	// %�� ����� ��� percentage ���¶� �����ϰ� 100:1�� ������ ��ȯ(100% -> 1.f, 50% -> 0.5f)
	// (in) source : �˻� ��� ���ڿ�
	// return : ��ȯ�� float ��
	static float ToFloat(std::string& source);

	// std::string���� ��ȯ
	// (in) source : �˻� ��� ��
	// return : ��ȯ�� ���ڿ�
	static std::string ToString(bool source);
	static std::string ToString(int source);
	static std::string ToString(float source);
	
	//---------------------------------------------------------------------------------//
	// ���� �� ��� ó��
	//---------------------------------------------------------------------------------//

	// ���� ���翩�� �˻�
	// (in) filePath : �˻��� ����
	static bool CheckFileEnable(const char* filePath);

	// ���� ����� ��� Ȯ���ڿ� �ش��ϴ� ���ڿ��� ���� (������ dot ������ ���ڿ�)
	// (in) source : �˻� ��� ���ڿ�
	// return : Ȯ���� ���ڿ�
	// ex> MkStringOp::GetFileExtension("D:\\test\\test.txt") -> "txt"
	// (NOTE!!!) source�� ���¿� ���� Ȯ���ڰ� ���� ��쵵 �����Ƿ� ���� �� �ݵ�� empty() üũ �� ��
	static std::string GetFileExtension(const std::string& source);

	// ���丮 Ž�� ���̾�α�(SHGetPathFromIDList)
	// (in) msg : ���̾�α׿� ��µ� �޼��� ���ڿ�
	// (in) initialPath : �⺻���� ���
	// ex> std::string path = MkStringOp::GetDirectoryPathFromDialog(NULL, "�����", "D:\\Solutions\\MiniKey\\Include");
	// return : ������ ���. ��ҵ� ��� �����ڿ�
	static std::string GetDirectoryPathFromDialog(HWND owner, const std::string& msg, const std::string& initialPath = "");

	// ���� ����� ��� ���, �̸�, Ȯ������ �� �κ����� �и��� ����
	// (in) source : �˻� ��� ���ڿ�
	// (out) path : ��ΰ� ��� ���� (\ ����)
	// (out) name : �̸��� ��� ����
	// (out) extension : Ȯ���ڰ� ��� ����
	// ex>
	//   std::string path, name, extension;
	//   MkStringOp::GetPathInformation("D:\\test\\test.txt", path, name, extension) -> "D:\test\test\", "test", "txt"
	// (NOTE!!!) source�� ���¿� ���� ���� ���ɼ��� �����ϹǷ� ���� �� �ݵ�� ��ȿ�� üũ �� ��
	static void GetPathInformation(const std::string& source, std::string& path, std::string& name, std::string& extension);

	// ���Ϸκ��� �ؽ�Ʈ�� �о�鿩 ����
	// (out) target : ������ �Ҵ�� ���ڿ�
	// (in) filePath : �о���� ����
	// (in) ignoreComment : �ּ� �ڵ� ���� ����
	// (in) position : ���� ������ ������ ��ġ. �����̰ų� ������ �Ѿ ��� �������� �߰���
	// return : op. ���� ����
	static bool AttachFromTextFile(std::string& target, const char* filePath, bool ignoreComment = true, int position = -1);

	// �ؽ�Ʈ�� ���Ϸ� ���
	// (in) source : ��� ������ ��� ���ڿ�
	// (in) filePath : ��µ� ���� ���
	// (in) overwrite : �̹� ���� �̸��� ������ ������ ��� ������� ����. false �� ��� �ڿ� ������
	// return : op. ���� ����
	static bool WriteToTextFile(const std::string& source, const char* filePath, bool overwrite = true);

	//---------------------------------------------------------------------------------//
	// wstring���� ��ȯ
	//---------------------------------------------------------------------------------//

	// multibyte -> wide string
	static void ConvertString(const std::string& source, std::wstring& buffer, unsigned int codePage = CP_ACP);

	// wide string -> multibyte
	static void ConvertString(const std::wstring& source, std::string& buffer, unsigned int codePage = CP_ACP);

	//---------------------------------------------------------------------------------//

protected:

	// GetDirectoryPathFromDialog()�� �ݹ�. SHBrowseForFolder���� �ʱ��� ������ ���ؼ��� �ݹ�ܿ� ���� ���ٰ� �Ѵ�. M$�� ���ô�.
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData);
};

