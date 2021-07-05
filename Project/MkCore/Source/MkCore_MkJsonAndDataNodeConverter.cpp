
#include <fstream>
#include "MkCore_MkCheck.h"
#include "MkCore_MkStringOp.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkJsonAndDataNodeConverter.h"

#pragma comment (lib, "JsonCpp.lib")

static MkHashStr OBJECT_LIST_KEY(L"__#ObjectArray");

#define MKDEF_PUSH_ARRAY_ELEMENT(DATATYPE, CMETHOD) \
	MkArray<DATATYPE> arrBuf(memberSize); \
	for (unsigned int i = 0; i < memberSize; ++i) { arrBuf.PushBack(targetValue[i].CMETHOD()); } \
	if (!parentNode.CreateUnit(targetKey, arrBuf)) \
		return false

#define MKDEF_ASSIGN_UNIT_DATA(DATATYPE) \
	{ \
		MkArray<DATATYPE> units; \
		parentNode.GetData(currName, units); \
		if (units.GetSize() > 0) \
		{ \
			std::string nameBuf; \
			MkStringOp::ConvertString(std::wstring(currName.GetString().GetPtr()), nameBuf, CP_UTF8); \
			if (units.GetSize() == 1) { parentValue[nameBuf.c_str()] = units[0]; } \
			else \
			{ \
				Json::Value value; \
				MK_INDEXING_LOOP(units, j) { value.append(units[j]); } \
				parentValue[nameBuf.c_str()] = value; \
			} \
		} \
	} \

//------------------------------------------------------------------------------------------------//

bool MkJsonAndDataNodeConverter::Convert(const MkByteArray& source, MkDataNode& destination)
{
	JSONCPP_STRING err;
	Json::Value root;
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();

	bool ok = reader->parse(reinterpret_cast<const char*>(source.GetPtr()), reinterpret_cast<const char*>(source.GetPtr()) + source.GetSize(), &root, &err);
	MK_DELETE(reader);

	MK_CHECK(ok, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일 파싱 실패")
		return false;

	return _ConvertObject(root, destination);
}

bool MkJsonAndDataNodeConverter::Convert(const MkPathName& filePath, MkDataNode& destination)
{
	MkByteArray txtBuf;
	MK_CHECK(MkFileManager::GetFileData(filePath, txtBuf), L"데이터 노드 구성을 위한 " + MkStr(filePath) + L" JSON 파일 읽기 실패")
		return false;

	m_TargetFilePath = filePath;
	return Convert(txtBuf, destination);
}

bool MkJsonAndDataNodeConverter::Convert(const MkDataNode& node, const MkPathName& filePath)
{
	Json::Value rootValue;
	if (!_ConvertNode(node, rootValue))
		return false;

	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);

	std::string strPath;
	fullPath.ExportMultiByteString(strPath);

	std::ofstream outStream;
	outStream.open(strPath.c_str(), std::ios::out | std::ios::trunc);
	if (!outStream.is_open())
		return false;

	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "\t";

	Json::StreamWriter* writer = builder.newStreamWriter();
	writer->write(rootValue, &outStream);
	outStream.close();
	MK_DELETE(writer);

	return true;
}

void MkJsonAndDataNodeConverter::Clear(void)
{
	m_TargetFilePath.Clear();
}

//------------------------------------------------------------------------------------------------//

bool MkJsonAndDataNodeConverter::_ConvertObject(const Json::Value& rootValue, MkDataNode& rootNode)
{
	Json::Value::Members mbr = rootValue.getMemberNames();
	for (Json::Value::Members::const_iterator itr = mbr.begin(); itr != mbr.end(); ++itr)
	{
		std::wstring strVal;
		MkStringOp::ConvertString(std::string(reinterpret_cast<const char*>((*itr).c_str())), strVal, CP_UTF8);

		const Json::Value& childValue = rootValue[(*itr)];

		if (!_ConvertValue(MkHashStr(strVal.c_str()), childValue, rootNode))
			return false;
	}
	return true;
}

bool MkJsonAndDataNodeConverter::_ConvertValue(const MkHashStr& targetKey, const Json::Value& targetValue, MkDataNode& parentNode)
{
	if (targetValue.isObject())
	{
		if (!_ConvertObject(targetValue, *parentNode.CreateChildNode(targetKey)))
			return false;
	}
	else if (targetValue.isArray())
	{
		unsigned int memberSize = targetValue.size();
		if (memberSize > 0)
		{
			const Json::Value& firstValue = targetValue[0];
			Json::ValueType valueType = firstValue.type();
			if (firstValue.isObject())
			{
				MkDataNode* targetNode = parentNode.CreateChildNode(targetKey);
				targetNode->CreateUnit(OBJECT_LIST_KEY, memberSize);

				for (unsigned int i = 0; i < memberSize; ++i)
				{
					if (!_ConvertValue(MkHashStr(i), targetValue[i], *targetNode))
						return false;
				}
			}
			else if (firstValue.isBool())
			{
				MKDEF_PUSH_ARRAY_ELEMENT(bool, asBool);
			}
			else if (firstValue.isInt())
			{
				MKDEF_PUSH_ARRAY_ELEMENT(int, asInt);
			}
			else if (firstValue.isInt64())
			{
				MKDEF_PUSH_ARRAY_ELEMENT(__int64, asInt64);
			}
			else if (firstValue.isUInt())
			{
				MKDEF_PUSH_ARRAY_ELEMENT(unsigned int, asUInt);
			}
			else if (firstValue.isUInt64())
			{
				MKDEF_PUSH_ARRAY_ELEMENT(unsigned __int64, asUInt64);
			}
			else if (valueType == Json::realValue) // double은 float으로 대체
			{
				MKDEF_PUSH_ARRAY_ELEMENT(float, asFloat);
			}
			else if (firstValue.isString())
			{
				MkArray<MkStr> arrBuf(memberSize);
				for (unsigned int i = 0; i < memberSize; ++i)
				{
					std::string tmpStr = targetValue[i].asString();
					std::wstring strVal;
					MkStringOp::ConvertString(std::string(reinterpret_cast<const char*>(tmpStr.c_str())), strVal, CP_UTF8);
					arrBuf.PushBack(MkStr(strVal.c_str()));
				}
				if (!parentNode.CreateUnit(targetKey, arrBuf))
					return false;
			}
			else if (firstValue.isArray())
			{
				MK_CHECK(false, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일(" + targetKey.GetString() + L") : 배열 안 배열은 허용되지 않음")
					return false;
			}
			else if (firstValue.isIntegral())
			{
				MK_CHECK(false, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일(" + targetKey.GetString() + L") : integral type은 허용되지 않음")
					return false;
			}
			else if (firstValue.isNumeric())
			{
				MK_CHECK(false, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일(" + targetKey.GetString() + L") : numeric type은 허용되지 않음")
					return false;
			}
		}
	}
	else if (targetValue.isBool())
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asBool()))
			return false;
	}
	else if (targetValue.isInt())
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asInt()))
			return false;
	}
	else if (targetValue.isInt64())
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asInt64()))
			return false;
	}
	else if (targetValue.isUInt())
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asUInt()))
			return false;
	}
	else if (targetValue.isUInt64())
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asUInt64()))
			return false;
	}
	else if (targetValue.type() == Json::realValue) // double은 float으로 대체
	{
		if (!parentNode.CreateUnit(targetKey, targetValue.asFloat()))
			return false;
	}
	else if (targetValue.isString())
	{
		std::string tmpStr = targetValue.asString();
		std::wstring strVal;
		MkStringOp::ConvertString(std::string(reinterpret_cast<const char*>(tmpStr.c_str())), strVal, CP_UTF8);
		if (!parentNode.CreateUnit(targetKey, MkStr(strVal.c_str())))
			return false;
	}
	else if (targetValue.isIntegral())
	{
		MK_CHECK(false, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일(" + targetKey.GetString() + L") : integral type은 허용되지 않음")
			return false;
	}
	else if (targetValue.isNumeric())
	{
		MK_CHECK(false, L"데이터 노드 구성을 위한 " + m_TargetFilePath + L" JSON 파일(" + targetKey.GetString() + L") : numeric type은 허용되지 않음")
			return false;
	}
	return true;
}

bool MkJsonAndDataNodeConverter::_ConvertNode(const MkDataNode& parentNode, Json::Value& parentValue)
{
	// unit
	MkArray<MkHashStr> unitKeys;
	parentNode.GetUnitKeyList(unitKeys);
	MK_INDEXING_LOOP(unitKeys, i)
	{
		const MkHashStr& currName = unitKeys[i];
		switch (parentNode.GetUnitType(currName))
		{
		case ePDT_Bool: MKDEF_ASSIGN_UNIT_DATA(bool) break;
		case ePDT_Int: MKDEF_ASSIGN_UNIT_DATA(int) break;
		case ePDT_UnsignedInt: MKDEF_ASSIGN_UNIT_DATA(unsigned int) break;
		case ePDT_DInt:  MKDEF_ASSIGN_UNIT_DATA(__int64) break;
		case ePDT_DUnsignedInt:  MKDEF_ASSIGN_UNIT_DATA(unsigned __int64) break;
		case ePDT_Float:  MKDEF_ASSIGN_UNIT_DATA(float) break;
		case ePDT_Str:
			{
				MkArray<MkStr> units;
				parentNode.GetData(currName, units);

				if (units.GetSize() > 0)
				{
					std::string nameBuf;
					MkStringOp::ConvertString(std::wstring(currName.GetString().GetPtr()), nameBuf, CP_UTF8);

					if (units.GetSize() == 1)
					{
						std::string tmpBuf;
						MkStringOp::ConvertString(std::wstring(units[0].GetPtr()), tmpBuf, CP_UTF8);
						parentValue[nameBuf.c_str()] = tmpBuf.c_str();
					}
					else
					{
						Json::Value value;
						MK_INDEXING_LOOP(units, j)
						{
							std::string tmpBuf;
							MkStringOp::ConvertString(std::wstring(units[j].GetPtr()), tmpBuf, CP_UTF8);
							value.append(tmpBuf.c_str());
						}
						parentValue[nameBuf.c_str()] = value;
					}
				}
			}
			break;
		}
	}

	// node
	MkArray<MkHashStr> nodeKeys;
	parentNode.GetChildNodeList(nodeKeys);
	MK_INDEXING_LOOP(nodeKeys, i)
	{
		const MkHashStr& currName = nodeKeys[i];
		const MkDataNode* currNode = parentNode.GetChildNode(currName);

		std::string nameBuf;
		MkStringOp::ConvertString(std::wstring(currName.GetString().GetPtr()), nameBuf, CP_UTF8);

		// object array
		unsigned int objectListCnt = 0;
		if (currNode->GetData(OBJECT_LIST_KEY, objectListCnt, 0))
		{
			Json::Value objectList;

			for (unsigned int j = 0; j < objectListCnt; ++j)
			{
				MkHashStr indexKey = j;
				const MkDataNode* indexNode = currNode->GetChildNode(indexKey);
				MK_CHECK(indexNode != NULL, L"JSON 출력을 위한 " + currName.GetString() + L" node의 object list에 " + indexKey.GetString() + L"번 object가 없음")
					return false;

				Json::Value indexValue;
				if (!_ConvertNode(*indexNode, indexValue))
					return false;

				objectList.append(indexValue);
			}

			parentValue[nameBuf.c_str()] = objectList;
		}
		// single object
		else
		{
			Json::Value currValue;
			if (!_ConvertNode(*currNode, currValue))
				return false;

			parentValue[nameBuf.c_str()] = currValue;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//
