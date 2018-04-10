
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

const static unsigned int CARD_TYPE_COUNT = 3;
const static unsigned int CARD_COLOR_COUNT = 3;
const static unsigned int FULL_CARD_COUNT = CARD_TYPE_COUNT * CARD_COLOR_COUNT;
const static unsigned int HANDS_COUNT = 3; // <= FULL_CARD_COUNT


// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		// error
		if ((FULL_CARD_COUNT == 0) || (HANDS_COUNT == 0) || (HANDS_COUNT > FULL_CARD_COUNT))
			return false;

		MK_DEV_PANEL.MsgToLog(L"FULL_CARD_COUNT : " + MkStr(FULL_CARD_COUNT));
		MK_DEV_PANEL.MsgToLog(L"HANDS_COUNT : " + MkStr(HANDS_COUNT));

		// full card
		MkArray<unsigned int> fullCards(FULL_CARD_COUNT);
		for (unsigned int ft = 0; ft<CARD_TYPE_COUNT; ++ft)
		{
			for (unsigned int rt = 0; rt<CARD_COLOR_COUNT; ++rt)
			{
				fullCards.PushBack(_GetCV(ft, rt));
			}
		}

		// full combination
		unsigned int totalCombinationCount = _GetTotalCombinationCount(FULL_CARD_COUNT, HANDS_COUNT);
		MK_DEV_PANEL.MsgToLog(L"totalCombinationCount : " + MkStr(totalCombinationCount));

		MkArray<unsigned int> fullCombinations(totalCombinationCount * HANDS_COUNT);
		MkArray<unsigned int> handStackBuffer(HANDS_COUNT);
		_FillCombination(fullCards, 0, 0, handStackBuffer, fullCombinations);

		// check made
		for (unsigned int i=0; i<totalCombinationCount; i+=HANDS_COUNT)
		{
			MkArray<unsigned int> currCombination;
			fullCombinations.GetSubArray(MkArraySection(i, HANDS_COUNT), currCombination);
		}

		//unsigned int cv = _GetCV(2, 4);
		//unsigned int ft = _GetFT(cv);
		//unsigned int rt = _GetRT(cv);

		return true;
	}

	virtual void Update(void)
	{
		
	}

	virtual void Clear(void)
	{
		
	}

	TestFramework() : MkBaseFramework()
	{
	}

	virtual ~TestFramework()
	{
	}

protected:

	inline unsigned int _GetCV(unsigned int ft, unsigned int rt) const { return ((ft << 16) | rt); } // card value
	inline unsigned int _GetFT(unsigned int cv) const { return (cv >> 16); } // front type
	inline unsigned int _GetRT(unsigned int cv) const { return (cv & 0x0000ffff); } // rear type

	unsigned int _Factorial(unsigned int value) const { return (value == 1) ? 1 : (value * _Factorial(value - 1)); }
	unsigned int _Factorial(unsigned int value, unsigned int count) const { return ((value == 1) || (count == 1)) ? value : (value * _Factorial(value - 1, count - 1)); }

	unsigned int _GetTotalCombinationCount(unsigned int cards, unsigned int hand) const { return (_Factorial(cards, hand) / _Factorial(hand)); }

	void _FillCombination(const MkArray<unsigned int>& fullCards, unsigned int handIndex, unsigned int minStart, MkArray<unsigned int>& handStackBuffer, MkArray<unsigned int>& resultBuffer)
	{
		unsigned int startCard = (handIndex == 0) ? 0 : GetMax<unsigned int>(handIndex, minStart);
		unsigned int endCard = FULL_CARD_COUNT - HANDS_COUNT + handIndex;
		unsigned int nextHand = handIndex + 1;

		for (unsigned int i = startCard; i <= endCard; ++i)
		{
			handStackBuffer.PushBack(fullCards[i]);

			if (nextHand < HANDS_COUNT)
			{
				_FillCombination(fullCards, nextHand, i + 1, handStackBuffer, resultBuffer);
			}
			else
			{
				resultBuffer += handStackBuffer;

				/*
				MkStr msg;
				msg.Reserve(512);
				msg += resultBuffer.GetSize() / HANDS_COUNT;
				msg += L") ";

				MK_INDEXING_LOOP(handStackBuffer, j)
				{
					if (j > 0)
					{
						msg += L"-";
					}
					msg += handStackBuffer[j];
				}
				MK_DEV_PANEL.MsgToLog(msg);
				*/
			}

			handStackBuffer.PopBack();
		}
	}

protected:
};


class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"WhiteBoard", L"..\\ResRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

