#ifndef __MINIKEY_CORE_MKPATHNAME_H__
#define __MINIKEY_CORE_MKPATHNAME_H__

//------------------------------------------------------------------------------------------------//
// MkStr�� ���̽��� �� ��θ�
// - ��� ���� Ž��
// - ��� ����
// - ���� ó��
// - �ý��� ���
// - ��� ����
// - ���̾�α� ����
//
// ������� ��� ������ ��ȯ�� ���ؼ��� �ΰ��� ������ ����
// - module directory : ���μ����� ����� ���. ���μ��� ������ �Һ�
// - working directory : ���� �۾����� ���. ���÷� ���� ����
// �ټ��� �մ�� ������ ��� working directory�� �ŷڼ��� �����ϹǷ� module directory��
// ����� ������ ����θ� ���� root directory�� ����� ó���� �ٰ����� ����
//
//****************************************************************************************************
// (NOTE) ������ ����� ���� �� '�ش� ���'�� ������, Ȥ�� root directory ��� �������!!!
//****************************************************************************************************
//
// Ȯ���ڰ� ���� ���ϸ�� ���丮 ��ΰ� ȥ���� ���ɼ��� �����Ƿ� ��Ģ�� ���� �̸� ��Ȯ�� ��
// - ���� ��� : Ȯ���ڰ� ����(L"abc\\test.exe")
// - ���丮 ��� : L"\\"�� ����ǰų� Ȯ���ڰ� ����(L"abc\\test\\", L"abc\\test"), Ȥ�� �����ڿ�
//
// ����/��� ��� �Ǵ� ��Ģ
// - ���� ��� : ������ ����̺������ ����(L"d:\\file\\test.exe"), ��Ʈ��ũ�� L"\\\\"�� ����(L"\\\\patchSrv\\")
// - ��� ��� : ���� ������ ����
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkPathName : public MkStr
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ : ���� static ���� ����
	// ��Ģ�����δ� lock�� �ɾ�� ������(data race ����), ����ڰ� ������ �ʱ�ȭ �� �� ���̶� ���
	//------------------------------------------------------------------------------------------------//

	// rootPath : ����, Ȥ�� module directory ��� ��� ���丮 ���. NULL�� ��� module directory�� ����
	// ex> MkPathName::SetUp(L"..\\FileRoot\\");
	static void SetUp(const wchar_t* rootPath = NULL);

	//------------------------------------------------------------------------------------------------//
	// ������
	//------------------------------------------------------------------------------------------------//

	MkPathName(void);
	MkPathName(const MkPathName& str);
	MkPathName(const MkStr& str);
	MkPathName(const MkHashStr& str);
	MkPathName(const std::wstring& str);
	MkPathName(const wchar_t* wcharArray);

	//------------------------------------------------------------------------------------------------//
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���� ��� ���翩�� �˻�
	// return : ���翩��
	bool CheckAvailable(void) const;

	// ���丮 ��� ���� ��ȯ
	// return : true�� ���丮, false�� ���� ���
	bool IsDirectoryPath(void) const;

	// ������ ���� ��ȯ
	// return : true�� ����, false�� ��� ���
	bool IsAbsolutePath(void) const;

	// ���� ũ�� ��ȯ
	// return : �ش� ������ ũ��. ���丮 ����̰ų� ������ ������ 0 ��ȯ
	unsigned int GetFileSize(void) const;

	// ��ο� �ش��ϴ� ���ڿ��� ��ȯ
	// return : ��� ���ڿ�
	// ex> (L"D:\\test\\test.txt").GetPath() -> L"D:\\test\\"
	// ex> (L"D:\\test\\test").GetPath() -> L"D:\\test\\"
	// ex> (L"test\\test\\").GetPath() -> L"test\\test\\"
	// ex> (L"test.txt").GetPath() -> L""
	MkPathName GetPath(void) const;

	// ���� ����� ��� ���ϸ� �ش��ϴ� ���ڿ��� ��ȯ
	// (in) includeExtension : Ȯ���� ���Կ���
	// return : ���ϸ� ���ڿ�
	// ex> (L"D:\\test\\test.txt").GetFileName(true) -> L"test.txt"
	// ex> (L"D:\\test\\test.txt").GetFileName(false) -> L"test"
	MkPathName GetFileName(bool includeExtension = true) const;

	// ���� ����� ��� Ȯ���ڿ� �ش��ϴ� ���ڿ��� ��ȯ
	// return : ���� Ȯ���� ���ڿ�
	// ex> (L"D:\\test\\test.txt").GetFileExtension() -> L"txt"
	// ex> (L"D:\\test\\test").GetFileExtension() -> L""
	MkStr GetFileExtension(void) const;

	// ���, �����̸�, Ȯ���ڷ� ������ ��ȯ
	// (NOTE) Ȯ���� ���� ����� ��� �������� '\'�� '/'�� ����Ǹ� ���丮 ���, �ƴϸ� ���ϸ����� ��ȯ��
	// (out) path : ���
	// (out) name : Ȯ���ڰ� ���ܵ� ���ϸ�
	// (out) extension : Ȯ����
	// ex> (L"D:\\test\\test.txt").SplitPath(path, name, extension) -> path == L"D:\\test\\", name == L"test", extension == L"txt"
	// ex> (L"D:\\test\\test").SplitPath(path, name, extension) -> path == L"D:\\test\\", name == L"test", extension == L""
	// ex> (L"test\\test\\").SplitPath(path, name, extension) -> path == L"test\\test", name == L"", extension == L""
	// ex> (L"test\\.txt").SplitPath(path, name, extension) -> path == L"test\\", name == L"", extension == L".txt"
	void SplitPath(MkPathName& path, MkStr& name, MkStr& extension) const;

	//------------------------------------------------------------------------------------------------//
	// ��� ���� ó���� ��ƿ��Ƽ
	//------------------------------------------------------------------------------------------------//

	class Filter
	{
	public:

		bool SetUp(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter);
		bool CheckAvailable(void) const; // ���� ���翩��
		bool CheckAvailableFile(const MkPathName& relativePath) const; // �ش� ���� ��� ��ȿ ����
		bool CheckAvailableDirectory(const MkPathName& relativePath) const; // �ش� ���丮 ��� ��ȿ ����
		
		bool GetExceptionFilter(MkArray<MkStr>& buffer) const;
		bool GetNameFilter(MkArray<MkStr>& buffer) const;
		bool GetExtensionFilter(MkArray<MkStr>& buffer) const;
		bool GetPrefixFilter(MkArray<MkStr>& buffer) const;

		void Clear(void);

		Filter(bool result);

	protected:

		static bool _ConvertArrayToHashMap(const MkArray<MkPathName>& source, MkHashMap<MkHashStr, int>& target);
		static bool _ConvertArrayToArray(const MkArray<MkStr>& source, MkArray<MkStr>& target);
		static bool _GetKeyList(const MkHashMap<MkHashStr, int>& source, MkArray<MkStr>& buffer);

	protected:

		MkHashMap<MkHashStr, int> m_ExceptionFilter;
		MkHashMap<MkHashStr, int> m_NameFilter;
		MkHashMap<MkHashStr, int> m_ExtensionFilter;
		MkArray<MkStr> m_PrefixFilter;

		bool m_OnExceptionFilter;
		bool m_OnNameFilter;
		bool m_OnExtensionFilter;
		bool m_OnPrefixFilter;

		bool m_PassingResult;
	};

	class WhiteFilter : public Filter
	{
	public:
		WhiteFilter();
	};

	class BlackFilter : public Filter
	{
	public:
		BlackFilter();
	};

	//------------------------------------------------------------------------------------------------//
	// ���� ���� �˻�
	// ���丮 ��θ� ��ȿ. ���� �Ӽ� ���� ����
	// ���Ͱ��� ���ڿ��� ��ҹ��� ���� ����
	// �� ���� �迭�� ������ ��� �ش� �˻�� ���� ����
	//------------------------------------------------------------------------------------------------//

	// ��� ���� ���� ��ȯ
	// (in) includeSubDirectory : ���� ���� Ž������
	// return : ���丮 ���� ���� ��
	unsigned int GetFileCount(bool includeSubDirectory = true) const;

	// ��� ���� ������ ũ���� �� �� ��ȯ
	// (out) totalFileSize : ���丮 ���� ��� ������ ũ�� ��
	// (in) includeSubDirectory : ���� ���� Ž������
	// return : ���丮 ���� ���� ��
	unsigned int GetFileCountAndTotalSize(unsigned __int64& totalFileSize, bool includeSubDirectory = true) const;

	// ��� ���� ��� ����Ʈ ��ȯ
	// (out) filePathList : ���� ��ΰ� ��� ����
	// (in) includeSubDirectory : ���� ���� Ž������. ���� �������� ���ϸ��� ���� ���丮������ ��� ��ΰ� ���Ե�
	// (in) includeZeroSizeFile : ���� ũ�Ⱑ 0�� ���ϵ� ���� �� �������� ����
	// (in) filter(opt) : ���͸��� ��ü. NULL�̸� ���͸� ����
	void GetFileList(MkArray<MkPathName>& filePathList, bool includeSubDirectory = true, bool includeZeroSizeFile = true, const Filter* filter = NULL) const;

	// ���͸��� ����� ���ϵ鸸 �˻�(white filter)
	// (out) filePathList : ���� ��ΰ� ��� ����
	// (in) nameFilter : ��� �� ����/���丮 �̸� ����Ʈ. ����� �����ϸ� '\'�� '/'�� ����Ǹ� ���丮 ��ζ� �ν�
	//			(ex> L"Item\\armor\\helmet.dds", L"Item\\weapon\\")
	// (in) extensionFilter : ��� �� ���� Ȯ���� ����Ʈ
	// (in) prefixFilter : ��� �� ���� ���λ� ����Ʈ
	// (in) exceptionFilter : ���͸��� ��� ���� ���������� ���� �� ���� �̸� ����Ʈ
	// (in) includeSubDirectory : ���� ���� Ž������. ���� �������� ���ϸ��� ���� ���丮������ ��� ��ΰ� ���Ե�
	void GetWhiteFileList(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
		const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory = true, bool includeZeroSizeFile = true) const;

	// ���͸��� ������� �� �� ���ϵ鸸 �˻�(black filter)
	// (out) filePathList : ���� ��ΰ� ��� ����
	// (in) nameFilter : ���� �� ����/���丮 �̸� ����Ʈ. ����� �����ϸ� '\'�� '/'�η� ����Ǹ� ���丮 ��ζ� �ν�
	//			(ex> L"Item\\armor\\helmet.dds", L"Item\\weapon\\")
	// (in) extensionFilter : ���� �� ���� Ȯ���� ����Ʈ
	// (in) prefixFilter : ���� �� ���� ���λ� ����Ʈ
	// (in) exceptionFilter : ���͸��� ��� ���� ���������� ���� �� ���� �̸� ����Ʈ
	// (in) includeSubDirectory : ���� ���� Ž������. ���� �������� ���ϸ��� ���� ���丮������ ��� ��ΰ� ���Ե�
	void GetBlackFileList(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
		const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory = true, bool includeZeroSizeFile = true) const;

	// white filter�� Ư���� ���� �� ���� ������ "[prefix][index].[extension]" ������ ���ϵ鸸 �˻��� <index, ���� ���> ���·� ��ȯ
	// (out) filePathTable : index�� ���� ��ΰ� ��� ����
	// (in) prefix : ��� ���λ�. �ݵ�� �����ؾ� ��
	// (in) extension : ��� Ȯ����. �ݵ�� �����ؾ� ��
	// ex> "MAP_01.bin", "MAP_04.bin", "MAP_12.bin" ������ ������ ��� GetIndexFormFileList(..., L"map_", L"bin")�� ȣ���ϸ�,
	//     <1, MAP_01.bin>, <4, MAP_04.bin>, <12, MAP_12.bin>�� ����� ��ȯ��
	// (NOTE) �ٸ� �̸������� ���� �ε����� ���� ���ϵ��� ������ ���(ex> "MAP_01.bin", "MAP_001.bin") �������� �������� ����
	void GetIndexFormFileList
		(MkMap<unsigned int, MkPathName>& filePathTable, const MkStr& prefix, const MkStr& extension, bool includeZeroSizeFile = true) const;

	// ��ο� �����ϴ� ������ ������ �����Ͻø� 2000�� ���� �ʴ����� ��ȯ�� ��ȯ
	// unsigned int ������ 2136.2����� ��ȿ����
	// ������ ���ų� ���� �߻��ϸ� 0 ��ȯ
	unsigned int GetWrittenTime(void) const;

	// GetWrittenTime()���� ���� �ð��� ��/��/��/��/��/�� ������ ��ȯ
	// (NOTE) 2000�� 1�� 1�� ������ �ð��� 2000�� 1�� 1�Ϸ� ������
	static void ConvertWrittenTime(unsigned int writtenTime, int& year, int& month, int& day, int& hour, int& min, int& sec);

	// ��/��/��/��/��/�� �������� GetWrittenTime() �ð����� ��ȯ
	// (NOTE) 2000�� 1�� 1�� ������ �ð��� 0���� ������
	static unsigned int ConvertWrittenTime(int year, int month, int day, int hour, int min, int sec);

	//------------------------------------------------------------------------------------------------//
	// ��� ����
	//------------------------------------------------------------------------------------------------//

	// ���ϸ� ����
	// ��θ� ���ϸ��� �����ϴ� ��� name���� ��ü
	// return : ���� ����. ���丮 ����� ��� �׻� false
	// ex> (L"D:\\test\\test.exe").ChangeFileName(L"newtest") -> L"D:\\test\\newtest.exe"
	bool ChangeFileName(const MkStr& name);

	// Ȯ���� ����
	// ��θ� Ȯ���ڰ� �����ϴ� ��� extension���� ��ü
	// return : ���� ����. ���丮 ����� ��� �׻� false
	// ex> (L"D:\\test\\test.exe").ChangeFileExtension(L"bin") -> L"D:\\test\\test.bin"
	bool ChangeFileExtension(const MkStr& extension);

	// ���丮 ���ȭ. �������� �˻��Ͽ� L"\\" �߰�
	// (NOTE) ������ ���ȭ �� �����Ƿ� ���� ��ο� �߸� �����ϴ� ��� ����
	// ex> (L"D:\\test\\test").CheckAndAddBackslash() -> L"D:\\test\\test\\"
	// ex> (L"test\\test\\").CheckAndAddBackslash() -> L"test\\test\\" (���� ����)
	// ex> (L"test\\test.exe").CheckAndAddBackslash() -> L"test\\test.exe\\", test.exe�� �������� �ǹ���
	void CheckAndAddBackslash(void);

	// ��� ����ȭ
	// ex> (L"D:\\abc\\test").OptimizePath() -> L"D:\\abc\\test\\"
	// ex> (L"D:\\abc\\test\\..\\..\\.\\test.exe").OptimizePath() -> L"D:\\test.exe"
	// ex> (L"..\\abc\\..\\.\\test").OptimizePath() -> L"..\\test\\"
	// ex> (L".\\test.exe").OptimizePath() -> L"test.exe"
	void OptimizePath(void);

	// path�� ������̸� module directory ���� �����η�, �������̸� ��� �� ��� ����ȭ
	// (in) path : ��θ�
	// ex> L"D:\\test\\test.exe"���� ����Ǿ��ٸ� module directory�� L"D:\\test\\"�̱⶧����
	//     ConvertToModuleBasisAbsolutePath(L"..\\test.txt") -> L"D:\\test.txt"
	//     ConvertToModuleBasisAbsolutePath(L"C:\\abc\\..\\") -> L"C:\\"
	void ConvertToModuleBasisAbsolutePath(const MkPathName& path);

	// module directory�� ���� ��ġ�� �������� ��� module directory ���� ����ȭ�� ����η� ��ȯ
	// ������� ��� ����ȭ�� ����
	// return : ������������ module directory�� �ٸ� ��ġ�� ��� false, �ƴϸ� true
	bool ConvertToModuleBasisRelativePath(void);

	// path�� ������̸� working directory ���� �����η�, �������̸� ��� �� ��� ����ȭ
	// (in) path : ��θ�
	void ConvertToWorkingBasisAbsolutePath(const MkPathName& path);

	// working directory�� ���� ��ġ�� �������� ��� working directory ���� ����ȭ�� ����η� ��ȯ
	// ������� ��� ����ȭ�� ����
	// return : ������������ working directory�� �ٸ� ��ġ�� ��� false, �ƴϸ� true
	bool ConvertToWorkingBasisRelativePath(void);

	// path�� ������̸� root directory ���� �����η�, �������̸� ��� �� ��� ����ȭ
	// (in) path : ��θ�
	void ConvertToRootBasisAbsolutePath(const MkPathName& path);

	// root directory�� ���� ��ġ�� �������� ��� root directory ���� ����ȭ�� ����η� ��ȯ
	// ������� ��� ����ȭ�� ����
	// return : ������������ root directory�� �ٸ� ��ġ�� ��� false, �ƴϸ� true
	bool ConvertToRootBasisRelativePath(void);

	//------------------------------------------------------------------------------------------------//
	// ���� ó��
	// ���� ��λ� ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���� ����
	// return : ���������� ������ �������� �ʴ��� ����(�̹� ������ ���� ���¿��� true)
	//          ���丮 ����� ��� false
	bool DeleteCurrentFile(void) const;

	// ���丮 ����
	// ���� ���丮, ���� ���� ���� ����(read-only, system, hidden �� �Ӽ� ��� ���� ���� ����)
	// deleteAllFiles : true�� ���Ե� ��� ���ϱ��� ����. false�� �� ���丮�� ����
	// return : ���������� ���丮�� �������� �ʴ��� ����(�̹� ���丮�� ���� ���¿��� true)
	//          ���� ����� ��� false
	bool DeleteCurrentDirectory(bool deleteAllFiles = true) const;

	// ���� ��ο� �̸�������� directory�� üũ�� ������ ����
	// return : ���������� ��ΰ� �����ϴ��� ����(�̹� ��ΰ� �����ϴ� ���¿��� true)
	//          ���� ����� ��� false
	bool MakeDirectoryPath(void) const;

	// ���ϸ� ����
	// (in) newFileName : �� ���ϸ�. �߰���� ���� ���ϸ�+Ȯ���� ����
	// return : ���������� ���ϸ� �ش��ϴ� ���� ���翩��(���ϸ� ���� �ʿ䰡 ��� true)
	//          ������ ��ο� �������� �ʰų� ���丮 ����̰ų� �̸� ������ �����ϸ� false
	// ex (L"file\\abc\\test.bin").RenameCurrentFile(L"t_0.bin") -> L"file\\abc\\t_0.bin"
	bool RenameCurrentFile(const MkPathName& newFileName);

	// ���� ����
	// (in) newPath : �ش� ������ �����ϴ� ���丮�� �������� �� ��� Ȥ�� ���� ����/���丮 ���
	//                ���丮 ����� ��� ������ ������ ���ϸ� ���
	// (in) failIfExists : newPath�� �ش��ϴ� ������ �̹� ���� �� ��� ���� ó������ ����
	// return : ���� ����. ���� ������ ��ο� �������� �ʰų� ���簡 �����ϸ� false
	// ex (L"file\\abc\\test.bin").CopyCurrentFile(L"..\\kk") -> L"file\\kk\\test.bin"
	// ex (L"file\\abc\\test.bin").CopyCurrentFile(L"..\\kk\\t_0.mmb") -> L"file\\kk\\t_0.mmb"
	bool CopyCurrentFile(const MkPathName& newPath, bool failIfExists = true) const;

	// ���� �̵�(cut & paste)
	// (in) newPath : �ش� ������ �����ϴ� ���丮�� �������� �� ��� Ȥ�� ���� ����/���丮 ���
	//                ���丮 ����� ��� ������ ������ ���ϸ� ���
	// return : ���� ����. ���� ������ ��ο� �������� �ʰų� �̵��� �����ϸ� false
	// ex (L"file\\abc\\test.bin").MoveCurrentFile(L"..\\kk") -> L"file\\kk\\test.bin"
	// ex (L"file\\abc\\test.bin").MoveCurrentFile(L"..\\kk\\t_0.mmb") -> L"file\\kk\\t_0.mmb"
	bool MoveCurrentFile(const MkPathName& newPath) const;

	// ���� �ð� ����
	// (in) writtenTime : ���� �ð�. GetWrittenTime() ����
	// return : ���� ����
	bool SetWrittenTime(unsigned int writtenTime) const;

	// ���� �ð� ����
	// (in) year, month, day, hour, min, sec : ���� �ð�
	// return : ���� ����
	bool SetWrittenTime(int year, int month, int day, int hour, int min, int sec) const;

	//------------------------------------------------------------------------------------------------//
	// �ý��� ���
	// ���� instance�� ���ӵ��� �ʴ� global ���̹Ƿ� static ����
	//------------------------------------------------------------------------------------------------//

	// ���� ����� ���ø����̼� �̸��� ����
	static const MkStr& GetApplicationName(void);

	// ���������� ��ġ�� ���丮 ���(module directory) ���
	// ���μ����� ���۵� ���� �Һ�
	// return : module directory�� ������
	// ex> D:\\test\\test.exe���� ����Ǿ��ٸ�, GetModuleDirectory() -> L"D:\\test\\"
	static const MkPathName& GetModuleDirectory(void);

	// ���� �۾� ���(working directory) ���
	// (NOTE) �ܺ��ڵ忡 ���� ����ڵ� �𸣰� ���� �� �� �����Ƿ�(ex> CFileDialog) �ŷڼ��� ������
	// return : working directory�� ������
	static MkPathName GetWorkingDirectory(void);

	// �۾� ��� ����
	// (in) workingPath : �۾� ��η� ���� �� ���� �����ϴ� ���� ���丮 ���
	static void SetWorkingDirectory(const MkPathName& workingDirectory);

	// ������ ������ ��Ʈ ��� (root directory) ���
	// return : root directory�� ������
	static const MkPathName& GetRootDirectory(void);

	// ��Ʈ ��� ����
	// (in) rootPath : ���� �����ϴ� ����, Ȥ�� module directory�� �������� �� ��� ���丮 ���
	static void SetRootDirectory(const MkPathName& rootPath);

	//------------------------------------------------------------------------------------------------//
	// ��� ����
	//------------------------------------------------------------------------------------------------//

	// ���丮 ����� ��� �ش� ����� ���� Ž��â(explorer.exe) ����
	void OpenDirectoryInExplorer(void) const;

	// ���� ����� ��� shell command�� ����
	// (NOTE) ���� ��İ� ���� ���α׷��� Ŭ���̾�Ʈ�� OS ���ÿ� ���� �ٸ�
	// (in) argument : ���޵� ����
	void OpenFileInVerb(const MkStr& argument = L"") const;

	// Ȯ���ڰ� �����ϴ� ������ ��������(.exe) ��ΰ� ������� ��� ���� ���
	// ���μ��� ����� working directory�� ����� ������ �����ϴ� ��η� ����
	// (in) processTitle : ������ ���μ����� Ÿ��Ʋ
	// (in) argument : ���޵� ����
	// return : ���� ��������
	bool ExcuteWindowProcess(const MkStr& processTitle, const MkStr& argument = L"") const;

	// Ȯ���ڰ� �����ϴ� �ܼ� ��������(.exe) ��ΰ� ������� ��� ���� ���
	// ���μ��� ����� working directory�� ����� ������ �����ϴ� ��η� ����
	// (in) processTitle : ������ ���μ����� Ÿ��Ʋ
	// (in) argument : ���޵� ����
	// return : ���� ��������
	bool ExcuteConsoleProcess(const MkStr& processTitle, const MkStr& argument = L"") const;

	//------------------------------------------------------------------------------------------------//
	// ���̾�α� ����
	//------------------------------------------------------------------------------------------------//

	// ���� ���� ���� ���̾�α�
	// (in) extensionList : ���͸� Ȯ���� ���. ��� ������ ��ü ���(.*)
	// (in) owner : ���̾�α� ���� ������ �ڵ�
	// return : ���� ���� ����
	bool GetSingleFilePathFromDialog(HWND owner = NULL); // ��ü Ȯ���� ���(.*)
	bool GetSingleFilePathFromDialog(const MkStr& extension, HWND owner = NULL); // ���� Ȯ���� ���
	bool GetSingleFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner = NULL);

	// ���� ���� ���� ���̾�α�
	// (out) directoryPath : ���õ� ���ϵ��� ��ġ�� ���丮 ���
	// (out) fileNameList : ���õ� ���� �̸� ���
	// (in) extensionList : ���͸� Ȯ���� ���. ��� ������ ��ü ���(.*)
	// (in) owner : ���̾�α� ���� ������ �ڵ�
	// return : ���õ� ���� ��
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, HWND owner = NULL); // ��ü Ȯ���� ���(.*)
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkStr& extension, HWND owner = NULL); // ���� Ȯ���� ���
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner = NULL);

	// ���� ���� ��� ��ȯ ���̾�α�
	// (in) extensionList : ���͸� Ȯ���� ���. ��� ������ ��ü ���(.*)
	// (in) owner : ���̾�α� ���� ������ �ڵ�
	// return : ���� ���� ����
	// (NOTE) ����ڰ� �������� �ʴ� ������ ���� ���� �� �� �����Ƿ� Ȯ���ڰ� �����ϸ� ��ȿ�� �˻縦 ����
	bool GetSaveFilePathFromDialog(HWND owner = NULL); // ��ü Ȯ���� ���(.*). ��ȿ�� �˻� ���� ����
	bool GetSaveFilePathFromDialog(const MkStr& extension, HWND owner = NULL); // ���� Ȯ���� ���. ��ȿ�� �˻�
	bool GetSaveFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner = NULL); // ��ȿ�� �˻�

	// ���丮 Ž�� ���̾�α�(SHBrowseForFolder)�� ���� ���� ���� ��� ����
	// (in) msg : ���̾�α׿� ��µ� �޼��� ���ڿ�
	// (in) owner : ���̾�α� ���� ������ �ڵ�
	// (in) initialPath : �⺻���ÿ� ���� ���丮 ���
	// return : ���� ���� ����
	bool GetDirectoryPathFromDialog(const MkStr& msg = L"", HWND owner = NULL, const MkPathName& initialPath = L"");

	//------------------------------------------------------------------------------------------------//
	// structure(MkDataNode) ���� �� ���� �Լ�
	//------------------------------------------------------------------------------------------------//

	// �ش� ���丮 ��ο� ���� ��� ����, ���丮 ������ data node�� ���
	// ũ�Ⱑ 4G�� �Ѵ� ���� ���ϰ� ������ �ƹ��͵� ���� ���丮�� ������
	// (out) node : ��� ���
	// return : �Ҽ� ������ ���������� ������ �ð�
	// ex>
	//	MkPathName path = L"D:\\Docs\\";
	//	MkDataNode node;
	//	unsigned int wt = path.ExportSystemStructure(node);
	//	node.SaveToText(MkStr(wt) + L".txt");
	unsigned int ExportSystemStructure(MkDataNode& node) const;

	// node�� ������ �����ϴ� ���丮 key ��ȯ
	static void __GetSubDirectories(const MkDataNode& node, MkArray<MkHashStr>& subDirs);

	// node�� ������ �����ϴ� ���� key ��ȯ
	static void __GetSubFiles(const MkDataNode& node, MkArray<MkHashStr>& subFiles);

	// node�� count ����
	static void __IncreaseDirectoryCount(MkDataNode& node); // KeyDirCount
	static void __IncreaseFileCount(MkDataNode& node); // KeyFileCount

	// �� file node�� size�� written time�� �ٸ��� ���� ��� ��ȯ
	// ������ currFileNode�� size������ lastFileNode�� �����ϰ� ����(lastFileNode�� ���� ũ�� ���� ����)
	static bool __CheckFileDifference(const MkDataNode& lastFileNode, MkDataNode& currFileNode);

	// ���ϸ��� node name���� ������ ũ��(KeyFileSize), �����ð�(KeyWrittenTime) ���
	// (out) node : ������ �����ϴ� ���丮 node
	// (in) compressed : ���� ����
	// (in) origSize : ���� ���� ũ��
	// (in) compSize : compressed�� true�� ��� ���� �� ũ��
	// (in) writtenTime : ���� �����ð�
	// return : ������ ��� ������
	// ex 0>
	//	Node "28.mp3"
	//	{
	//		uint SZ = 1930819;
	//		uint WT = 534955830;
	//	}
	// ex 1>
	//	Node "datanode_00.bin"
	//	{
	//		uint SZ = // [2]
	//			2080 / 701;
	//		uint WT = 534955830;
	//	}
	MkDataNode* __CreateFileStructureInfo(MkDataNode& node, bool compressed, unsigned int origSize, unsigned int compSize, unsigned int writtenTime);

	//------------------------------------------------------------------------------------------------//

protected:

	// Ȯ���� ��ġ ��ȯ
	// 0 : Ȯ���� ����, 1 �̻� : Ȯ���� ��ġ, MKDEF_ARRAY_ERROR : ������ ���
	unsigned int _GetExtensionPosition(void) const;

	// basePath�� �������� sourcePath�� ������ ����
	void _UpdateCurrentPath(const MkPathName& basePath, const MkPathName& sourcePath);

	// basePath�� targetPath�� ���� ��ġȯ�� ������� �Ǻ�
	// basePath�� targetPath�� ���� ����� ��
	bool _CheckOnSameDevice(const MkPathName& basePath, const MkPathName& targetPath) const;

	// basePath�� �������� �� ����η� ��ȯ
	// basePath�� �ٸ� ��ġ�� ��� false ��ȯ 
	bool _ConvertToRelativePath(const MkPathName& basePath);

	// copy or move
	bool _CopyOrMoveCurrentFile(const MkPathName& newPath, bool copy, bool failIfExists) const;

	// ���μ��� ����
	bool _ExcuteProcess(const MkStr& processTitle, DWORD flag, const MkStr& argument) const;

	// GetDirectoryPathFromDialog()�� �ݹ�
	// SHBrowseForFolder���� �ʱ��� ������ ���ؼ��� �ݹ�ܿ� ���� ���ٰ� �Ѵ�
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData);

	static unsigned int _GetFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner, bool singleSelection, bool forOpen);

	static void _GetSubNodesByTag(const MkDataNode& node, MkArray<MkHashStr>& subNodes, const MkHashStr& countTag, const MkHashStr& typeTag, bool exist);
	static void _IncreaseUnitCountByTag(MkDataNode& node, const MkHashStr& key);

public:

	// data node ������ key
	static const MkHashStr KeyFileCount;
	static const MkHashStr KeyDirCount;
	static const MkHashStr KeyFileSize;
	static const MkHashStr KeyWrittenTime;
};

#endif
