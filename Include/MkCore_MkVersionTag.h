#ifndef __MINIKEY_CORE_MKVERSIONTAG_H__
#define __MINIKEY_CORE_MKVERSIONTAG_H__

//------------------------------------------------------------------------------------------------//
// version tag (major, minor, build, revision)
//------------------------------------------------------------------------------------------------//

class MkStr;
class MkPathName;

class MkVersionTag
{
public:

	// 직접 값을 넣어 초기화
	void SetUp(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision);

	// 파일의 버전정보를 읽어들여 초기화
	void SetUp(const MkPathName& filePath);

	// getter
	inline unsigned int GetMajor(void) const { return m_Version[0]; }
	inline unsigned int GetMinor(void) const { return m_Version[1]; }
	inline unsigned int GetBuild(void) const { return m_Version[2]; }
	inline unsigned int GetRevision(void) const { return m_Version[3]; }

	// 문자열로 반환
	MkStr ToString(void) const;

	// operator
	bool operator == (const MkVersionTag& tag) const;
	bool operator != (const MkVersionTag& tag) const;

	// operator
	bool operator < (const MkVersionTag& tag) const;
	bool operator > (const MkVersionTag& tag) const;
	bool operator <= (const MkVersionTag& tag) const;
	bool operator >= (const MkVersionTag& tag) const;

	// 버전 증가
	// 상위 버전이 증가하면 하위 버전은 초기화(0)됨
	// (in) count : 증가 폭
	// 반환값은 새로 설정된 값
	// ex> (title 4.2.17.1).IncreaseMajor(1) -> (title local 5.0.0.0)
	// ex> (title 4.2.17.1).IncreaseBuild(1) -> (title local 4.2.18.0)
	inline unsigned int IncreaseMajor(unsigned int count = 1) { return _IncreaseVersion(0, count); }
	inline unsigned int IncreaseMinor(unsigned int count = 1) { return _IncreaseVersion(1, count); }
	inline unsigned int IncreaseBuild(unsigned int count = 1) { return _IncreaseVersion(2, count); }
	inline unsigned int IncreaseRevision(unsigned int count = 1) { return _IncreaseVersion(3, count); }

	MkVersionTag();
	MkVersionTag(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision);
	MkVersionTag(const MkPathName& filePath);

protected:

	unsigned int _IncreaseVersion(unsigned int index, unsigned int count);

protected:

	unsigned int m_Version[4];
};

//------------------------------------------------------------------------------------------------//

#endif
