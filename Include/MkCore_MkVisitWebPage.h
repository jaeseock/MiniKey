#pragma once

//------------------------------------------------------------------------------------------------//
// 웹 페이지 호출
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
