
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkRegistryOp.h"


//------------------------------------------------------------------------------------------------//

const MkHashStr MkRegistryOperator::KEY_Subkeys(L"#Subkeys");
const MkHashStr MkRegistryOperator::KEY_Values(L"#Values");


bool MkRegistryOperator::Read(HKEY rootKey, const MkStr& subKey, MkDataNode& node)
{
	// root node 생성
	MkHashStr rootName;
	if (rootKey == HKEY_CLASSES_ROOT) { rootName = L"HKEY_CLASSES_ROOT"; }
	else if (rootKey == HKEY_CURRENT_CONFIG) { rootName = L"HKEY_CURRENT_CONFIG"; }
	else if (rootKey == HKEY_CURRENT_USER) { rootName = L"HKEY_CURRENT_USER"; }
	else if (rootKey == HKEY_LOCAL_MACHINE) { rootName = L"HKEY_LOCAL_MACHINE"; }
	else if (rootKey == HKEY_USERS) { rootName = L"HKEY_USERS"; }

	MK_CHECK(!rootName.Empty(), L"rootKey는 predefined 값들 중 하나이어야 함")
		return false;

	ReadOp op;
	if (op.Open(rootKey, subKey)) // 해당 subkey가 열린다는 것은 도달 경로도 모두 존재함을 의미
	{
		// root node 생성
		MkDataNode* destNode = node.ChildExist(rootName) ? node.GetChildNode(rootName) : node.CreateChildNode(rootName);
		MK_CHECK(destNode != NULL, rootName.GetString() + L" root node 생성 실패")
			return false;

		// root부터 현재까지의 경로를 생성
		MkStr currPath;
		MkArray<MkStr> tokens;
		WriteOp::ConvertSubkeyToTokens(subKey, tokens);
		MK_INDEXING_LOOP(tokens, i)
		{
			MkHashStr currKey = tokens[i];
			destNode = destNode->ChildExist(currKey) ? destNode->GetChildNode(currKey) : destNode->CreateChildNode(currKey);
			MK_CHECK(destNode != NULL, currPath + L" node 생성 실패")
				return false;

			if (i > 0)
			{
				currPath += L"\\";
			}
			currPath += tokens[i];
		}

		_Read(rootKey, currPath, op, *destNode);
		return true;
	}
	return false;
}

bool MkRegistryOperator::Write(const MkDataNode& node)
{
	MkArray<MkHashStr> rootKeys;
	node.GetChildNodeList(rootKeys);
	MK_INDEXING_LOOP(rootKeys, i)
	{
		// root key & node
		HKEY rootKey = NULL;

		const MkHashStr& currRootKey = rootKeys[i];
		if (currRootKey.GetString() == MkStr(L"HKEY_CLASSES_ROOT")) { rootKey = HKEY_CLASSES_ROOT; }
		else if (currRootKey.GetString() == MkStr(L"HKEY_CURRENT_CONFIG")) { rootKey = HKEY_CURRENT_CONFIG; }
		else if (currRootKey.GetString() == MkStr(L"HKEY_CURRENT_USER")) { rootKey = HKEY_CURRENT_USER; }
		else if (currRootKey.GetString() == MkStr(L"HKEY_LOCAL_MACHINE")) { rootKey = HKEY_LOCAL_MACHINE; }
		else if (currRootKey.GetString() == MkStr(L"HKEY_USERS")) { rootKey = HKEY_USERS; }

		MK_CHECK(rootKey != NULL, currRootKey.GetString() + L"는 predefined key가 아님")
			continue;

		MkArray<MkStr> subKeys;
		WriteOp op;
		if (op.Open(rootKey, subKeys))
		{
			_Write(rootKey, subKeys, op, *node.GetChildNode(currRootKey));
		}
	}
	return false;
}

bool MkRegistryOperator::Delete(HKEY hKey, const MkStr& subKey)
{
	const wchar_t* skName = subKey.Empty() ? NULL : subKey.GetPtr();
	HKEY tmpKey = NULL;
	if (::RegOpenKeyEx(hKey, skName, 0, KEY_ALL_ACCESS, &tmpKey) == ERROR_SUCCESS)
	{
		::RegCloseKey(tmpKey);
		return (::RegDeleteTree(hKey, skName) == ERROR_SUCCESS);
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkRegistryOperator::RegistApplicationInfo
(const MkStr& regName, const MkStr& displayName, const MkPathName& iconApp, const MkStr& publisher, const MkStr& url, const MkPathName& uninstallApp)
{
	MkDataNode node;
	MkDataNode* destNode = node.CreateChildNode(L"HKEY_LOCAL_MACHINE");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"SOFTWARE");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"Microsoft");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"Windows");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"CurrentVersion");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"Uninstall");
	if (destNode == NULL)
		return false;

	if (regName.Empty())
		return false;

	destNode = destNode->CreateChildNode(regName);
	if (destNode == NULL)
		return false;

	MkArray<MkStr> subKeys(5), values(5);
	
	subKeys.PushBack(L"DisplayName");
	values.PushBack(displayName.Empty() ? regName : displayName);

	if (!iconApp.Empty())
	{
		subKeys.PushBack(L"DisplayIcon");

		MkPathName fullPath;
		fullPath.ConvertToModuleBasisAbsolutePath(iconApp);
		values.PushBack(fullPath);
	}

	if (!publisher.Empty())
	{
		subKeys.PushBack(L"Publisher");
		values.PushBack(publisher);
	}

	if (!url.Empty())
	{
		subKeys.PushBack(L"URLInfoAbout");
		values.PushBack(url);
	}

	if (!uninstallApp.Empty())
	{
		subKeys.PushBack(L"UninstallString");

		MkPathName fullPath;
		fullPath.ConvertToModuleBasisAbsolutePath(uninstallApp);
		values.PushBack(fullPath);
	}

	destNode->CreateUnit(KEY_Subkeys, subKeys);
	destNode->CreateUnit(KEY_Values, values);

	return Write(node);
}

bool MkRegistryOperator::DeleteApplicationInfo(const MkStr& regName)
{
	return Delete(HKEY_LOCAL_MACHINE, MkStr(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\") + regName);
}

bool MkRegistryOperator::RegistCustomURLScheme(const MkStr& protocol, const MkPathName& launcherApp, bool useArgument)
{
	MkDataNode node;
	MkDataNode* destNode = node.CreateChildNode(L"HKEY_CLASSES_ROOT");
	if (destNode == NULL)
		return false;

	if (protocol.Empty())
		return false;

	destNode = destNode->CreateChildNode(protocol);
	if (destNode != NULL)
	{
		MkArray<MkStr> subKeys(2);
		MkArray<MkStr> values(2);

		subKeys.PushBack(MkStr::EMPTY); // default
		subKeys.PushBack(L"URL Protocol");
		values.PushBack(L"URL:" + protocol + L" protocol");
		values.PushBack(MkStr::EMPTY);

		// ex> protocol == L"pwkopw"
		//	str #Subkeys = "" / "URL Protocol";
		//	str #Values = "URL:pwkopw protocol" / "";
		destNode->CreateUnit(KEY_Subkeys, subKeys);
		destNode->CreateUnit(KEY_Values, values);
	}
	else
		return false;

	destNode = destNode->CreateChildNode(L"Shell");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"Open");
	if (destNode == NULL)
		return false;

	destNode = destNode->CreateChildNode(L"Command");
	if (destNode == NULL)
		return false;

	MkStr valMsg = L"\"";
	MkPathName fullPath;
	fullPath.ConvertToModuleBasisAbsolutePath(launcherApp);
	valMsg += fullPath;
	valMsg += (useArgument) ? L"\" \"%1\"" : L"\"";

	MkArray<MkStr> subKeys, values;
	subKeys.PushBack(MkStr::EMPTY); // default
	values.PushBack(valMsg);

	destNode->CreateUnit(KEY_Subkeys, subKeys);
	destNode->CreateUnit(KEY_Values, values);

	return Write(node);
}

bool MkRegistryOperator::DeleteCustomURLScheme(const MkStr& protocol)
{
	return Delete(HKEY_CLASSES_ROOT, protocol);
}

//------------------------------------------------------------------------------------------------//

bool MkRegistryOperator::ReadOp::Open(HKEY hKey, const MkStr& subKey)
{
	Close();

	return (::RegOpenKeyEx(hKey, subKey.Empty() ? NULL : subKey.GetPtr(), 0, KEY_READ, &m_hKey) == ERROR_SUCCESS);
}

bool MkRegistryOperator::ReadOp::Read(const MkStr& subKey, MkStr& value) const
{
	do
	{
		if (m_hKey == NULL)
			break;

		const wchar_t* skName = subKey.Empty() ? NULL : subKey.GetPtr();

		DWORD dwType = REG_NONE;
		if (::RegQueryValueEx(m_hKey, skName, 0, &dwType, NULL, NULL) != ERROR_SUCCESS)
			break;

		if (dwType == REG_SZ)
		{
			DWORD dataSize = 0;
			if (::RegQueryValueEx(m_hKey, skName, 0, NULL, NULL, &dataSize) != ERROR_SUCCESS)
				break;

			// dataSize에는 NULL(wchar_t)이 포함되어 있음. 즉 dataSize는 항상 2 이상으로 나옴
			wchar_t* buffer = new wchar_t[dataSize / 2];
			::ZeroMemory(buffer, dataSize);

			if (::RegQueryValueEx(m_hKey, skName, 0, NULL, reinterpret_cast<BYTE*>(buffer), &dataSize) != ERROR_SUCCESS)
				break;

			value = buffer;
		}
		else if (dwType == REG_DWORD)
		{
			DWORD dataSize = sizeof(DWORD);
			DWORD buffer = 0;
			if (::RegQueryValueEx(m_hKey, skName, 0, NULL, reinterpret_cast<BYTE*>(&buffer), &dataSize) != ERROR_SUCCESS)
				break;

			value = static_cast<unsigned int>(buffer);
		}
		else
			break;

		return true;
	}
	while (false);
	return false;
}

bool MkRegistryOperator::ReadOp::GetAllValues(MkArray<MkStr>& buffer) const
{
	do
	{
		if (m_hKey == NULL)
			break;

		DWORD index = 0;
		while (true)
		{
			wchar_t nameBuf[MAX_PATH] = {0, };
			DWORD dataSize = MAX_PATH;
			LONG status = ::RegEnumValue(m_hKey, index, nameBuf, &dataSize, NULL, NULL, NULL, NULL);
			if (status == ERROR_SUCCESS)
			{
				buffer.PushBack(nameBuf);
			}
			else if (status == ERROR_NO_MORE_ITEMS)
				break;

			++index;
		}

		return true;
	}
	while (false);
	return false;
}

bool MkRegistryOperator::ReadOp::GetAllChildren(MkArray<MkStr>& buffer) const
{
	do
	{
		if (m_hKey == NULL)
			break;

		DWORD index = 0;
		while (true)
		{
			wchar_t nameBuf[MAX_PATH] = {0, };
			DWORD dataSize = MAX_PATH;
			LONG status = ::RegEnumKeyEx(m_hKey, index, nameBuf, &dataSize, NULL, NULL, NULL, NULL);
			if (status == ERROR_SUCCESS)
			{
				buffer.PushBack(nameBuf);
			}
			else if (status == ERROR_NO_MORE_ITEMS)
				break;

			++index;
		}

		return true;
	}
	while (false);
	return false;
}

void MkRegistryOperator::ReadOp::Close(void)
{
	if (m_hKey != NULL)
	{
		::RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

//------------------------------------------------------------------------------------------------//

bool MkRegistryOperator::WriteOp::Open(HKEY hKey, const MkStr& subKey)
{
	MkArray<MkStr> tokens;
	ConvertSubkeyToTokens(subKey, tokens);
	return Open(hKey, tokens);
}

bool MkRegistryOperator::WriteOp::Open(HKEY hKey, const MkArray<MkStr>& tokens)
{
	Close();

	if (tokens.Empty())
	{
		if (::RegOpenKeyEx(hKey, NULL, 0, KEY_ALL_ACCESS, &m_hKey) != ERROR_SUCCESS)
		{
			if (::RegCreateKeyEx(hKey, NULL, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, NULL) == ERROR_SUCCESS)
				return true;
		}
		else
			return true;
	}
	else
	{
		// ex> tokens == L"a", L"b", L"c"
		MkStr currPath;
		MkArray<MkStr> paths(tokens.GetSize());

		MK_INDEXING_LOOP(tokens, i)
		{
			if (i > 0)
			{
				currPath += L"\\";
			}
			currPath += tokens[i];
			paths.PushBack(currPath); // ex> paths == L"a", L"a\\b", L"a\\b\\c"
		}

		unsigned int beginIndex = 0;
		MK_INDEXING_RLOOP(paths, i) // 마지막 path부터 역탐색
		{
			HKEY tmpKey = NULL;
			if (::RegOpenKeyEx(hKey, paths[i].GetPtr(), 0, KEY_ALL_ACCESS, &tmpKey) == ERROR_SUCCESS)
			{
				if (i == (paths.GetSize() - 1))
				{
					m_hKey = tmpKey; // 마지막 path
					return true;
				}
				else
				{
					::RegCloseKey(tmpKey);
					beginIndex = i + 1;
					break;
				}
			}
		}

		for (unsigned int cnt=paths.GetSize(), i=beginIndex; i<cnt; ++i)
		{
			HKEY tmpKey = NULL;
			if (::RegCreateKeyEx(hKey, paths[i].GetPtr(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &tmpKey, NULL) == ERROR_SUCCESS)
			{
				if (i == (cnt - 1))
				{
					m_hKey = tmpKey; // 마지막 path까지 생성 성공
					return true;
				}
				else
				{
					::RegCloseKey(tmpKey);
				}
			}
			else
				break;
		}
	}
	return false;
}

bool MkRegistryOperator::WriteOp::Write(const MkStr& subKey, const MkStr& value) const
{
	if (m_hKey == NULL)
		return false;

	DWORD dataSize = static_cast<DWORD>((value.GetSize() + 1) * sizeof(wchar_t));
	return (::RegSetValueEx(m_hKey, subKey.Empty() ? NULL : subKey.GetPtr(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.GetPtr()), dataSize) == ERROR_SUCCESS);
}

bool MkRegistryOperator::WriteOp::Delete(const MkStr& subKey) const
{
	return (m_hKey == NULL) ? false : (::RegDeleteValue(m_hKey, subKey.Empty() ? NULL : subKey.GetPtr()) == ERROR_SUCCESS);
}

void MkRegistryOperator::WriteOp::Close(void)
{
	if (m_hKey != NULL)
	{
		::RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

void MkRegistryOperator::WriteOp::ConvertSubkeyToTokens(const MkStr& subKey, MkArray<MkStr>& tokens)
{
	MkArray<MkStr> seperator(2);
	seperator.PushBack(L"\\");
	seperator.PushBack(L"/");
	subKey.Tokenize(tokens, seperator);
}

//------------------------------------------------------------------------------------------------//

void MkRegistryOperator::_Read(HKEY rootKey, const MkStr& currSubKey, const ReadOp& currOp, MkDataNode& node)
{
	// value
	MkArray<MkStr> valKeys;
	if (currOp.GetAllValues(valKeys) && (!valKeys.Empty()))
	{
		MkArray<MkStr> subKeys(valKeys.GetSize());
		MkArray<MkStr> values(valKeys.GetSize());
		MK_INDEXING_LOOP(valKeys, i)
		{
			MkStr value;
			if (currOp.Read(valKeys[i], value))
			{
				subKeys.PushBack(valKeys[i]);
				values.PushBack(value);
			}
		}

		if (!subKeys.Empty())
		{
			node.CreateUnit(KEY_Subkeys, subKeys);
			node.CreateUnit(KEY_Values, values);
		}
	}

	// children
	MkArray<MkStr> childKeys;
	if (currOp.GetAllChildren(childKeys) && (!childKeys.Empty()))
	{
		MK_INDEXING_LOOP(childKeys, i)
		{
			MkStr childPath = currSubKey;
			if (!childPath.Empty())
			{
				childPath += L"\\";
			}
			childPath += childKeys[i];

			ReadOp childOp;
			if (childOp.Open(rootKey, childPath))
			{
				MkHashStr nodeName = childKeys[i];
				MkDataNode* destNode = node.ChildExist(nodeName) ? node.GetChildNode(nodeName) : node.CreateChildNode(nodeName);
				MK_CHECK(destNode != NULL, childPath + L" node 생성 실패")
					continue;

				_Read(rootKey, childPath, childOp, *destNode);
			}
		}
	}
}

void MkRegistryOperator::_Write(HKEY rootKey, const MkArray<MkStr>& tokens, const WriteOp& currOp, const MkDataNode& node)
{
	// value
	MkArray<MkStr> subKeys, values;
	if (node.GetData(KEY_Subkeys, subKeys) &&
		node.GetData(KEY_Values, values) &&
		(subKeys.GetSize() > 0) &&
		(subKeys.GetSize() == values.GetSize()))
	{
		MK_INDEXING_LOOP(subKeys, i)
		{
			MK_CHECK(currOp.Write(subKeys[i], values[i]), L"레지스트리에 [" + subKeys[i] + L"=" + values[i] + L" 기록 실패") {}
		}
	}

	// children
	MkArray<MkHashStr> childKeys;
	if (node.GetChildNodeList(childKeys) > 0)
	{
		MK_INDEXING_LOOP(childKeys, i)
		{
			const MkHashStr& currSubKey = childKeys[i];

			MkArray<MkStr> childTokens = tokens;
			childTokens.PushBack(currSubKey.GetString());

			WriteOp childOp;
			if (childOp.Open(rootKey, childTokens))
			{
				_Write(rootKey, childTokens, childOp, *node.GetChildNode(currSubKey));
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//