#ifndef __MINIKEY_CORE_MKCHEATMESSAGE_H__
#define __MINIKEY_CORE_MKCHEATMESSAGE_H__


//------------------------------------------------------------------------------------------------//
// 치트 처리 원형
//
// 문법 : command [arg0] [arg1] [arg2] ...
// - 각 구성단어들은 공문자와 DQM으로 구분 됨
// - command keyword는 공문자가 없는 단어(필수)
// - argument들은 DQM으로 둘러쌓인 문자열 혹은 공문자가 없는 단어(옵션)
//
// (NOTE) ExcuteMsg()의 스레드 안전성을 보장해야 함
//
// ex>
//	ChangeCameraMode TOP // command:ChangeCameraMode, arg0:TOP
//	JumpTo 100 4000 // command:JumpTo, arg0:100, arg1:4000
//	TalkTo "James halks" 2 "welcome."  // command:TalkTo, arg0:James halks, arg1:2, arg2:welcome.
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


class MkStr;

class MkCheatMessage
{
public:

	// 라인별 자동 파싱여부 반환
	// true면 command, arguments로 분리해 ExcuteMsg() 호출
	// false면 라인 명령어를 통채로 넣어 ExcuteLine() 호출
	// firstKeyword : 유효문자가 존재하는 라인의 최초 키워드. 조건 검사시 사용(ex> lineMsg[0] == L'/')
	virtual bool DoAutoParsing(const MkStr& firstKeyword) const { return true; }

	// 자동 파싱된 명령 처리
	// command : 공문자가 없는 명령 키워드
	// argument : 인수 배열
	// resultMsg에 실행 결과를 넣고 정상적 처리여부 반환
	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const;

	// 라인 단위 명령 처리
	// lineMsg : 라인 명령
	// resultMsg에 실행 결과를 넣고 정상적 처리여부 반환
	virtual bool ExcuteLine(const MkStr& lineMsg, MkStr& resultMsg) const { return false; }

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	bool __ExcuteMsg(const MkStr& inputMsg, MkStr& resultMsg) const;

public:

	virtual ~MkCheatMessage() {}
};

//------------------------------------------------------------------------------------------------//

#endif
