#pragma once

//------------------------------------------------------------------------------------------------//
// ���α׷� OS ���(install postprocess) �� ����(uninstall)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataNode.h"


class MkApplicationRegister
{
public:
	//------------------------------------------------------------------------------------------------//
	// service data ��±�
	// ���� ������ ������ client�� ���� ���¸� ����
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
	// ������Ʈ��, ���� ���
	// GenerateServiceData::SaveService()�� ����� service data ���
	// ex> MkApplicationRegister::UpdateService(L"registerinfo.mmd");
	static bool UpdateService(const MkPathName& targetServicesFilePath);

	//------------------------------------------------------------------------------------------------//
	// ������Ʈ��, ����, client ���� ����
	// GenerateServiceData::SaveService()�� ����� service data ���
	// ex> MkApplicationRegister::UninstallService(L"registerinfo.mmd", false);
	static bool UninstallService(const MkPathName& targetServicesFilePath, bool requireConfirm = true);

	//------------------------------------------------------------------------------------------------//
};
