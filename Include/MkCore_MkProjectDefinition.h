#ifndef __MINIKEY_CORE_MKPROJECTDEFINITION_H__
#define __MINIKEY_CORE_MKPROJECTDEFINITION_H__

//------------------------------------------------------------------------------------------------//
// library version
//------------------------------------------------------------------------------------------------//

#define MKDEF_CORE_MAJOR_VERSION 1
#define MKDEF_CORE_MINOR_VERSION 0
#define MKDEF_CORE_BUILD_VERSION 0
#define MKDEF_CORE_REVISION_VERSION 0

//------------------------------------------------------------------------------------------------//
// dev environment
//------------------------------------------------------------------------------------------------//

#define MKDEF_CORE_DEV_ENV_OVER_VISTA TRUE

//------------------------------------------------------------------------------------------------//
// MkArray
//------------------------------------------------------------------------------------------------//

// 기본 블록 단위 확장시 크기
#define MKDEF_DEFAULT_BLOCK_EXPANSION_SIZE_IN_ARRAY 8

//------------------------------------------------------------------------------------------------//
// MkCircularDeque
//------------------------------------------------------------------------------------------------//

// 별도 선언 없을때의 기본 크기
#define MKDEF_DEFAULT_SIZE_IN_CIRCULAR_DEQUE 64

//------------------------------------------------------------------------------------------------//
// MkEventQueuePattern
//------------------------------------------------------------------------------------------------//

// 별도 선언 없을때의 기본 크기
#define MKDEF_DEFAULT_SIZE_IN_EVENT_QUEUE 8

//------------------------------------------------------------------------------------------------//
// MkLogManager
//------------------------------------------------------------------------------------------------//

#define MKDEF_CRASH_LOG_FILE_PREFIX L"_MkCrash_"
#define MKDEF_SENDING_PATCH_FILE_NAME L"_MkSendLog.bat"

//------------------------------------------------------------------------------------------------//
// MkSlangFilter
//------------------------------------------------------------------------------------------------//

#define MKDEF_SLANG_REPLACEMENT_TAG L'*'

//------------------------------------------------------------------------------------------------//
// MkTimeManager
//------------------------------------------------------------------------------------------------//

#define MKDEF_FRAME_COUNTS_PER_SEC 60 // 초당 루핑 수
#define MKDEF_FRAME_TIME_FACTOR 0.06f // MKDEF_FRAME_COUNTS_PER_SEC / 1000.f
#define MKDEF_FRAME_ELAPSED_TIME 0.01666666f // 1.f / MKDEF_FRAME_COUNTS_PER_SEC
#define MKDEF_FRAME_ELAPSED_IN_MICROSEC_FACTOR 0.006f // MKDEF_FRAME_COUNTS_PER_SEC / 1000000.f * 100

//------------------------------------------------------------------------------------------------//
// DEBUG
//------------------------------------------------------------------------------------------------//

// input event 정보 출력
#define MKDEF_SHOW_INPUT_EVENT FALSE

// background loading 종료 정보 출력
#define MKDEF_SHOW_BGLOADING_COMPLETE TRUE

// shared pointer reference 정보 출력
#define MKDEF_SHOW_SHARED_POINTER_REF_INFO FALSE

// 파일 패킹시 정보 출력
#define MKDEF_SHOW_FILE_PACKING_INFO TRUE

//------------------------------------------------------------------------------------------------//

#endif

