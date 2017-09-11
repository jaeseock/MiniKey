#pragma once

//------------------------------------------------------------------------------------------------//
// 프로그램 OS 등록(install postprocess) 및 삭제(uninstall)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataNode.h"


class MkApplicationRegister
{
public:
	//------------------------------------------------------------------------------------------------//
	// service data 출력기
	// 서비스 정보와 지정된 client의 파일 상태를 저장
	// ex>
	//	GenerateServiceData serviceData;
	//	MkArray<MkHashStr> services;
	//	if (!serviceData.LoadServices(L"services.mmd", services))
	//	{
	//		...
	//		serviceData.SaveService(services[0], L"registerinfo.mmd");
	//	}

	class GenerateServiceData
	{
	public:
		bool LoadServices(const MkPathName& servicesFilePath, MkArray<MkHashStr>& services);
		bool SaveService(const MkHashStr& targetService, const MkPathName& saveFileName) const;

	protected:
		static bool _CreateDefaultService(MkDataNode& dataNode);
		static bool _CheckServiceEnable(const MkDataNode& serviceNode, MkStr& errorMsg);
		static bool _CheckPathEnable(const MkDataNode& targetNode, const MkHashStr& fileKey, const MkPathName& clientPath, MkStr& errorMsg);
		static bool _CheckShortcutEnable(const MkDataNode& targetNode, const MkPathName& clientPath, MkStr& errorMsg);

	protected:
		MkDataNode m_Services;
	};

	//------------------------------------------------------------------------------------------------//
	// 레지스트리, 숏컷 등록
	// GenerateServiceData::SaveService()로 저장된 service data 기반
	// ex> MkApplicationRegister::UpdateService(L"registerinfo.mmd");
	static bool UpdateService(const MkPathName& targetServicesFilePath);

	//------------------------------------------------------------------------------------------------//
	// 레지스트리, 숏컷, client 파일 삭제
	// GenerateServiceData::SaveService()로 저장된 service data 기반
	// ex> MkApplicationRegister::UninstallService(L"registerinfo.mmd", false);
	static bool UninstallService(const MkPathName& targetServicesFilePath, bool requireConfirm = true);

	//------------------------------------------------------------------------------------------------//
};
