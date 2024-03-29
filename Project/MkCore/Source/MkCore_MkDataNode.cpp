
#include "MkCore_MkProjectDefinition.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkDataTextToMemoryConverter.h"
#include "MkCore_MkMemoryToDataNodeConverter.h"
#include "MkCore_MkJsonAndDataNodeConverter.h"
#include "MkCore_MkDataNodeToMemoryConverter.h"
#include "MkCore_MkMemoryToDataTextConverter.h"
#include "MkCore_MkExcelToMemoryConverter.h"
#include "MkCore_MkDataNodeToExcelConverter.h"
#include "MkCore_MkDataNode.h"

const MkHashStr MkDataNode::DataTypeTag(L"#_DataTypeTag");
const MkStr MkDataNode::DefaultFileExtension(MKDEF_DEF_DATA_NODE_FILE_EXTENSION);

//------------------------------------------------------------------------------------------------//

bool MkDataNode::LoadFromText(const MkPathName& filePath)
{
	if (!GetReadOnly())
	{
		Clear();
		MkByteArray buffer;
		MkDataTextToMemoryConverter textToMemoryConverter;
		if (textToMemoryConverter.Convert(filePath, buffer))
		{
			MkMemoryToDataNodeConverter memoryToDataNodeConverter;
			return memoryToDataNodeConverter.ConvertMemory(buffer, *this);
		}
	}
	return false;
}

bool MkDataNode::LoadFromText(const MkByteArray& fileData)
{
	if (!GetReadOnly())
	{
		Clear();
		MkByteArray buffer;
		MkDataTextToMemoryConverter textToMemoryConverter;
		if (textToMemoryConverter.Convert(fileData, buffer))
		{
			MkMemoryToDataNodeConverter memoryToDataNodeConverter;
			return memoryToDataNodeConverter.ConvertMemory(buffer, *this);
		}
	}
	return false;
}

bool MkDataNode::LoadFromJson(const MkPathName& filePath)
{
	if (!GetReadOnly())
	{
		Clear();
		
		MkJsonAndDataNodeConverter converter;
		return converter.Convert(filePath, *this);
	}
	return false;
}

bool MkDataNode::LoadFromJson(const MkByteArray& fileData)
{
	if (!GetReadOnly())
	{
		Clear();

		MkJsonAndDataNodeConverter converter;
		return converter.Convert(fileData, *this);
	}
	return false;
}

bool MkDataNode::LoadFromBinary(const MkPathName& filePath)
{
	if (!GetReadOnly())
	{
		Clear();
		MkMemoryToDataNodeConverter memoryToDataNodeConverter;
		return memoryToDataNodeConverter.ConvertBinaryFile(filePath, *this);
	}
	return false;
}

bool MkDataNode::LoadFromBinary(const MkByteArray& fileData)
{
	if (!GetReadOnly())
	{
		Clear();
		MkMemoryToDataNodeConverter memoryToDataNodeConverter;
		return memoryToDataNodeConverter.ConvertBinaryData(fileData, *this);
	}
	return false;
}

bool MkDataNode::LoadFromExcel(const MkPathName& filePath)
{
	if (!GetReadOnly())
	{
		Clear();
		MkByteArray buffer;
		MkExcelToMemoryConverter excelToMemoryConverter;
		if (excelToMemoryConverter.Convert(filePath, buffer))
		{
			MkMemoryToDataNodeConverter memoryToDataNodeConverter;
			return memoryToDataNodeConverter.ConvertMemory(buffer, *this);
		}
	}
	return false;
}

bool MkDataNode::Load(const MkPathName& filePath)
{
	eLoadResult result;
	return Load(filePath, result);
}

bool MkDataNode::Load(const MkByteArray& fileData)
{
	eLoadResult result;
	return Load(fileData, result);
}

bool MkDataNode::Load(const MkPathName& filePath, eLoadResult& result)
{
	if (GetReadOnly())
	{
		result = eLR_ReadOnly;
	}
	else
	{
		MkStr ext = filePath.GetFileExtension();
		ext.ToLower();
		
		if (ext.CheckPrefix(L"xls")) // Excel 파일인지 확장자 체크
		{
			result = LoadFromExcel(filePath) ? eLR_Excel : eLR_Failed;
		}
		else if (ext.CheckPrefix(L"json")) // JSON 파일인지 확장자 체크
		{
			result = LoadFromJson(filePath) ? eLR_Json : eLR_Failed;
		}
		else
		{
			MkByteArray fileData;
			MK_CHECK(MkFileManager::GetFileData(filePath, fileData), MkStr(filePath) + L" 경로 파일 읽기 실패")
			{
				result = eLR_Failed;
				return false;
			}

			return Load(fileData, result);
		}
	}
	return (result > eLR_Failed);
}

bool MkDataNode::Load(const MkByteArray& fileData, eLoadResult& result)
{
	// 먼저 binary format으로 읽기 시도(tag 검사)
	// 실패하면 text, json format으로 읽기 시도
	if (GetReadOnly())
	{
		result = eLR_ReadOnly;
	}
	else
	{
		if (LoadFromBinary(fileData))
		{
			result = eLR_Binary;
		}
		else if (LoadFromText(fileData))
		{
			result = eLR_Text;
		}
		else if (LoadFromJson(fileData))
		{
			result = eLR_Json;
		}
		else
		{
			result = eLR_Failed;
		}
	}
	return (result > eLR_Failed);
}

bool MkDataNode::SaveToJson(const MkPathName& filePath) const
{
	MkJsonAndDataNodeConverter jsonAndDataNodeConverter;
	return jsonAndDataNodeConverter.Convert(*this, filePath);
}

bool MkDataNode::SaveToText(const MkPathName& filePath) const
{
	MkByteArray buffer;
	MkDataNodeToMemoryConverter dataNodeToMemoryConverter;
	if (dataNodeToMemoryConverter.ConvertToMemory(*this, buffer))
	{
		MkMemoryToDataTextConverter memoryToDataTextConverter;
		return memoryToDataTextConverter.Convert(buffer, filePath);
	}
	return false;
}

bool MkDataNode::SaveToBinary(const MkPathName& filePath) const
{
	MkDataNodeToMemoryConverter dataNodeToMemoryConverter;
	return dataNodeToMemoryConverter.ConvertToBinaryFile(*this, filePath);
}

bool MkDataNode::SaveToBinary(MkByteArray& fileData) const
{
	MkDataNodeToMemoryConverter dataNodeToMemoryConverter;
	return dataNodeToMemoryConverter.ConvertToBinaryData(*this, fileData);
}

bool MkDataNode::SaveToExcel(const MkPathName& filePath) const
{
	MkDataNodeToExcelConverter dataNodeToExcelConverter;
	return dataNodeToExcelConverter.Convert(*this, filePath);
}

//------------------------------------------------------------------------------------------------//

bool MkDataNode::ChangeNodeName(const MkHashStr& newName)
{
	return IsDefinedNodeByTemplate() ? false : MkSingleTypeTreePattern<MkDataNode>::ChangeNodeName(newName);
}

bool MkDataNode::ChangeChildNodeName(const MkHashStr& oldName, const MkHashStr& newName)
{
	if (m_ChildrenNode.Exist(oldName))
	{
		if (m_ChildrenNode[oldName]->IsDefinedNodeByTemplate())
			return false;
	}
	return MkSingleTypeTreePattern<MkDataNode>::ChangeChildNodeName(oldName, newName);
}

bool MkDataNode::DetachChildNode(const MkHashStr& childNodeName)
{
	return _PredefinedChildNodeByTemplate(childNodeName) ? false : MkSingleTypeTreePattern<MkDataNode>::DetachChildNode(childNodeName);
}

//------------------------------------------------------------------------------------------------//

bool MkDataNode::IsValidKey(const MkHashStr& key) const { return m_DataPack.IsValidKey(key); }
unsigned int MkDataNode::GetUnitCount(void) const { return m_DataPack.GetUnitCount(); }
unsigned int MkDataNode::GetDataSize(const MkHashStr& key) const { return m_DataPack.GetDataSize(key); }
ePrimitiveDataType MkDataNode::GetUnitType(const MkHashStr& key) const { return m_DataPack.GetUnitType(key); }
unsigned int MkDataNode::GetUnitKeyList(MkArray<MkHashStr>& keyList) const { return m_DataPack.GetUnitKeyList(keyList); }

bool MkDataNode::CreateUnit(const MkHashStr& key, ePrimitiveDataType type, unsigned int size) { return GetReadOnly() ? false : m_DataPack.CreateUnit(key, type, size); }

bool MkDataNode::CreateUnit(const MkHashStr& key, bool value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<bool>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, int value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<int>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, unsigned int value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<unsigned int>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, __int64 value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<__int64>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, unsigned __int64 value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<unsigned __int64>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, float value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<float>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkInt2& value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkInt2>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkVec2& value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkVec2>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkVec3& value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkVec3>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkStr& value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkStr>(), 1) ? SetData(key, value, 0) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkByteArray& value) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkByteArray>(), 1) ? SetData(key, value, 0) : false; }

bool MkDataNode::CreateUnitEx(const MkHashStr& key, const MkFloat2& value) { return CreateUnit(key, MkVec2(value.x, value.y)); }
bool MkDataNode::CreateUnitEx(const MkHashStr& key, const MkHashStr& value) { return CreateUnit(key, value.GetString()); }

// SetData()에서 다시 크기를 잡으므로 CreateUnit()에서 size는 의미 없음
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<bool>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<bool>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<int>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<int>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<unsigned int>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<unsigned int>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<__int64>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<__int64>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<unsigned __int64>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<unsigned __int64>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<float>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<float>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<MkInt2>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkInt2>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<MkVec2>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkVec2>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<MkVec3>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkVec3>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<MkStr>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkStr>(), 1) ? SetData(key, values) : false; }
bool MkDataNode::CreateUnit(const MkHashStr& key, const MkArray<MkByteArray>& values) { return CreateUnit(key, MkPrimitiveDataType::GetEnum<MkByteArray>(), 1) ? SetData(key, values) : false; }

bool MkDataNode::CreateUnitEx(const MkHashStr& key, const MkArray<MkFloat2>& values)
{
	MkArray<MkVec2> buffer(values.GetSize());
	MK_INDEXING_LOOP(values, i)
	{
		buffer.PushBack(MkVec2(values[i].x, values[i].y));
	}
	return CreateUnit(key, buffer);
}

bool MkDataNode::CreateUnitEx(const MkHashStr& key, const MkArray<MkHashStr>& values)
{
	MkArray<MkStr> buffer(values.GetSize());
	MK_INDEXING_LOOP(values, i)
	{
		buffer.PushBack(values[i].GetString());
	}
	return CreateUnit(key, buffer);
}

bool MkDataNode::Expand(const MkHashStr& key, unsigned int size) { return GetReadOnly() ? false : m_DataPack.Expand(key, size); }

bool MkDataNode::SetData(const MkHashStr& key, bool value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, int value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, unsigned int value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, __int64 value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, unsigned __int64 value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, float value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, const MkInt2& value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, const MkVec2& value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, const MkVec3& value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, const MkStr& value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }
bool MkDataNode::SetData(const MkHashStr& key, const MkByteArray& value, unsigned int index) { return GetReadOnly() ? false : m_DataPack.SetData(key, value, index); }

bool MkDataNode::SetDataEx(const MkHashStr& key, const MkFloat2& value, unsigned int index) { return SetData(key, MkVec2(value.x, value.y), index); }
bool MkDataNode::SetDataEx(const MkHashStr& key, const MkHashStr& value, unsigned int index) { return SetData(key, value.GetString(), index); }

bool MkDataNode::SetData(const MkHashStr& key, const MkArray<bool>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<int>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<unsigned int>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<__int64>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<unsigned __int64>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<float>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<MkInt2>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<MkVec2>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<MkVec3>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<MkStr>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }
bool MkDataNode::SetData(const MkHashStr& key, const MkArray<MkByteArray>& values) { return GetReadOnly() ? false : m_DataPack.SetData(key, values); }

bool MkDataNode::SetDataEx(const MkHashStr& key, const MkArray<MkFloat2>& values)
{
	MkArray<MkVec2> buffer(values.GetSize());
	MK_INDEXING_LOOP(values, i)
	{
		buffer.PushBack(MkVec2(values[i].x, values[i].y));
	}
	return SetData(key, buffer);
}

bool MkDataNode::SetDataEx(const MkHashStr& key, const MkArray<MkHashStr>& values)
{
	MkArray<MkStr> buffer(values.GetSize());
	MK_INDEXING_LOOP(values, i)
	{
		buffer.PushBack(values[i].GetString());
	}
	return SetData(key, buffer);
}

bool MkDataNode::GetData(const MkHashStr& key, bool& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, int& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, unsigned int& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, __int64& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, unsigned __int64& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, float& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, MkInt2& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, MkVec2& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, MkVec3& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, MkStr& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }
bool MkDataNode::GetData(const MkHashStr& key, MkByteArray& buffer, unsigned int index) const { return m_DataPack.GetData(key, buffer, index); }

bool MkDataNode::GetDataEx(const MkHashStr& key, MkFloat2& buffer, unsigned int index) const
{
	MkVec2 v2;
	bool ok = GetData(key, v2, index);
	if (ok)
	{
		buffer.x = v2.x;
		buffer.y = v2.y;
	}
	return ok;
}

bool MkDataNode::GetDataEx(const MkHashStr& key, MkHashStr& buffer, unsigned int index) const
{
	MkStr str;
	bool ok = GetData(key, str, index);
	if (ok)
	{
		buffer = str;
	}
	return ok;
}

bool MkDataNode::GetData(const MkHashStr& key, MkArray<bool>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<int>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<unsigned int>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<__int64>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<unsigned __int64>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<float>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<MkInt2>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<MkVec2>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<MkVec3>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<MkStr>& buffers) const { return m_DataPack.GetData(key, buffers); }
bool MkDataNode::GetData(const MkHashStr& key, MkArray<MkByteArray>& buffers) const { return m_DataPack.GetData(key, buffers); }

bool MkDataNode::GetDataEx(const MkHashStr& key, MkArray<MkFloat2>& buffers) const
{
	MkArray<MkVec2> v2s;
	bool ok = GetData(key, v2s);
	if (ok && (!v2s.Empty()))
	{
		buffers.Reserve(v2s.GetSize());
		MK_INDEXING_LOOP(v2s, i)
		{
			buffers.PushBack(MkFloat2(v2s[i].x, v2s[i].y));
		}
	}
	return ok;
}

bool MkDataNode::GetDataEx(const MkHashStr& key, MkArray<MkHashStr>& buffers) const
{
	MkArray<MkStr> strs;
	bool ok = GetData(key, strs);
	if (ok && (!strs.Empty()))
	{
		buffers.Reserve(strs.GetSize());
		MK_INDEXING_LOOP(strs, i)
		{
			buffers.PushBack(strs[i]);
		}
	}
	return ok;
}

bool MkDataNode::RemoveUnit(const MkHashStr& key)
{
	return (GetReadOnly() || _PredefinedUnitByTemplate(key)) ? false : m_DataPack.RemoveUnit(key);
}

bool MkDataNode::RemoveData(const MkHashStr& key, unsigned int index)
{
	if (GetReadOnly())
		return false;

	if ((index == 0) && (m_DataPack.GetDataSize(key) == 1) && _PredefinedUnitByTemplate(key))
		return false;
	
	return m_DataPack.RemoveData(key, index);
}

bool MkDataNode::Empty(void) const
{
	return ((GetUnitCount() == 0) && m_ChildrenNode.Empty());
}

void MkDataNode::SetDataTypeTag(const MkStr& tag)
{
	if (IsValidKey(DataTypeTag))
	{
		if (tag.Empty())
		{
			RemoveUnit(DataTypeTag);
		}
		else
		{
			SetData(DataTypeTag, tag, 0);
		}
	}
	else if (!tag.Empty())
	{
		CreateUnit(DataTypeTag, tag);
	}
}

MkStr MkDataNode::GetDataTypeTag(void) const
{
	MkStr tag;
	if (IsValidKey(DataTypeTag))
	{
		GetData(DataTypeTag, tag, 0);
	}
	return tag;
}

void MkDataNode::Rehash(void)
{
	if (!GetReadOnly())
	{
		m_DataPack.Rehash();
	}
	MkSingleTypeTreePattern<MkDataNode>::Rehash();
}

void MkDataNode::Clear(void)
{
	if (!GetReadOnly())
	{
		m_DataPack.Clear();
	}
	MkSingleTypeTreePattern<MkDataNode>::Clear();

	m_TemplateLink = NULL;
}

//------------------------------------------------------------------------------------------------//

MkDataNode& MkDataNode::operator = (const MkDataNode& source)
{
	if (!GetReadOnly())
	{
		Clear();

		m_DataPack = source.GetDataPack();
		m_TemplateLink = const_cast<MkDataNode*>(source.GetTemplateLink());

		MkArray<MkHashStr> nodeNameList;
		source.GetChildNodeList(nodeNameList);
		MK_INDEXING_LOOP(nodeNameList, i)
		{
			const MkHashStr& currName = nodeNameList[i];
			MkDataNode* childNode = CreateChildNode(currName);
			assert(childNode != NULL);
			*childNode = *source.GetChildNode(currName);
		}
	}
	return *this;
}

//------------------------------------------------------------------------------------------------//

bool MkDataNode::RegisterTemplate(const MkPathName& filePath)
{
	MkDataNode node;
	return node.Load(filePath);
}

bool MkDataNode::RegisterTemplate(const MkByteArray& fileData)
{
	MkDataNode node;
	return node.Load(fileData);
}

bool MkDataNode::DeclareToTemplate(bool checkDuplication) const
{
	const MkHashStr& nodeName = GetNodeName();
	MK_CHECK(!GetReadOnly(), MkStr(nodeName) + L" 노드는 read-only 상태이어서 템플릿화 할 수 없음")
		return false;

	MkDataNode& templateRoot = __GetTemplateRoot();
	if (checkDuplication)
	{
		MK_CHECK(!templateRoot.ChildExist(nodeName), L"이미 " + MkStr(nodeName) + L" 템플릿 노드가 등록되어 있음")
			return false;
	}
	else
	{
		if (templateRoot.ChildExist(nodeName)) // 이미 해당 이름의 템플릿 노드가 존재하면 무시
			return true;
	}

	unsigned int priority = templateRoot.GetChildNodeCount();
	MkDataNode* newNode = templateRoot.CreateChildNode(nodeName);
	assert(newNode != NULL);
	*newNode = *this;
	templateRoot.Rehash();
	newNode->SetReadOnly(true); // 템플릿 노드는 수정 불가

	MkHashMap<MkHashStr, unsigned int>& priorityTable = __GetPriorityTable();
	priorityTable.Create(nodeName, priority);
	priorityTable.Rehash();
	return true;
}

bool MkDataNode::ApplyTemplate(const MkHashStr& templateName)
{
	const MkHashStr& nodeName = GetNodeName();
	MK_CHECK(!GetReadOnly(), MkStr(nodeName) + L" 노드는 read-only 상태이어서 템플릿 적용 할 수 없음")
		return false;

	MK_CHECK(m_TemplateLink == NULL, MkStr(nodeName) + L" 노드는 이미 템플릿이 적용된 상태이어서 새 템플릿을 적용 할 수 없음")
		return false;

	MkDataNode& templateRoot = __GetTemplateRoot();
	MK_CHECK(templateRoot.ChildExist(templateName), MkStr(templateName) + L" 템플릿 노드가 등록되지 않아 " + MkStr(nodeName) + L" 노드에 적용 불가")
		return false;

	__CopyFromTemplateNode(templateRoot.GetChildNode(templateName));
	return true;
}

bool MkDataNode::IsTemplateNode(void) const
{
	const MkHashStr& nodeName = GetNodeName();
	MkDataNode& templateRoot = __GetTemplateRoot();
	return templateRoot.ChildExist(nodeName);
}

bool MkDataNode::GetAppliedTemplateName(MkHashStr& name) const
{
	if (m_TemplateLink == NULL)
		return false;

	const MkHashStr& currTemplateName = m_TemplateLink->GetNodeName();
	bool ok = __GetTemplateRoot().ChildExist(currTemplateName);
	if (ok)
	{
		name = currTemplateName;
	}
	return ok;
}

bool MkDataNode::IsDefinedUnitByTemplate(const MkHashStr& key) const
{
	return ((m_TemplateLink != NULL) && IsValidKey(key) && m_TemplateLink->GetDataPack().IsValidKey(key));
}

bool MkDataNode::IsDefinedNodeByTemplate(void) const
{
	return (m_TemplateLink == NULL) ? false : (m_TemplateLink->GetNodeName() == GetNodeName());
}

void MkDataNode::ClearAllTemplate(void)
{
	__GetTemplateRoot().Clear();
	__GetPriorityTable().Clear();
}

MkDataNode::MkDataNode() : MkSingleTypeTreePattern<MkDataNode>()
{
	m_TemplateLink = NULL;
}

MkDataNode::MkDataNode(const MkHashStr& name) : MkSingleTypeTreePattern<MkDataNode>(name)
{
	m_TemplateLink = NULL;
}

MkDataNode::MkDataNode(const MkDataNode& node) : MkSingleTypeTreePattern<MkDataNode>()
{
	m_TemplateLink = NULL;
	*this = node;
}

//------------------------------------------------------------------------------------------------//

void MkDataNode::__CopyFromTemplateNode(const MkDataNode* source)
{
	assert(source != NULL);

	m_DataPack = source->GetDataPack();
	m_TemplateLink = const_cast<MkDataNode*>(source);

	MkArray<MkHashStr> nodeNameList;
	source->GetChildNodeList(nodeNameList);
	MK_INDEXING_LOOP(nodeNameList, i)
	{
		const MkHashStr& currName = nodeNameList[i];
		MkDataNode* childNode = CreateChildNode(currName);
		assert(childNode != NULL);
		childNode->__CopyFromTemplateNode(source->GetChildNode(currName));
	}
}

bool MkDataNode::__IsValidUnit(const MkHashStr& key) const
{
	if (!IsValidKey(key))
		return false;

	if (m_TemplateLink == NULL)
		return true;

	return (!m_DataPack.Equals(m_TemplateLink->GetDataPack(), key));
}

void MkDataNode::__GetValidUnitList(MkArray<MkHashStr>& keyList) const
{
	if (m_TemplateLink == NULL)
	{
		m_DataPack.GetUnitKeyList(keyList);
	}
	else
	{
		m_DataPack.GetValidUnitList(m_TemplateLink->GetDataPack(), keyList);
	}
}

bool MkDataNode::__IsValidNode(bool rootApplied) const
{
	if (m_TemplateLink == NULL)
		return true;

	// 템플릿 적용된 노드는 해당 원본에 속한 노드들을 떼어낼 수 없으므로 크기가 같다면 동일 노드 유지를 의미
	if (GetChildNodeCount() > m_TemplateLink->GetChildNodeCount())
		return true;

	// 템플릿이 적용되었더라도 root이면 정보 표시
	if (rootApplied)
	{
		MkHashStr tmpName;
		if (GetAppliedTemplateName(tmpName))
			return true;
	}

	// 유닛 정보 동일여부 확인
	if (m_DataPack != m_TemplateLink->GetDataPack())
		return true;

	// 하위 노드 순환
	MkConstHashMapLooper<MkHashStr, MkDataNode*> looper(m_ChildrenNode);
	MK_STL_LOOP(looper)
	{
		if (looper.GetCurrentField()->__IsValidNode(rootApplied))
			return true;
	}

	return false;
}

void MkDataNode::__GetClassifiedChildNodeNameList(MkArray<MkHashStr>& templateNodeList, MkArray<MkHashStr>& normalNodeList, bool validOnly) const
{
	MkArray<MkHashStr> childNodeList;
	if (GetChildNodeList(childNodeList) == 0)
		return;

	// 출력 가치가 있는(고유 정보를 지닌) 노드만 걸러냄
	if (validOnly)
	{
		MkArray<MkHashStr> validNodeList;
		validNodeList.Reserve(childNodeList.GetSize());
		MK_INDEXING_LOOP(childNodeList, i)
		{
			const MkHashStr& currName = childNodeList[i];
			if (GetChildNode(currName)->__IsValidNode())
			{
				validNodeList.PushBack(currName);
			}
		}
		childNodeList = validNodeList;
	}

	// 분류
	normalNodeList.Reserve(childNodeList.GetSize());
	MkMap<unsigned int, MkHashStr> templateNodes; // priority 기준 정렬용 테이블
	MkHashMap<MkHashStr, unsigned int>& priorityTable = __GetPriorityTable();

	MK_INDEXING_LOOP(childNodeList, i)
	{
		const MkHashStr& currName = childNodeList[i];
		if (priorityTable.Exist(currName))
		{
			templateNodes.Create(priorityTable[currName], currName);
		}
		else
		{
			normalNodeList.PushBack(currName);
		}
	}

	if (!templateNodes.Empty())
	{
		templateNodes.GetFieldList(templateNodeList);
	}
	if (!normalNodeList.Empty())
	{
		normalNodeList.SortInAscendingOrder();
	}
}

bool MkDataNode::__SameForm(const MkDataNode& source) const
{
	if (GetChildNodeCount() != source.GetChildNodeCount())
		return false;

	if (!m_DataPack.SameForm(source.GetDataPack()))
		return false;

	MkArray<MkHashStr> childNodeList;
	GetChildNodeList(childNodeList);
	MK_INDEXING_LOOP(childNodeList, i)
	{
		if (!source.ChildExist(childNodeList[i]))
			return false;
	}
	return true;
}

MkDataNode& MkDataNode::__GetTemplateRoot(void)
{
	static MkDataNode sTemplateRoot(L"TemplateRoot");
	return sTemplateRoot;
}

MkHashMap<MkHashStr, unsigned int>& MkDataNode::__GetPriorityTable(void)
{
	static MkHashMap<MkHashStr, unsigned int> sPriorityTable;
	return sPriorityTable;
}

//------------------------------------------------------------------------------------------------//

bool MkDataNode::_PredefinedUnitByTemplate(const MkHashStr& key) const
{
	return (m_TemplateLink == NULL) ? false : m_TemplateLink->IsValidKey(key);
}

bool MkDataNode::_PredefinedChildNodeByTemplate(const MkHashStr& nodeName) const
{
	return (m_TemplateLink == NULL) ? false : m_TemplateLink->ChildExist(nodeName);
}

//------------------------------------------------------------------------------------------------//
