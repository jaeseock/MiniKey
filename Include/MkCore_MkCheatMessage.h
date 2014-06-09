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

	// 입력 처리
	// command : 공문자가 없는 명령 키워드
	// argument : 인수 배열
	// resultMsg에 실행 결과를 넣고 정상적 처리여부 반환
	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const;

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	bool __ExcuteMsg(const MkStr& inputMsg, MkStr& resultMsg) const;

public:

	virtual ~MkCheatMessage() {}
};

//------------------------------------------------------------------------------------------------//

#endif
