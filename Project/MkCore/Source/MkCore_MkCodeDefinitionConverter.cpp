
#include "MkCore_MkDataNode.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkCodeDefinitionConverter.h"


static const MkHashStr KEY_Services = L"#Services";
static const MkHashStr KEY_ExportTo = L"#ExportTo";
static const MkHashStr KEY_Target = L"#Target";
static const MkHashStr KEY_Default = L"#Default";

//------------------------------------------------------------------------------------------------//

bool __MK_CDC_ERROR(const MkStr& msg)
{
	::MessageBox(NULL, msg.GetPtr(), L"MkCodeDefinitionConverter", MB_OK);
	return false;
}

template <class DataType>
class __TSI_GetAndSetValue
{
public:
	static void Proceed(MkStr& strBuffer, const MkDataNode& node, const MkHashStr& service, const MkHashStr& contents, bool addDQM, const MkStr& prefix)
	{
		DataType value;
		node.GetData(service, value, 0);

		strBuffer += MkStr::LF;
		strBuffer += L"#define ";
		strBuffer += contents.GetString();
		strBuffer += (addDQM) ? L" L\"" : L" ";
		if (!prefix.Empty())
		{
			strBuffer += prefix;
		}
		strBuffer += value;
		if (addDQM)
		{
			strBuffer += L"\"";
		}
	}
};

bool __MK_CDC_GET_AND_SET_VALUE(MkStr& strBuffer, const MkDataNode& node, const MkHashStr& service, const MkHashStr& contents)
{
	switch (node.GetUnitType(service))
	{
	case ePDT_Bool: __TSI_GetAndSetValue<bool>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_Int: __TSI_GetAndSetValue<int>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_UnsignedInt: __TSI_GetAndSetValue<unsigned int>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_DInt: __TSI_GetAndSetValue<__int64>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_DUnsignedInt: __TSI_GetAndSetValue<unsigned __int64>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_Float: __TSI_GetAndSetValue<float>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	case ePDT_Int2: __TSI_GetAndSetValue<MkInt2>::Proceed(strBuffer, node, service, contents, false, L"MkInt2"); break;
	case ePDT_Vec2: __TSI_GetAndSetValue<MkVec2>::Proceed(strBuffer, node, service, contents, false, L"MkVec2"); break;
	case ePDT_Vec3: __TSI_GetAndSetValue<MkVec3>::Proceed(strBuffer, node, service, contents, false, L"MkVec3"); break;
	case ePDT_Str: __TSI_GetAndSetValue<MkStr>::Proceed(strBuffer, node, service, contents, true, MkStr::EMPTY); break;
	case ePDT_ByteArray: __TSI_GetAndSetValue<MkByteArray>::Proceed(strBuffer, node, service, contents, false, MkStr::EMPTY); break;
	default:
		return __MK_CDC_ERROR(contents.GetString() + L" �������� " + service.GetString() + L" ���� �� Ÿ���� ���ǵ��� �ʾ���");
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkCodeDefinitionConverter::Convert(const MkPathName& filePath, const MkStr& target)
{
	MkDataNode node;
	if (!node.Load(filePath))
		return false;

	// services
	MkArray<MkStr> services;
	node.GetData(KEY_Services, services);
	if (services.Empty())
		return __MK_CDC_ERROR(L"��ϵ� ���񽺰� ����");

	MK_INDEXING_LOOP(services, i)
	{
		MkStr& currSrv = services[i];
		unsigned int lastSize = currSrv.GetSize();
		if (lastSize == 0)
			return __MK_CDC_ERROR(MkStr(i) + L"��° ���� �̸��� ��� ����");

		currSrv.RemoveBlank();
		if (currSrv.GetSize() < lastSize)
			return __MK_CDC_ERROR(MkStr(i) + L"��° ���� �̸��� �����ڰ� ���ԵǾ� ����");
	}

	// export file path
	MkStr exportTo;
	node.GetData(KEY_ExportTo, exportTo, 0);
	if (exportTo.Empty())
		return __MK_CDC_ERROR(L"��� ���� ��� ������ �Ǿ� ���� ����");

	MkPathName exportPath = exportTo;
	MkPathName exportFilePath;

	if (!exportPath.IsAbsolutePath())
	{
		if (filePath.IsAbsolutePath())
		{
			exportFilePath = filePath.GetPath();
			exportFilePath += exportTo;
		}
		else
		{
			exportFilePath.ConvertToRootBasisAbsolutePath(exportTo);
		}
	}
	
	if (exportFilePath.IsDirectoryPath())
		return __MK_CDC_ERROR(L"��� ���ϸ��� �������� ���ϸ��� �ƴ�");

	// target
	if (!target.Empty())
	{
		if (node.IsValidKey(KEY_Target))
		{
			node.RemoveUnit(KEY_Target);
		}
		
		node.CreateUnit(KEY_Target, target);
	}

	if (!node.IsValidKey(KEY_Target))
		return __MK_CDC_ERROR(L"��� ���񽺸��� �����Ǿ� ���� ����");

	MkStr targetServiceName;
	node.GetData(KEY_Target, targetServiceName, 0);
	if (!services.Exist(MkArraySection(0), targetServiceName))
		return __MK_CDC_ERROR(L"���񽺸� ��Ͽ� " + targetServiceName + L"�� ���ԵǾ� ���� ����");

	MkHashStr targetServiceKey = targetServiceName;

	// contents definition
	MkArray<MkHashStr> contents;
	node.GetChildNodeList(contents);

	// text buffer
	MkStr strBuffer;
	strBuffer.Reserve(1024 * 1024 * 10); // text�� 10M�� ��������...
	strBuffer += L"#pragma once";
	strBuffer += MkStr::LF;
	strBuffer += MkStr::LF;
	strBuffer += L"//--------------------------------------------------------------------//" + MkStr::LF;
	strBuffer += L"// MkCodeDefinitionConverter" + MkStr::LF;
	strBuffer += L"//   - target service  : " + targetServiceName + MkStr::LF;

	if (MkSystemEnvironment::InstancePtr() != NULL)
	{
		strBuffer += L"//   - time stamp : " + MK_SYS_ENV.GetCurrentSystemDate() + L" (" + MK_SYS_ENV.GetCurrentSystemTime() + L")" + MkStr::LF;
	}

	strBuffer += L"//--------------------------------------------------------------------//";
	strBuffer += MkStr::LF;
	
	// contents�� ��ȸ
	MK_INDEXING_LOOP(contents, i)
	{
		const MkHashStr& currName = contents[i];
		const MkDataNode& currNode = *node.GetChildNode(currName);
		
		// ���ǵ� ���񽺰� �ִٸ�
		if (currNode.IsValidKey(targetServiceKey))
		{
			if (!__MK_CDC_GET_AND_SET_VALUE(strBuffer, currNode, targetServiceKey, currName))
				return false;
		}
		// ������ default �� ���
		else if (currNode.IsValidKey(KEY_Default))
		{
			if (!__MK_CDC_GET_AND_SET_VALUE(strBuffer, currNode, KEY_Default, currName))
				return false;
		}
		// default�� ������ ����(#ifdef/#ifndef Ȱ�� ����)
	}

	// ����
	return strBuffer.WriteToTextFile(exportFilePath, true, false);
}

//------------------------------------------------------------------------------------------------//
