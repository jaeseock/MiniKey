#pragma once

#include "MkCore_MkHashStr.h"

//------------------------------------------------------------------------------------------------//
// application version
//------------------------------------------------------------------------------------------------//

#define ADEF_CORE_MAJOR_VERSION 1
#define ADEF_CORE_MINOR_VERSION 0
#define ADEF_CORE_BUILD_VERSION 0
#define ADEF_CORE_REVISION_VERSION 0


//------------------------------------------------------------------------------------------------//
// game page
//------------------------------------------------------------------------------------------------//

class GamePage
{
public:

	//------------------------------------------------------------------------------------------------//

	class Root
	{
	public:
		static const MkHashStr Name;
	};

	//------------------------------------------------------------------------------------------------//

	class AppStart
	{
	public:
		static const MkHashStr Name;

		class Condition
		{
		public:
			static const MkHashStr Next;
		};
	};

	class AppIntro
	{
	public:
		static const MkHashStr Name;
		
		class Condition
		{
		public:
			static const MkHashStr Next;
		};
	};

	class AppLobby
	{
	public:
		static const MkHashStr Name;
		
		class Condition
		{
		public:
			static const MkHashStr Battle;
		};
	};

	class AppBriefingRoom
	{
	public:
		static const MkHashStr Name;
		
		class Condition
		{
		public:
			static const MkHashStr Next;
		};
	};

	class AppBattleZone
	{
	public:
		static const MkHashStr Name;
		
		class Condition
		{
		public:
			static const MkHashStr Next;
		};
	};

	//------------------------------------------------------------------------------------------------//
};


//------------------------------------------------------------------------------------------------//
// global definition
//------------------------------------------------------------------------------------------------//

class GlobalDef
{
public:

	static const MkHashStr FinalDrawStepName;
};

