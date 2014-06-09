
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkVersionTag.h"

#pragma comment (lib, "version.lib")

//------------------------------------------------------------------------------------------------//

void MkVersionTag::SetUp(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision)
{
	m_Version[0] = major;
	m_Version[1] = minor;
	m_Version[2] = build;
	m_Version[3] = revision;
}

void MkVersionTag::SetUp(const MkPathName& filePath)
{
	// http://www.tipssoft.com/bulletin/tb.php/FAQ/1196
	// http://timeless.tistory.com/46

	const wchar_t* pathPtr = filePath.GetPtr();
	bool hasVersion = false;
	DWORD h_version_handle;
	DWORD version_info_size = GetFileVersionInfoSize(pathPtr, &h_version_handle);
	if (version_info_size > 0)
	{
		HANDLE h_memory = GlobalAlloc(GMEM_MOVEABLE, version_info_size);
		LPVOID p_info_memory = GlobalLock(h_memory);
		GetFileVersionInfo(pathPtr, h_version_handle, version_info_size, p_info_memory);

		MkStr versionBuffer;
		LPVOID p_lan = NULL;
		UINT tmp_size = 0;
		if (VerQueryValue(p_info_memory, L"\\VarFileInfo\\Translation", (void **)&p_lan, &tmp_size))
		{
			WORD* lanId = reinterpret_cast<WORD*>(p_lan);
			wchar_t destination[255];
			wsprintf(destination, L"\\StringFileInfo\\%04X%04X\\FileVersion", lanId[0], lanId[1]);

			UINT data_size = 0;
			wchar_t* p_ver = NULL;
			if (VerQueryValue(p_info_memory, destination, (void **)&p_ver, &data_size))
			{
				versionBuffer = p_ver;
				if (!versionBuffer.Empty())
				{
					versionBuffer.RemoveBlank();

					MkArray<MkStr> tokens;
					if (versionBuffer.Tokenize(tokens, L",") == 4)
					{
						for (unsigned int i=0; i<4; ++i)
						{
							m_Version[i] = tokens[i].ToUnsignedInteger();
						}
						
						hasVersion = true;
					}
				}
			}
		}

		GlobalUnlock(h_memory); 
		GlobalFree(h_memory);
	}

	if (!hasVersion)
	{
		SetUp(0, 0, 0, 0);
	}
}

MkStr MkVersionTag::ToString(void) const
{
	MkStr buffer;
	buffer.Reserve(256);

	buffer += m_Version[0];
	buffer += L".";
	buffer += m_Version[1];
	buffer += L".";
	buffer += m_Version[2];
	buffer += L".";
	buffer += m_Version[3];

	buffer.OptimizeMemory();
	return buffer;
}

bool MkVersionTag::operator == (const MkVersionTag& tag) const
{
	return ((m_Version[0] == tag.GetMajor()) &&
		(m_Version[1] == tag.GetMinor()) &&
		(m_Version[2] == tag.GetBuild()) &&
		(m_Version[3] == tag.GetRevision()));
}

bool MkVersionTag::operator != (const MkVersionTag& tag) const
{
	return ((m_Version[0] != tag.GetMajor()) ||
		(m_Version[1] != tag.GetMinor()) ||
		(m_Version[2] != tag.GetBuild()) ||
		(m_Version[3] != tag.GetRevision()));
}

bool MkVersionTag::operator < (const MkVersionTag& tag) const
{
	if (m_Version[0] < tag.GetMajor()) return true;
	if (m_Version[0] > tag.GetMajor()) return false;
	if (m_Version[1] < tag.GetMinor()) return true;
	if (m_Version[1] > tag.GetMinor()) return false;
	if (m_Version[2] < tag.GetBuild()) return true;
	if (m_Version[2] > tag.GetBuild()) return false;
	if (m_Version[3] < tag.GetRevision()) return true;
	return false;
}

bool MkVersionTag::operator > (const MkVersionTag& tag) const
{
	if (m_Version[0] > tag.GetMajor()) return true;
	if (m_Version[0] < tag.GetMajor()) return false;
	if (m_Version[1] > tag.GetMinor()) return true;
	if (m_Version[1] < tag.GetMinor()) return false;
	if (m_Version[2] > tag.GetBuild()) return true;
	if (m_Version[2] < tag.GetBuild()) return false;
	if (m_Version[3] > tag.GetRevision()) return true;
	return false;
}

bool MkVersionTag::operator <= (const MkVersionTag& tag) const
{
	if (m_Version[0] < tag.GetMajor()) return true;
	if (m_Version[0] > tag.GetMajor()) return false;
	if (m_Version[1] < tag.GetMinor()) return true;
	if (m_Version[1] > tag.GetMinor()) return false;
	if (m_Version[2] < tag.GetBuild()) return true;
	if (m_Version[2] > tag.GetBuild()) return false;
	if (m_Version[3] < tag.GetRevision()) return true;
	if (m_Version[3] > tag.GetRevision()) return false;
	return true;
}

bool MkVersionTag::operator >= (const MkVersionTag& tag) const
{
	if (m_Version[0] > tag.GetMajor()) return true;
	if (m_Version[0] < tag.GetMajor()) return false;
	if (m_Version[1] > tag.GetMinor()) return true;
	if (m_Version[1] < tag.GetMinor()) return false;
	if (m_Version[2] > tag.GetBuild()) return true;
	if (m_Version[2] < tag.GetBuild()) return false;
	if (m_Version[3] > tag.GetRevision()) return true;
	if (m_Version[3] < tag.GetRevision()) return false;
	return true;
}

MkVersionTag::MkVersionTag()
{
	SetUp(0, 0, 0, 0);
}

MkVersionTag::MkVersionTag(unsigned int major, unsigned int minor, unsigned int build, unsigned int revision)
{
	SetUp(major, minor, build, revision);
}

MkVersionTag::MkVersionTag(const MkPathName& filePath)
{
	SetUp(filePath);
}

unsigned int MkVersionTag::_IncreaseVersion(unsigned int index, unsigned int count)
{
	assert((index >= 0) && (index <= 3));
	if (count > 0)
	{
		m_Version[index] += count;
		for (unsigned int i = index + 1; i < 4; ++i)
		{
			m_Version[i] = 0;
		}
	}
	return m_Version[index];
}

//------------------------------------------------------------------------------------------------//
