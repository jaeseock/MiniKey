#pragma once

//------------------------------------------------------------------------------------------------//
// �� ������ ȣ��
// ftp ���ε�� MkFtpInterface, �ٿ�ε�� MkFileDownloader, �������� �������� ����� MkWebBrowser ����
//------------------------------------------------------------------------------------------------//


class MkStr;

class MkVisitWebPage
{
public:

	// get ������� ȣ��
	static bool Get(const MkStr& url, MkStr& buffer);

	// post ������� ȣ��
	static bool Post(const MkStr& url, const MkStr& postData, MkStr& buffer);

	// address�� �˻��� domain�̸� ip�� ��ȯ
	static void CheckAddress(MkStr& address);
};
