
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
		return __MK_CDC_ERROR(contents.GetString() + L" 콘텐츠의 " + service.GetString() + L" 서비스 값 타입이 정의되지 않았음");
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
		return __MK_CDC_ERROR(L"등록된 서비스가 없음");

	MK_INDEXING_LOOP(services, i)
	{
		MkStr& currSrv = services[i];
		unsigned int lastSize = currSrv.GetSize();
		if (lastSize == 0)
			return __MK_CDC_ERROR(MkStr(i) + L"번째 서비스 이름이 비어 있음");

		currSrv.RemoveBlank();
		if (currSrv.GetSize() < lastSize)
			return __MK_CDC_ERROR(MkStr(i) + L"번째 서비스 이름에 공문자가 포함되어 있음");
	}

	// export file path
	MkStr exportTo;
	node.GetData(KEY_ExportTo, exportTo, 0);
	if (exportTo.Empty())
		return __MK_CDC_ERROR(L"출력 파일 경로 지정이 되어 있지 않음");

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
		return __MK_CDC_ERROR(L"출력 파일명이 정상적인 파일명이 아님");

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
		return __MK_CDC_ERROR(L"대상 서비스명이 지정되어 있지 않음");

	MkStr targetServiceName;
	node.GetData(KEY_Target, targetServiceName, 0);
	if (!services.Exist(MkArraySection(0), targetServiceName))
		return __MK_CDC_ERROR(L"서비스명 목록에 " + targetServiceName + L"은 포함되어 있지 않음");

	MkHashStr targetServiceKey = targetServiceName;

	// contents definition
	MkArray<MkHashStr> contents;
	node.GetChildNodeList(contents);

	// text buffer
	MkStr strBuffer;
	strBuffer.Reserve(1024 * 1024 * 10); // text가 10M를 넘을리가...
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
	
	// contents별 순회
	MK_INDEXING_LOOP(contents, i)
	{
		const MkHashStr& currName = contents[i];
		const MkDataNode& currNode = *node.GetChildNode(currName);
		
		// 정의된 서비스가 있다면
		if (currNode.IsValidKey(targetServiceKey))
		{
			if (!__MK_CDC_GET_AND_SET_VALUE(strBuffer, currNode, targetServiceKey, currName))
				return false;
		}
		// 없으면 default 값 사용
		else if (currNode.IsValidKey(KEY_Default))
		{
			if (!__MK_CDC_GET_AND_SET_VALUE(strBuffer, currNode, KEY_Default, currName))
				return false;
		}
		// default도 없으면 무시(#ifdef/#ifndef 활용 가능)
	}

	// 저장
	return strBuffer.WriteToTextFile(exportFilePath, true, false);
}

//------------------------------------------------------------------------------------------------//
