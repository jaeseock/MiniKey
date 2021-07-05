#pragma once

//------------------------------------------------------------------------------------------------//
// 윈도우 프로세스 처리
//------------------------------------------------------------------------------------------------//

class MkStr;

class MkWindowProcessOp
{
public:
	
	// 프로세스 동작여부 반환
	static bool Exist(const MkStr& processName);

	// 동작중인 프로세스 갯수 반환
	// 실패면 -1, 없으면 0 반환
	static int Count(const MkStr& processName);

	// 동작중인 프로세스 삭제
	// 실패면 -1, 아니면 죽인 프로세스 수 반환
	static int Kill(const MkStr& processName);
};
