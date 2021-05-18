
#include "MkCore_MkDataNode.h"
#include "MkCore_MkExcelFileInterface.h"


typedef struct _SheetData
{
	MkStr time;
	int pay;
	MkStr shop;
}
SheetData;

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MkStr::SetUp();
	MkPathName::SetUp();

	MkPathName filePath;

	const wchar_t* cmdLine = ::GetCommandLine();
	int lineCount = 0;
	LPWSTR* cmdLines = ::CommandLineToArgvW(cmdLine, &lineCount);
	if (lineCount > 1)
	{
		filePath = MkStr(cmdLines[1]);
	}

	if (filePath.Empty())
	{
		if (!filePath.GetSingleFilePathFromDialog(MkStr(L"xls")))
			return 0;
	}

	MkExcelFileInterface excel;
	if (!excel.SetUp(filePath))
	{
		::MessageBox(NULL, L"엑셀 파일 열기 오류", L"부서운영비", MB_OK);
		return 0;
	}
	excel.SetActiveSheet(0);

	int targetYear = 2020;
	int startingRow = 20;

	MkStr timeColumn = L"A";
	MkStr payColumn = L"Q";
	MkStr cancelColumn = L"R";
	MkStr shopColumn = L"F";

	MkDataNode setting;
	MkPathName settingFilePath;
	settingFilePath.ConvertToModuleBasisAbsolutePath(L"setting.txt");
	if (settingFilePath.CheckAvailable() && setting.Load(settingFilePath))
	{
		setting.GetData(L"TargetYear", targetYear, 0);
		setting.GetData(L"StartingRow", startingRow, 0);
		setting.GetData(L"TimeColumn", timeColumn, 0);
		setting.GetData(L"PayColumn", payColumn, 0);
		setting.GetData(L"CancelColumn", cancelColumn, 0);
		setting.GetData(L"ShopColumn", shopColumn, 0);
	}
	else
	{
		setting.CreateUnit(L"TargetYear", targetYear);
		setting.CreateUnit(L"StartingRow", startingRow);
		setting.CreateUnit(L"TimeColumn", timeColumn);
		setting.CreateUnit(L"PayColumn", payColumn);
		setting.CreateUnit(L"CancelColumn", cancelColumn);
		setting.CreateUnit(L"ShopColumn", shopColumn);
		setting.SaveToText(settingFilePath);
	}

	MkStr currYear = targetYear;
	currYear += L".";

	MkArray<SheetData> table(512);

	int currRow = startingRow;
	int skipCount = 0;
	while (skipCount < 2)
	{
		MkStr timeBuf;
		MkStr row = currRow;
		MkStr timePos = timeColumn + row;

		if (excel.GetData(timePos, timeBuf) && (!timeBuf.Empty()))
		{
			MkStr cancelBuf;
			excel.GetData(cancelColumn + row, cancelBuf);
			if (cancelBuf.ToInteger() == 0)
			{
				MkStr payBuf, shopBuf;
				excel.GetData(payColumn + row, payBuf);
				excel.GetData(shopColumn + row, shopBuf);

				SheetData& sd = table.PushBack();
				{
					sd.time = currYear;
					MkStr tmp;
					timeBuf.GetSubStr(MkArraySection(0, 5), tmp);
					sd.time += tmp;
				}
				sd.pay = payBuf.ToInteger();
				sd.shop = shopBuf;
			}

			skipCount = 0;
			++currRow;
		}
		else
		{
			++skipCount;
			currRow += 2;
		}
	}

	excel.Clear();

	if (table.GetSize() == 0)
		return 0;

	table.ReverseOrder();
	if (!excel.SetUp())
	{
		::MessageBox(NULL, L"엑셀 파일 생성 오류", L"부서운영비", MB_OK);
		return 0;
	}
	excel.SetActiveSheet(0);

	MK_INDEXING_LOOP(table, i)
	{
		const SheetData& sd = table[i];
		excel.PutData(i, 0, sd.time);
		excel.SetBorder(i, 0, 1);
		excel.PutData(i, 1, sd.pay);
		excel.SetBorder(i, 1, 1);
		excel.PutData(i, 2, sd.shop);
		excel.SetBorder(i, 2, 1);
	}

	excel.AutoFit(0);
	excel.AutoFit(1);
	excel.AutoFit(2);

	MkPathName lastFileName = filePath.GetFileName(false);
	lastFileName += L"_변환";
	filePath.ChangeFileName(lastFileName);
	filePath.ChangeFileExtension(L"xlsx");
	MkStr msg = filePath.GetFileName();

	if (excel.SaveAs(filePath))
	{	
		msg += L" 파일 저장 성공";
	}
	else
	{
		msg += L" 파일 저장 오류";
	}
	::MessageBox(NULL, msg.GetPtr(), L"부서운영비", MB_OK);

	return 0;
}

