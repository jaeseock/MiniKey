// InsertDownloadInfos.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTailData.h"


int main()
{
    // mk 초기화
    MkStr::SetUp();
    MkPathName::SetUp();

    MkPathName tmpFilePath;
    tmpFilePath.ConvertToRootBasisAbsolutePath(L"_tmp.mmd");

    MkDataNode settingNode;
    if (!settingNode.Load(L"InsertDownloadInfos.txt"))
    {
        std::cout << "[InsertDownloadInfos.txt] 파일이 없습니다.\n";
        return -1;
    }

    MkStr codesignFile;
    settingNode.GetData(L"CodeSign", codesignFile, 0);

    MkPathName codesignPath;
    if (!codesignFile.Empty())
    {
        codesignPath.ConvertToRootBasisAbsolutePath(codesignFile);
        if (codesignPath.CheckAvailable())
        {
            std::wcout << L"CodeSign : " << codesignFile.GetPtr() << L"\n";
        }
        else
        {
            codesignPath.Clear();
        }
    }

    // src list
    MkArray<MkHashStr> srcExeList;
    settingNode.GetChildNodeList(srcExeList);

    MK_INDEXING_LOOP(srcExeList, i)
    {
        MkPathName srcFullPath;
        srcFullPath.ConvertToRootBasisAbsolutePath(srcExeList[i].GetString());
        if (!srcFullPath.CheckAvailable())
        {
            std::wcout << L"[ERROR] " << srcExeList[i].GetString().GetPtr() << L" 원본 파일은 존재하지 않음.\n";
            continue;
        }
            
        const MkDataNode& currSrc = *settingNode.GetChildNode(srcExeList[i]);

        MkArray<MkHashStr> targetExeList;
        currSrc.GetChildNodeList(targetExeList);
        if (targetExeList.Empty())
        {
            std::wcout << L"[ERROR] " << srcExeList[i].GetString().GetPtr() << L" 원본 파일의 출력 정보가 없음.\n";
            continue;
        }

        MK_INDEXING_LOOP(targetExeList, j)
        {
            const MkDataNode& currTarget = *currSrc.GetChildNode(targetExeList[j]);
            if (!currTarget.SaveToBinary(tmpFilePath))
            {
                std::wcout << L"[ERROR] " << targetExeList[j].GetString().GetPtr() << L" 대상의 출력 정보 파일 저장 실패.\n";
                continue;
            }

            if (MkTailData::AttachData(srcFullPath, tmpFilePath, targetExeList[j].GetString()))
            {
                std::wcout << L"[OK] " << targetExeList[j].GetString().GetPtr() << L"\n";

                if (!codesignPath.Empty())
                {
                    MkPathName targetFullPath;
                    targetFullPath.ConvertToRootBasisAbsolutePath(targetExeList[j].GetString());
                    if (codesignPath.ExcuteConsoleProcess(L"CodeSign", L"\"" + targetFullPath + L"\""))
                    {
                        std::wcout << L"   " << L"CodeSign OK.\n";
                    }
                    else
                    {
                        std::wcout << L"   " << L"CodeSign FAILED.\n";
                    }
                }
            }
            else
            {
                std::wcout << L"[ERROR] " << targetExeList[j].GetString().GetPtr() << L" 대상의 출력 정보 연결 실패.\n";
            }
        }
    }

    tmpFilePath.DeleteCurrentFile();

    std::cout << "Complete!\n";
    std::cin.get();
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
