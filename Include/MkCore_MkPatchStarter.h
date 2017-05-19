#pragma once

//------------------------------------------------------------------------------------------------//
// 패킹된 개별 파일 관리자
//------------------------------------------------------------------------------------------------//

// argument를 통해 launcher에게 전달되는 url key
#define MKDEF_PATCH_DOWNLOAD_URL_KEY "#PURL" // Patch URL. std::string

class MkStr;

class MkPatchStarter
{
public:

	static bool StartLauncher(const MkStr& url, const char* arg = NULL);
};

//------------------------------------------------------------------------------------------------//
