@echo off

set timestamp=http://timestamp.verisign.com/scripts/timestamp.dll
set passward=play0072
set signexefilename=signtool.exe
set signfilename=cert.pfx

rem 파라메터가 없는경우 - 사용방법은 코드서명할 파일을 드래그앤 드랍해서 배치파일에 넣는다.
IF NOT EXIST %1  ( goto error_1 )

echo %1

rem 파일 경로 및 설정
set signexefilepath=%~dp0signtool.exe
set signfilepath=%~dp0cert.pfx

rem 코드 서명 실행파일이 없는경우
IF NOT EXIST %signexefilepath% ( goto error_2 )

rem 코드 서명 파일이 없는경우
IF NOT EXIST %signfilepath%  ( goto error_3 )

echo %~n1 파일의 코드 서명을 시작합니다.

echo 실행 파일 : %signexefilename%
echo 서명 파일 : %signfilename%
echo 서명할 파일 : %~nx1

rem 서명 시작 
Call %signexefilepath% sign /a /f %signfilepath% /p %passward% %1

exit

rem 파라메터가 없는경우
:error_1
echo 코드 서명할 파일을 배치파일에 드래그해서 배치파일을 실행시켜주세요.
exit

rem 코드서명 실행파일이 없는경우
:error_2
echo signtool.exe 파일을 같은 파일 경로에 넣어주세요.
exit

rem cert.pfx 파일이 없는경우
:error_3
echo cert.pfx 파일을 같은 파일 경로에 넣어주세요.
exit

