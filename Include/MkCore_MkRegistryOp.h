#pragma once

//------------------------------------------------------------------------------------------------//
// window registry operation
//
// (NOTE) write/delete의 경우 당연하게도 관리자 권한이 있어야 함
//        프로젝트 구성 속성 -> 링커 -> 메니페스트 도구 -> UAC 실행 수준 : 관리자 권한 요구(requireAdministrator)
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include "MkCore_MkHashStr.h"


class MkPathName;
class MkDataNode;

class MkRegistryOperator
{
public:

	// 해당 노드와 하위 모든 tree 정보를 node에 담음
	// value는 REG_SZ, REG_DWORD 자료형만 인식
	// value의 subkey와 값은 각각 배열을 이루어 KEY_Subkeys, KEY_Values 태그에 담김
	// 예를 들어 [기본값="URL:pwropw protocol"], ["URL Protocol"=""]가 들어 있으면 아래의 예처럼 기록
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

	// Read()를 통해 읽은 node에서 subKey에 매칭되는 value값을 반환
	static bool ReadValue(const MkDataNode& node, const MkStr& subKey, MkStr& value);

	// node에 담긴 정보를 그대로 레지스트리에 기록(node 구성 형식은 위와 동일)
	// (NOTE) 관리자 권한 요구
	static bool Write(const MkDataNode& node);

	// 해당 노드와 하위 모든 tree 삭제
	// 반환값은 성공여부가 아닌 해당 노드가 존재하지 않는지 여부(삭제 할 노드가 없어도 true)
	// ex> MkRegistryOperator::Delete(HKEY_CLASSES_ROOT, L"pwkopw");
	// (NOTE) 관리자 권한 요구
	static bool Delete(HKEY hKey, const MkStr& subKey);

	// 해당 노드의 value 삭제
	// 반환값은 성공여부가 아닌 해당 value가 존재하지 않는지 여부(삭제 할 value가 없어도 true)
	// (NOTE) 관리자 권한 요구
	static bool Delete(HKEY hKey, const MkStr& subKey, const MkStr& value);

	//------------------------------------------------------------------------------------------------//
	// predefined set
	//------------------------------------------------------------------------------------------------//

	// application 레지스트리 등록
	// ex>
	//	MkRegistryOperator::RegistApplicationInfo(L"Kuntara Online - 플레이위드", L"군타라 온라인",
	//		L"icon.ico", L"Playwith-GAMES Entertainment", L"http://www.playwith.co.kr/", L"Uninstall.exe");
	static bool RegistApplicationInfo
		(const MkStr& regName, // 레지스트리 항목 이름(필수)
		const MkStr& displayName, // 실제 표시 이름(EMPTY면 regName을 대신 사용)
		const MkPathName& iconApp, // 아이콘 혹은 따올 어플의 절대, 혹은 module directory기반 상대 경로
		const MkStr& publisher, // 퍼블리셔명(옵션)
		const MkStr& url, // 연결 URL
		const MkPathName& uninstallApp); // 언인스톨 어플의 절대, 혹은 module directory기반 상대 경로

	// application 레지스트리 삭제
	// ex> MkRegistryOperator::DeleteApplicationInfo(L"Kuntara Online - 플레이위드");
	static bool DeleteApplicationInfo(const MkStr& regName);

	// custom URL scheme 등록
	// ex> MkRegistryOperator::RegistCustomURLScheme(L"pwkopw", L"Starter.exe", true);
	//	-> 확인 : 브라우저를 열고 "pwkopw:'arg'" 입력
	static bool RegistCustomURLScheme
		(const MkStr& protocol, // 포로토콜명. 크롬의 경우 특수문자 입력받지 못함
		const MkPathName& launcherApp, // 호출될 어플의 절대, 혹은 module directory기반 상대 경로
		bool useArgument = true); // true일 경우 launcherApp 실행시 argument 전달 받음

	// custom URL scheme 삭제
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

		// KEY_READ 모드로 열기. Close() 호출까지 읽기 가능
		bool Open(HKEY hKey, const MkStr& subKey);

		// value 읽기. REG_SZ, REG_DWORD만 인식. subKey가 공문자열(MkStr::EMPTY)이면 기본 값 읽음
		bool Read(const MkStr& subKey, MkStr& value) const;

		// 하위 모든 value의 sub key를 반환. 공문자열이면 기본 값이 존재함을 의미
		bool GetAllValues(MkArray<MkStr>& buffer) const;

		// 하위 모든 자식 노드들의 sub key를 반환
		bool GetAllChildren(MkArray<MkStr>& buffer) const;

		// 닫기
		void Close(void);

		ReadOp() { m_hKey = NULL; }
		~ReadOp() { Close(); }

	protected:
		HKEY m_hKey;
	};

	//------------------------------------------------------------------------------------------------//
	// (NOTE) 프로젝트 구성 속성 -> 링커 -> 메니페스트 도구 -> UAC 실행 수준이 관리자 권한 요구로 되어 있어야 함
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

		// KEY_ALL_ACCESS 모드로 열기. Close() 호출까지 쓰기 가능. 존재하지 않으면 생성
		bool Open(HKEY hKey, const MkStr& subKey);
		bool Open(HKEY hKey, const MkArray<MkStr>& tokens);

		// REG_SZ로 value 쓰기. subKey가 공문자열(MkStr::EMPTY)이면 기본 값 의미
		bool Write(const MkStr& subKey, const MkStr& value) const;

		// value 삭제. subKey가 공문자열이면 기본 값 의미
		bool Delete(const MkStr& subKey) const;

		// 닫기
		void Close(void);

		// 주어진 subKey까지의 경로를 token으로 분리. ex> subKey == L"a/b/c" -> tokens == L"a", L"b", L"c"
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
