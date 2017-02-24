#ifndef __MINIKEY_CORE_MKCHEATMESSAGE_H__
#define __MINIKEY_CORE_MKCHEATMESSAGE_H__


//------------------------------------------------------------------------------------------------//
// ġƮ ó�� ����
//
// ���� : command [arg0] [arg1] [arg2] ...
// - �� �����ܾ���� �����ڿ� DQM���� ���� ��
// - command keyword�� �����ڰ� ���� �ܾ�(�ʼ�)
// - argument���� DQM���� �ѷ����� ���ڿ� Ȥ�� �����ڰ� ���� �ܾ�(�ɼ�)
//
// (NOTE) ExcuteMsg()�� ������ �������� �����ؾ� ��
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

	// ���κ� �ڵ� �Ľ̿��� ��ȯ
	// true�� command, arguments�� �и��� ExcuteMsg() ȣ��
	// false�� ���� ��ɾ ��ä�� �־� ExcuteLine() ȣ��
	// firstKeyword : ��ȿ���ڰ� �����ϴ� ������ ���� Ű����. ���� �˻�� ���(ex> lineMsg[0] == L'/')
	virtual bool DoAutoParsing(const MkStr& firstKeyword) const { return true; }

	// �ڵ� �Ľ̵� ��� ó��
	// command : �����ڰ� ���� ��� Ű����
	// argument : �μ� �迭
	// resultMsg�� ���� ����� �ְ� ������ ó������ ��ȯ
	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const;

	// ���� ���� ��� ó��
	// lineMsg : ���� ���
	// resultMsg�� ���� ����� �ְ� ������ ó������ ��ȯ
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
