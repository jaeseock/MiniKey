#pragma once

//------------------------------------------------------------------------------------------------//
// ��ŷ�� ���� ���� ������
//------------------------------------------------------------------------------------------------//

// argument�� ���� launcher���� ���޵Ǵ� url key
#define MKDEF_PATCH_DOWNLOAD_URL_KEY L"#PURL" // Patch URL. std::string

class MkStr;

class MkPatchStarter
{
public:

	static bool StartLauncher(const MkStr& url, const wchar_t* arg = NULL, bool updateLauncher = true);
};

//------------------------------------------------------------------------------------------------//
