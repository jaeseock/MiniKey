#pragma once

//------------------------------------------------------------------------------------------------//
// window registry operation
//
// (NOTE) write/delete�� ��� �翬�ϰԵ� ������ ������ �־�� ��
//        ������Ʈ ���� �Ӽ� -> ��Ŀ -> �޴��佺Ʈ ���� -> UAC ���� ���� : ������ ���� �䱸(requireAdministrator)
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include "MkCore_MkHashStr.h"


class MkPathName;
class MkDataNode;

class MkRegistryOperator
{
public:

	// �ش� ���� ���� ��� tree ������ node�� ����
	// value�� REG_SZ, REG_DWORD �ڷ����� �ν�
	// value�� subkey�� ���� ���� �迭�� �̷�� KEY_Subkeys, KEY_Values �±׿� ���
	// ���� ��� [�⺻��="URL:pwropw protocol"], ["URL Protocol"=""]�� ��� ������ �Ʒ��� ��ó�� ���
	//
	// ex>
	//	MkDataNode node;
	//	MkRegistryOperator::Read(HKEY_CLASSES_ROOT, L"pwropw", node);
	//	node.SaveToText(L"reg_out.txt");
	//	->
	//	[reg_out.txt]
	//
	//	Node "HKEY_CLASSES_ROOT"
	//	{
	//		Node "pwropw"
	//		{
	//			str #Subkeys = // [2]
	//				"URL Protocol" /
	//				"";
	//			str #Values = // [2]
	//				"" /
	//				"URL:pwropw protocol";
	//
	//			Node "Shell"
	//			{
	//				Node "Open"
	//				{
	//					Node "Command"
	//					{
	//						str #Subkeys = "";
	//						str #Values = "\"C:\RohanOrigin\Loader.exe\" \"%1\"";
	//					}
	//				}
	//			}
	//		}
	//	}
	static bool Read(HKEY rootKey, const MkStr& subKey, MkDataNode& node);

	// Read()�� ���� ���� node���� subKey�� ��Ī�Ǵ� value���� ��ȯ
	static bool ReadValue(const MkDataNode& node, const MkStr& subKey, MkStr& value);

	// node�� ��� ������ �״�� ������Ʈ���� ���(node ���� ������ ���� ����)
	// (NOTE) ������ ���� �䱸
	static bool Write(const MkDataNode& node);

	// �ش� ���� ���� ��� tree ����
	// ��ȯ���� �������ΰ� �ƴ� �ش� ��尡 �������� �ʴ��� ����(���� �� ��尡 ��� true)
	// ex> MkRegistryOperator::Delete(HKEY_CLASSES_ROOT, L"pwkopw");
	// (NOTE) ������ ���� �䱸
	static bool Delete(HKEY hKey, const MkStr& subKey);

	// �ش� ����� value ����
	// ��ȯ���� �������ΰ� �ƴ� �ش� value�� �������� �ʴ��� ����(���� �� value�� ��� true)
	// (NOTE) ������ ���� �䱸
	static bool Delete(HKEY hKey, const MkStr& subKey, const MkStr& value);

	//------------------------------------------------------------------------------------------------//
	// predefined set
	//------------------------------------------------------------------------------------------------//

	// application ������Ʈ�� ���
	// ex>
	//	MkRegistryOperator::RegistApplicationInfo(L"Kuntara Online - �÷�������", L"��Ÿ�� �¶���",
	//		L"icon.ico", L"Playwith-GAMES Entertainment", L"http://www.playwith.co.kr/", L"Uninstall.exe");
	static bool RegistApplicationInfo
		(const MkStr& regName, // ������Ʈ�� �׸� �̸�(�ʼ�)
		const MkStr& displayName, // ���� ǥ�� �̸�(EMPTY�� regName�� ��� ���)
		const MkPathName& iconApp, // ������ Ȥ�� ���� ������ ����, Ȥ�� module directory��� ��� ���
		const MkStr& publisher, // �ۺ��Ÿ�(�ɼ�)
		const MkStr& url, // ���� URL
		const MkPathName& uninstallApp); // ���ν��� ������ ����, Ȥ�� module directory��� ��� ���

	// application ������Ʈ�� ����
	// ex> MkRegistryOperator::DeleteApplicationInfo(L"Kuntara Online - �÷�������");
	static bool DeleteApplicationInfo(const MkStr& regName);

	// custom URL scheme ���
	// ex> MkRegistryOperator::RegistCustomURLScheme(L"pwkopw", L"Starter.exe", true);
	//	-> Ȯ�� : �������� ���� "pwkopw:'arg'" �Է�
	static bool RegistCustomURLScheme
		(const MkStr& protocol, // �������ݸ�. ũ���� ��� Ư������ �Է¹��� ����
		const MkPathName& launcherApp, // ȣ��� ������ ����, Ȥ�� module directory��� ��� ���
		bool useArgument = true); // true�� ��� launcherApp ����� argument ���� ����

	// custom URL scheme ����
	// ex> MkRegistryOperator::DeleteCustomURLScheme(L"pwkopw");
	static bool DeleteCustomURLScheme(const MkStr& protocol);

public:

	static const MkHashStr KEY_Subkeys;
	static const MkHashStr KEY_Values;

public:

	//------------------------------------------------------------------------------------------------//
	// ex>
	//	MkRegistryOperator::ReadOp rOp;
	//	if (rOp.Open(HKEY_CLASSES_ROOT, L"pwkopw\\test1\\test2"))
	//	{
	//		MkArray<MkStr> valBuf;
	//		rOp.GetAllValues(valBuf);
	//
	//		MK_INDEXING_LOOP(valBuf, i)
	//		{
	//			MkStr buffer;
	//			rOp.Read(valBuf[i], buffer);
	//		}
	//		rOp.Close();
	//	}

	class ReadOp
	{
	public:

		// KEY_READ ���� ����. Close() ȣ����� �б� ����
		bool Open(HKEY hKey, const MkStr& subKey);

		// value �б�. REG_SZ, REG_DWORD�� �ν�. subKey�� �����ڿ�(MkStr::EMPTY)�̸� �⺻ �� ����
		bool Read(const MkStr& subKey, MkStr& value) const;

		// ���� ��� value�� sub key�� ��ȯ. �����ڿ��̸� �⺻ ���� �������� �ǹ�
		bool GetAllValues(MkArray<MkStr>& buffer) const;

		// ���� ��� �ڽ� ������ sub key�� ��ȯ
		bool GetAllChildren(MkArray<MkStr>& buffer) const;

		// �ݱ�
		void Close(void);

		ReadOp() { m_hKey = NULL; }
		~ReadOp() { Close(); }

	protected:
		HKEY m_hKey;
	};

	//------------------------------------------------------------------------------------------------//
	// (NOTE) ������Ʈ ���� �Ӽ� -> ��Ŀ -> �޴��佺Ʈ ���� -> UAC ���� ������ ������ ���� �䱸�� �Ǿ� �־�� ��
	// ex>
	//	MkRegistryOperator::Write wOp;
	//	if (wOp.Open(HKEY_CLASSES_ROOT, L"pwkopw\\test1\\test2"))
	//	{
	//		wOp.Write(MkStr::EMPTY, L"def val");
	//		wOp.Write(L"testkey", L"test val");
	//		wOp.Delete(MkStr::EMPTY);
	//		wOp.Delete(L"testkey");
	//		wOp.Close();
	//	}
	
	class WriteOp
	{
	public:

		// KEY_ALL_ACCESS ���� ����. Close() ȣ����� ���� ����. �������� ������ ����
		bool Open(HKEY hKey, const MkStr& subKey);
		bool Open(HKEY hKey, const MkArray<MkStr>& tokens);

		// REG_SZ�� value ����. subKey�� �����ڿ�(MkStr::EMPTY)�̸� �⺻ �� �ǹ�
		bool Write(const MkStr& subKey, const MkStr& value) const;

		// value ����. subKey�� �����ڿ��̸� �⺻ �� �ǹ�
		bool Delete(const MkStr& subKey) const;

		// �ݱ�
		void Close(void);

		// �־��� subKey������ ��θ� token���� �и�. ex> subKey == L"a/b/c" -> tokens == L"a", L"b", L"c"
		static void ConvertSubkeyToTokens(const MkStr& subKey, MkArray<MkStr>& tokens);

		WriteOp() { m_hKey = NULL; }
		~WriteOp() { Close(); }

	protected:
		HKEY m_hKey;
	};

	//------------------------------------------------------------------------------------------------//

protected:

	static void _Read(HKEY rootKey, const MkStr& currSubKey, const ReadOp& currOp, MkDataNode& node);
	static void _Write(HKEY rootKey, const MkArray<MkStr>& tokens, const WriteOp& currOp, const MkDataNode& node);
};
