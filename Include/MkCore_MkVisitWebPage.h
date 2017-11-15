#pragma once

//------------------------------------------------------------------------------------------------//
// 웹 페이지 호출
// ftp 업로드는 MkFtpInterface, 다운로드는 MkFileDownloader, 브라우저로 웹페이지 열기는 MkWebBrowser 참조
//------------------------------------------------------------------------------------------------//


class MkStr;

class MkVisitWebPage
{
public:

	// get 방식으로 호출
	static bool Get(const MkStr& url, MkStr& buffer);

	// post 방식으로 호출
	static bool Post(const MkStr& url, const MkStr& postData, MkStr& buffer);

	// address를 검사해 domain이면 ip로 변환
	static void CheckAddress(MkStr& address);
};
