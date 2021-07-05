
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkRegistryOp.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkWindowStartRegister.h"

bool MkWindowStartRegister::RegisterStartProgram(const MkStr& key, const MkPathName& targetFilePath)
{
	// 이미 존재하면 삭제
	if (!RemoveStartProgram(key))
	{
		// 생성
		MkDataNode bufferNode;
		MkDataNode* destNode = bufferNode.CreateChildNode(L"HKEY_LOCAL_MACHINE")->
			CreateChildNode(L"Software")->
			CreateChildNode(L"Microsoft")->
			CreateChildNode(L"Windows")->
			CreateChildNode(L"CurrentVersion")->
			CreateChildNode(L"run");

		destNode->CreateUnit(MkRegistryOperator::KEY_Subkeys, key);

		MkPathName fullPath;
		fullPath.ConvertToModuleBasisAbsolutePath(targetFilePath);
		destNode->CreateUnit(MkRegistryOperator::KEY_Values, fullPath);

		return MkRegistryOperator::Write(bufferNode);
	}
	return false;
}

bool MkWindowStartRegister::RemoveStartProgram(const MkStr& key)
{
	return MkRegistryOperator::Delete(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\run", key);
}
