#pragma once

//------------------------------------------------------------------------------------------------//
// ������ ������ ����(url) �ٿ�δ�
//------------------------------------------------------------------------------------------------//

class MkStr;
class MkPathName;

class MkFileDownloader
{
public:

	enum eResult
	{
		eSuccess = 0, // ����

		eInvalidURL, // fileURL�� �ش��ϴ� ������ ��Ʈ��ũ�� �������� ����
		eDestFilePathIsDirectory, // destFilePath�� ���丮 �����
		eNetworkError, // ���� ��Ʈ��ũ ���� ���� �߻�
		eOutOfMemory // �޸� ����
	};

	// �ٿ� ����
	// fileURL : ��Ʈ��ũ�� ���� ���
	// destFilePath : ����� ���� ���� ���. ��Ʈ ���丮 ���� ���, Ȥ�� ���� ���
	// tryCount : ��Ʈ��ũ ������ �������� ��� �絵�� �õ� ȸ��(1�� �� ����)
	static eResult Start(const MkStr& fileURL, const MkPathName& destFilePath, int retryCount = 9);
};

//------------------------------------------------------------------------------------------------//
