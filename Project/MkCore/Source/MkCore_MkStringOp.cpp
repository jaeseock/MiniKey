
#include <io.h>
#include <fstream>
#include <algorithm>
#include <ShlObj.h>

#include "MkCore_MkStringOp.h"

//-------------------------------------------------------------//

#define MKDEF_STR_TAP "\t"
#define MKDEF_STR_LINEFEED "\n"
#define MKDEF_STR_RETURN "\r"
#define MKDEF_STR_SPACE " "
#define MKDEF_CHAR_TAP '\t'
#define MKDEF_CHAR_LINEFEED '\n'
#define MKDEF_CHAR_RETURN '\r'
#define MKDEF_CHAR_SPACE ' '

//-------------------------------------------------------------//

int MkStringOp::CountKeyword(const std::string& source, const std::string& keyword)
{
	int count = 0;
	size_t findFrom = 0;
	size_t sizeOfKeyword = keyword.size();
	while (true)
	{
		size_t pos = source.find(keyword, findFrom);
		if (pos == std::string::npos)
		{
			return count;
		}
		else
		{
			++count;
			findFrom = pos + sizeOfKeyword;
		}
	}
}

bool MkStringOp::CheckPrefix(const std::string& source, const std::string& prefix)
{
	return (GetFirstKeywordPosition(source, prefix) == 0);
}

bool MkStringOp::CheckPostfix(const std::string& source, const std::string& postfix)
{
	int foundPos = GetLastKeywordPosition(source, postfix);
	if (foundPos == -1)
		return false;
	return (foundPos == (int)(source.size() - postfix.size()));
}

int MkStringOp::GetFirstKeywordPosition(const std::string& source, const std::string& keyword)
{
	size_t position = source.find(keyword);
	return (position == std::string::npos) ? -1 : (int)position;
}

int MkStringOp::GetLastKeywordPosition(const std::string& source, const std::string& keyword)
{
	size_t position = source.rfind(keyword);
	return (position == std::string::npos) ? -1 : (int)position;
}

int MkStringOp::GetFirstValidPosition(const std::string& source, int startPosition)
{
	if (source.empty())
		return -1;

	size_t size = source.size();
	size_t position = (size_t)startPosition;
	while (position < size)
	{
		char character = source[position];
		if ((character == MKDEF_CHAR_TAP) ||
			(character == MKDEF_CHAR_LINEFEED) ||
			(character == MKDEF_CHAR_RETURN) ||
			(character == MKDEF_CHAR_SPACE))
		{
			++position;
		}
		else
			return (int)position;
	}
	return -1;
}

int MkStringOp::GetLastValidPosition(const std::string& source, int startPosition)
{
	if (source.empty())
		return -1;

	size_t endPos = (size_t)startPosition;
	size_t position = source.size() - 1;
	while (position >= endPos)
	{
		char character = source[position];
		if ((character == MKDEF_CHAR_TAP) ||
			(character == MKDEF_CHAR_LINEFEED) ||
			(character == MKDEF_CHAR_RETURN) ||
			(character == MKDEF_CHAR_SPACE))
		{
			--position;
		}
		else
			return (int)position;
	}
	return -1;
}

//-------------------------------------------------------------//

int MkStringOp::ReplaceKeyword(std::string& source, const std::string& keyword_a, const std::string& keyword_b, int startPosition)
{
	if (source.empty())
		return 0;

	int count = 0;
	size_t a_size = keyword_a.size();
	size_t b_size = keyword_b.size();
	size_t nextPosition = (size_t)startPosition;
	while (true)
	{
		size_t keywordBeginPos = source.find(keyword_a, nextPosition);
		if (keywordBeginPos != std::string::npos)
		{
			source.erase(keywordBeginPos, a_size);
			source.insert(keywordBeginPos, keyword_b);
			nextPosition = keywordBeginPos + b_size;
			++count;
		}
		else
			break;
	}
	return count;
}

void MkStringOp::RemoveKeyword(std::string& source, const std::string& keyword, int startPosition)
{
	if (source.empty())
		return;

	size_t sizeOfKeyword = keyword.size();
	size_t nextPosition = (size_t)startPosition;
	while (true)
	{
		size_t keywordBeginPos = source.find(keyword, nextPosition);
		if (keywordBeginPos != std::string::npos)
		{
			source.erase(keywordBeginPos, sizeOfKeyword);
			nextPosition = keywordBeginPos;
		}
		else
			break;
	}
}

void MkStringOp::RemoveBlank(std::string& source, int startPosition)
{
	RemoveKeyword(source, MKDEF_STR_TAP, startPosition);
	RemoveKeyword(source, MKDEF_STR_LINEFEED, startPosition);
	RemoveKeyword(source, MKDEF_STR_RETURN, startPosition);
	RemoveKeyword(source, MKDEF_STR_SPACE, startPosition);
}


void MkStringOp::RemoveFrontSideBlank(std::string& source)
{
	if (source.empty())
		return;

	int removeSize = GetFirstValidPosition(source);
	if (removeSize > 0)
	{
		source.erase(0, removeSize);
	}
	else if (removeSize == -1)
	{
		source.clear(); // 유효문자 없음
	}
}

void MkStringOp::RemoveRearSideBlank(std::string& source)
{
	if (source.empty())
		return;

	int removePos = GetLastValidPosition(source);
	if (removePos == (source.size() - 1))
		return;

	if (removePos >= 0)
	{
		source.erase(removePos + 1);
	}
	else
	{
		source.clear(); // 유효문자 없음
	}
}

//-------------------------------------------------------------//

bool MkStringOp::GetFirstBlock(std::string& source, const std::string& endKeyword, std::string& buffer, int startPosition, bool cut)
{
	size_t startPos = (size_t)startPosition;
	if (startPos >= source.size())
		return false;

	size_t endPos = source.find(endKeyword, startPos);
	if (endPos == std::string::npos)
		return false;

	size_t blockSize = endPos - startPos;
	buffer = source.substr(startPos, blockSize);

	if (cut)
	{
		source.erase(startPos, blockSize + endKeyword.size());
	}
	return true;
}

int MkStringOp::GetFirstBlock
(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition, bool cut)
{
	size_t startPos = (size_t)startPosition;
	if (startPos >= source.size())
		return -1;

	size_t targetPosBegin = source.find(beginKeyword, startPos);
	if (targetPosBegin != std::string::npos)
	{
		size_t strBeginPos = targetPosBegin + beginKeyword.size();
		size_t targetPosEnd = source.find(endKeyword, strBeginPos);
		if (targetPosEnd != std::string::npos)
		{
			buffer = source.substr(strBeginPos, targetPosEnd - strBeginPos);

			if (cut)
			{
				source.erase(targetPosBegin, targetPosEnd - targetPosBegin + endKeyword.size());
				return (int)targetPosBegin;
			}
			else
			{
				return (int)strBeginPos;
			}
		}
	}
	return -1;
}

int MkStringOp::GetFirstStackBlock
(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition, bool cut)
{
	size_t startPos = (size_t)startPosition;
	if (startPos >= source.size())
		return -1;

	size_t targetPosBegin = source.find(beginKeyword, startPos);
	if (targetPosBegin != std::string::npos)
	{
		// 내용 시작점
		size_t strBeginPos = targetPosBegin + beginKeyword.size();
		size_t seekPos = strBeginPos;
		size_t endKeywordSize = endKeyword.size();

		while (true)
		{
			// 종료점 탐색
			seekPos = source.find(endKeyword, seekPos);
			if (seekPos != std::string::npos)
			{
				// 임시로 블록 내용 복사
				std::string subsetBuffer = source.substr(strBeginPos, seekPos - strBeginPos);
				
				// 블록 안의 begin/end keyword 수가 같아야 동일 깊이
				if (CountKeyword(subsetBuffer, beginKeyword) == CountKeyword(subsetBuffer, endKeyword))
				{
					buffer = subsetBuffer;

					if (cut)
					{
						source.erase(targetPosBegin, seekPos - targetPosBegin + endKeywordSize);
						return (int)targetPosBegin;
					}
					else
					{
						return (int)strBeginPos;
					}
				}
				else
				{
					seekPos += endKeywordSize; // 이후부터 재검색
				}
			}
			else
				break; // 종료 키워드가 없음
		}
	}
	return -1;
}

bool MkStringOp::ReplaceBlock
(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, const std::string& replaceBlock, int startPosition)
{
	std::string tmpBuf;
	int beginPos = GetFirstBlock(source, beginKeyword, endKeyword, tmpBuf, startPosition, true);
	if (beginPos >= 0)
	{
		source.insert(beginPos, replaceBlock);
		return true;
	}
	return false;
}

void MkStringOp::RemoveAllComment(std::string& source)
{
	while(ReplaceBlock(source, "//", "\n", "\n", 0)) {}
	while(ReplaceBlock(source, "/*", "*/", "", 0)) {}
}

//-------------------------------------------------------------//

int MkStringOp::Tokenize(const std::string& source, std::vector<std::string>& tokens, const std::string& separator)
{
	int sepCnt = CountKeyword(source, separator);
	tokens.reserve(tokens.size() + sepCnt + 1);

	size_t size = source.size();
	size_t start = source.find_first_not_of(separator);
	size_t stop;
	while ((start >= 0) && (start < size))
	{
		stop = source.find_first_of(separator, start);
		if ((stop < 0) || (stop > size))
		{
			stop = size;
		}
		tokens.push_back(source.substr(start, stop - start));
		start = source.find_first_not_of(separator, stop + 1);
	}
	return (int)tokens.size();
}

int MkStringOp::Tokenize(const std::string& source, std::vector<std::string>& tokens)
{
	std::string tmpBuf = source;

	// space를 구분자로 삼아 다른 공문자 치환
	ReplaceKeyword(tmpBuf, MKDEF_STR_TAP, MKDEF_STR_SPACE); // tab -> space
	ReplaceKeyword(tmpBuf, MKDEF_STR_LINEFEED, MKDEF_STR_SPACE); // line feed -> space
	ReplaceKeyword(tmpBuf, MKDEF_STR_RETURN, MKDEF_STR_SPACE); // return -> space

	return Tokenize(source, tokens, MKDEF_STR_SPACE);
}

//-------------------------------------------------------------//

void MkStringOp::PopFront(std::string& source, int count)
{
	if ((source.empty()) || (count <= 0))
		return;

	if (source.size() <= (size_t)count)
	{
		source.clear();
	}
	else
	{
		source.erase(0, count);
	}
}

void MkStringOp::BackSpace(std::string& source, int count)
{
	if (source.empty())
		return;

	size_t cnt = (size_t)count;
	size_t size = source.size();
	if (size <= cnt)
	{
		source.clear();
	}
	else
	{
		source.erase(size - cnt);
	}
}

void MkStringOp::ToUpper(std::string& source)
{
	if (source.empty())
		return;

	std::transform(source.begin(), source.end(), source.begin(), toupper);
}

void MkStringOp::ToLower(std::string& source)
{
	if (source.empty())
		return;

	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

bool MkStringOp::ToBool(std::string& source)
{
	std::string tmpBuf = source;
	RemoveBlank(tmpBuf);
	ToLower(tmpBuf);
	return ((tmpBuf == "true") || (tmpBuf == "yes"));
}

int MkStringOp::ToInteger(std::string& source)
{
	std::string tmpBuf = source;
	RemoveBlank(tmpBuf);
	return atoi(tmpBuf.c_str());
}

float MkStringOp::ToFloat(std::string& source)
{
	std::string tmpBuf = source;
	RemoveBlank(tmpBuf);
	size_t size = tmpBuf.size();
	bool percentage = false;
	if ((size > 1) && (tmpBuf[size - 1] == '%'))
	{
		BackSpace(tmpBuf, 1); // delete '%'
		percentage = true;
	}
	float result = (float)atof(tmpBuf.c_str());
	if (percentage)
	{
		result *= 0.01f; // 100% -> 1.f
	}
	return result;
}

std::string MkStringOp::ToString(bool source)
{
	return (source) ? "Yes" : "No";
}

std::string MkStringOp::ToString(int source)
{
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	_itoa_s(source, buffer, 256, 10);
	return std::string(buffer);
}

std::string MkStringOp::ToString(float source)
{
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	sprintf_s(buffer, 256, "%-5.3f", source);
	return std::string(buffer);
}

//-------------------------------------------------------------//

bool MkStringOp::CheckFileEnable(const char* filePath)
{
	return (_access(filePath, 0) == 0);
}

std::string MkStringOp::GetFileExtension(const std::string& source)
{
	size_t pos = (size_t)GetLastKeywordPosition(source, ".");
	if ((pos >= 0) && (pos < (source.size() - 1)))
	{
		return source.substr(pos + 1);
	}
	return "";
}

std::string MkStringOp::GetDirectoryPathFromDialog(HWND owner, const std::string& msg, const std::string& initialPath)
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = owner;
	bi.pidlRoot = NULL;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.pszDisplayName = NULL;

	std::wstring msgBuffer;
	ConvertString(msg, msgBuffer);
	bi.lpszTitle = msgBuffer.c_str();

	std::wstring pathBuffer;
	if ((!initialPath.empty()) && CheckFileEnable(initialPath.c_str()))
	{
		ConvertString(initialPath, pathBuffer);
			
		bi.lParam = reinterpret_cast<LPARAM>(pathBuffer.c_str());
		bi.lpfn = BrowseCallbackProc;
	}

	LPITEMIDLIST itemIdList = SHBrowseForFolder(&bi);
	
	wchar_t szPath[MAX_PATH];
	if (itemIdList != NULL)
	{
		SHGetPathFromIDList(itemIdList, szPath);

		std::string result;
		ConvertString(std::wstring(szPath), result);
		return result;
	}
	return "";
}

void MkStringOp::GetPathInformation(const std::string& source, std::string& path, std::string& name, std::string& extension)
{
	std::string tmpBuf = source;
	extension = GetFileExtension(tmpBuf);
	if (extension.empty())
	{
		if (source[source.size() - 1] == '.') // "~xxx."
		{
			BackSpace(tmpBuf, 1);
		}
	}
	else
	{
		BackSpace(tmpBuf, (int)extension.size() + 1); // "~xxx.ext"
	}

	int pos = GetLastKeywordPosition(tmpBuf, "\\");
	if (pos <= 0) // "\xxx", "xxx"
	{
		RemoveKeyword(tmpBuf, "\\");
		name = tmpBuf;
		if (!path.empty()) path.clear();
	}
	else
	{
		path = tmpBuf.substr(0, pos + 1); // "\ 포함"
		name = tmpBuf.substr(pos + 1);
	}
}

bool MkStringOp::AttachFromTextFile(std::string& target, const char* filePath, bool ignoreComment, int position)
{
	if (filePath == NULL)
		return false;

	if (!CheckFileEnable(filePath))
		return false;

	// 파일 오픈
	std::ifstream inStream;
	inStream.open(filePath, std::ios::in);
	if (!inStream.is_open())
		return false;

	// 파일 사이즈
	int fileSize = (int)std::distance(std::istreambuf_iterator<char>(inStream), std::istreambuf_iterator<char>());
	if (fileSize <= 0)
		return false;

	inStream.seekg(0, std::ios::beg);

	// 파일 스트림 -> 버퍼
	char* charBuf = new char[fileSize + 1];
	inStream.read(charBuf, fileSize);
	charBuf[fileSize] = 0;
	inStream.close();
	std::string stringBuf = charBuf;
	delete [] charBuf;

	// stringBuf 에서 주석 삭제
	if (ignoreComment)
	{
		//stringBuf.RemoveAllComment();
	}

	// 마무리로 line feed 추가
	stringBuf += "\n";

	// 문자열에 추가
	size_t targetSize = target.size();
	target.reserve(targetSize + stringBuf.size());
	if ((position < 0) || (position <= (int)targetSize))
	{
		// 범위를 넘어가면 뒤쪽
		target += stringBuf;
	}
	else
	{
		// 범위 안이면 삽입
		target.insert(position, stringBuf);
	}

	return true;
}

bool MkStringOp::WriteToTextFile(const std::string& source, const char* filePath, bool overwrite)
{
	// 파일 오픈
	std::ofstream outStream;
	outStream.open(filePath, std::ios::out | (overwrite ? std::ios::trunc : std::ios::app));
	if (!outStream.is_open())
		return false;

	// 변환 후 기록
	outStream.write(source.c_str(), (std::streamsize)source.size());
	outStream.close();

	return true;
}

void MkStringOp::ConvertString(const std::string& source, std::wstring& buffer, unsigned int codePage)
{
	if (!buffer.empty())
	{
		buffer.clear();
	}

	if (!source.empty())
	{
		int sizeOfMultiByte = static_cast<int>(source.size());
		int sizeOfWideChar = ::MultiByteToWideChar(codePage, 0, source.c_str(), sizeOfMultiByte, NULL, 0);
		wchar_t* tmpBuffer = ::SysAllocStringLen(NULL, sizeOfWideChar);
		// 완성형으로 변환하고 싶을때는 두번째 인자로 MB_PRECOMPOSED 사용 할 것
		::MultiByteToWideChar(codePage, 0, source.c_str(), sizeOfMultiByte, tmpBuffer, sizeOfWideChar);
		tmpBuffer[sizeOfWideChar] = NULL;
		buffer = tmpBuffer;
		::SysFreeString(tmpBuffer);
	}
}

void MkStringOp::ConvertString(const std::wstring& source, std::string& buffer, unsigned int codePage)
{
	if (!buffer.empty())
	{
		buffer.clear();
	}

	if (!source.empty())
	{
		int sizeOfChar = ::WideCharToMultiByte(codePage, 0, source.c_str(), -1, NULL, 0, NULL, NULL);
		char* tmpBuffer = new char[sizeOfChar];
		::WideCharToMultiByte(codePage, 0, source.c_str(), -1, tmpBuffer, sizeOfChar, NULL, NULL);
		buffer = tmpBuffer;
		delete [] tmpBuffer;
	}
}

//-------------------------------------------------------------//

int CALLBACK MkStringOp::BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
    case BFFM_INITIALIZED:
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, dwData);
		break;
	}
	return 0;
}

//-------------------------------------------------------------//