@echo off

set timestamp=http://timestamp.verisign.com/scripts/timestamp.dll
set passward=play0072
set signexefilename=signtool.exe
set signfilename=cert.pfx

rem �Ķ���Ͱ� ���°�� - ������� �ڵ弭���� ������ �巡�׾� ����ؼ� ��ġ���Ͽ� �ִ´�.
IF NOT EXIST %1  ( goto error_1 )

echo %1

rem ���� ��� �� ����
set signexefilepath=%~dp0signtool.exe
set signfilepath=%~dp0cert.pfx

rem �ڵ� ���� ���������� ���°��
IF NOT EXIST %signexefilepath% ( goto error_2 )

rem �ڵ� ���� ������ ���°��
IF NOT EXIST %signfilepath%  ( goto error_3 )

echo %~n1 ������ �ڵ� ������ �����մϴ�.

echo ���� ���� : %signexefilename%
echo ���� ���� : %signfilename%
echo ������ ���� : %~nx1

rem ���� ���� 
Call %signexefilepath% sign /a /f %signfilepath% /p %passward% %1

exit

rem �Ķ���Ͱ� ���°��
:error_1
echo �ڵ� ������ ������ ��ġ���Ͽ� �巡���ؼ� ��ġ������ ��������ּ���.
exit

rem �ڵ弭�� ���������� ���°��
:error_2
echo signtool.exe ������ ���� ���� ��ο� �־��ּ���.
exit

rem cert.pfx ������ ���°��
:error_3
echo cert.pfx ������ ���� ���� ��ο� �־��ּ���.
exit

