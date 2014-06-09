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

	// ���� ���� �־� �ʱ�ȭ
	void SetUp(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision);

	// ������ ���������� �о�鿩 �ʱ�ȭ
	void SetUp(const MkPathName& filePath);

	// getter
	inline unsigned int GetMajor(void) const { return m_Version[0]; }
	inline unsigned int GetMinor(void) const { return m_Version[1]; }
	inline unsigned int GetBuild(void) const { return m_Version[2]; }
	inline unsigned int GetRevision(void) const { return m_Version[3]; }

	// ���ڿ��� ��ȯ
	MkStr ToString(void) const;

	// operator
	bool operator == (const MkVersionTag& tag) const;
	bool operator != (const MkVersionTag& tag) const;

	// operator
	bool operator < (const MkVersionTag& tag) const;
	bool operator > (const MkVersionTag& tag) const;
	bool operator <= (const MkVersionTag& tag) const;
	bool operator >= (const MkVersionTag& tag) const;

	// ���� ����
	// ���� ������ �����ϸ� ���� ������ �ʱ�ȭ(0)��
	// (in) count : ���� ��
	// ��ȯ���� ���� ������ ��
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
