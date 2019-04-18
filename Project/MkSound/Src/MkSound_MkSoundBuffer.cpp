
#include <math.h>
#include "vorbis/vorbisfile.h"

#include "MkCore_MkCheck.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkInterfaceForDataReading.h"

#include "MkSound_MkSoundBuffer.h"


//------------------------------------------------------------------------------------------------//

bool MkSoundBuffer::SetUp(LPDIRECTSOUND directSound, const MkPathName& filePath)
{
	if (directSound == NULL)
		return false;

	Clear();

	do
	{
		MkStr ext = filePath.GetFileExtension();
		ext.ToLower();

		bool isWave = ext.Equals(0, L"wav");
		bool isOgg = ext.Equals(0, L"ogg");
		
		if ((!isWave) && (!isOgg))
			break;

		// file data
		MkByteArray srcData;
		MK_CHECK(MkFileManager::GetFileData(filePath, srcData), L"음원 파일 로딩 실패 : " + filePath)
			break;

		if (srcData.Empty())
			break;

		if (isWave)
		{
			if (!_CreateBufferFromWaveFile(directSound, srcData))
				break;
		}
		else if (isOgg)
		{
			if (!_CreateBufferFromOggFile(directSound, srcData))
				break;
		}
		else
			break;

		_UpdateInformation();
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool MkSoundBuffer::SetUp(LPDIRECTSOUND directSound, LPDIRECTSOUNDBUFFER srcBuffer)
{
	Clear();

	if ((directSound == NULL) || (srcBuffer == NULL))
		return false;

	directSound->DuplicateSoundBuffer(srcBuffer, &m_SoundBuffer);
	if (m_SoundBuffer == NULL)
		return false;

	_UpdateInformation();
	return true;
}

MkSoundBuffer* MkSoundBuffer::GetClone(LPDIRECTSOUND directSound)
{
	if ((directSound == NULL) || (m_SoundBuffer == NULL))
		return NULL;

	MkSoundBuffer* instance = new MkSoundBuffer;
	if (!instance->SetUp(directSound, m_SoundBuffer))
	{
		delete instance;
		return NULL;
	}
	return instance;
}

void MkSoundBuffer::Play(bool loop)
{
	if (m_SoundBuffer != NULL)
	{
		Stop();

		m_SoundBuffer->Play(0, 0, (loop) ? DSBPLAY_LOOPING : 0);
		m_SoundBuffer->SetCurrentPosition(0);
	}
}

bool MkSoundBuffer::IsPlaying(void) const
{
	if (m_SoundBuffer == NULL)
		return false;

	DWORD state = 0;
	m_SoundBuffer->GetStatus(&state);
	return ((state & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING);
}

float MkSoundBuffer::GetPlayPosition(void) const
{
	if (m_SoundBuffer == NULL)
		return 0.f;

	DWORD readPos, writePos;
	m_SoundBuffer->GetCurrentPosition(&readPos, &writePos);
	return (static_cast<float>(readPos) / m_BytesPerSec);
}

void MkSoundBuffer::SetPlayPosition(float position)
{
	if ((m_SoundBuffer != NULL) && IsPlaying())
	{
		DWORD timePos = static_cast<DWORD>(position * m_BytesPerSec);
		m_SoundBuffer->SetCurrentPosition(timePos);
	}
}

float MkSoundBuffer::GetTotalPlayTime(void) const
{
	return (m_SoundBuffer == NULL) ? 0.f : m_TotalPlayTime;
}

void MkSoundBuffer::Stop(void)
{
	if ((m_SoundBuffer != NULL) && IsPlaying())
	{
		m_SoundBuffer->Stop();
	}
}

void MkSoundBuffer::SetVolume(float volume)
{
	if (m_SoundBuffer != NULL)
	{
		m_Volume = Clamp<float>(volume, 0.f, 1.f);

		// dB to linear(http://www.phys.unsw.edu.au/jw/dB.html)
		LONG dB = (m_Volume == 0.f) ? DSBVOLUME_MIN : static_cast<DWORD>(-2000.f * log10(1.0f / m_Volume));
		m_SoundBuffer->SetVolume(dB);
	}
}

void MkSoundBuffer::SetFrequency(float frequency)
{
	if (m_SoundBuffer != NULL)
	{
		DWORD freq = (DWORD)(m_DefaultFrequency * frequency);
		m_SoundBuffer->SetFrequency(freq);
	}
}

void MkSoundBuffer::SetSemitone(float semitone)
{
	if (m_SoundBuffer != NULL)
	{
		float additional = pow(1.05946f, semitone);
		DWORD freq = static_cast<DWORD>(m_DefaultFrequency * additional);
		m_SoundBuffer->SetFrequency(freq);
	}
}

float MkSoundBuffer::GetFrequency(void) const
{
	if (m_SoundBuffer != NULL)
	{
		DWORD freq;
		m_SoundBuffer->GetFrequency(&freq);
		return (static_cast<float>(freq) / m_DefaultFrequency);
	}
	return 0.f;
}

void MkSoundBuffer::SetPan(float position)
{
	if (m_SoundBuffer != NULL)
	{
		position = Clamp<float>(position, -1.f, 1.f);
		position *= static_cast<float>(DSBPAN_RIGHT);
		m_SoundBuffer->SetPan(static_cast<LONG>(position));
	}
}

float MkSoundBuffer::GetPan(void) const
{
	if (m_SoundBuffer != NULL)
	{
		LONG position;
		m_SoundBuffer->GetPan(&position);
		return (static_cast<float>(position) / static_cast<float>(DSBPAN_RIGHT));
	}
	return 0.f;
}

void MkSoundBuffer::Clear(void)
{
	if (m_SoundBuffer != NULL)
	{
		Stop();

		m_SoundBuffer->Release();
		m_SoundBuffer = NULL;
	}

	m_Channels = 0;
	m_DefaultFrequency = 1.f;
	m_BytesPerSec = 0.f;
	m_TotalPlayTime = 0.f;
	m_Volume = 0.f;
}

MkSoundBuffer::MkSoundBuffer()
{
	m_SoundBuffer = NULL;
	Clear();
}

//------------------------------------------------------------------------------------------------//

bool MkSoundBuffer::_CreateBufferFromWaveFile(LPDIRECTSOUND directSound, const MkByteArray& srcData)
{
	HMMIO fileHandle = NULL;
	
	do
	{
		MMIOINFO mmioInfo;
		::ZeroMemory(&mmioInfo, sizeof(MMIOINFO));
		mmioInfo.pchBuffer = reinterpret_cast<HPSTR>(const_cast<unsigned char*>(srcData.GetPtr()));
		mmioInfo.fccIOProc = FOURCC_MEM;
		mmioInfo.cchBuffer = srcData.GetSize();

		fileHandle = mmioOpen(NULL, &mmioInfo, MMIO_READ);
		if (fileHandle == NULL)
			break;

		// 구조 정보
		MMCKINFO parentCI, childCI;
		PCMWAVEFORMAT pcmwf;

		if (mmioDescend(fileHandle, &parentCI, NULL, 0) != MMSYSERR_NOERROR)
			break;

		if((parentCI.ckid != FOURCC_RIFF) || (parentCI.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
			break;

		childCI.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if (mmioDescend(fileHandle, &childCI, &parentCI, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			break;

		if (childCI.cksize < sizeof(pcmwf))
			break;

		if (mmioRead(fileHandle, reinterpret_cast<HPSTR>(&pcmwf), sizeof(pcmwf)) != sizeof(pcmwf))
			break;

		if (pcmwf.wf.wFormatTag != WAVE_FORMAT_PCM)
			break;

		WAVEFORMATEX waveFormatEx;
		memcpy_s(&waveFormatEx, sizeof(pcmwf), &pcmwf, sizeof(pcmwf));
		waveFormatEx.cbSize = 0;

		if (mmioAscend(fileHandle, &childCI, 0) != MMSYSERR_NOERROR)
			break;

		// 청크 탐색
		if (FAILED(mmioSeek(fileHandle, parentCI.dwDataOffset + sizeof(FOURCC), SEEK_SET)))
			break;

		childCI.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if (mmioDescend(fileHandle, &childCI, &parentCI, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			break;

		// 데이터를 임시 버퍼에 복사
		MkByteArray dataBuffer;
		dataBuffer.Fill(childCI.cksize);
		mmioRead(fileHandle, reinterpret_cast<HPSTR>(dataBuffer.GetPtr()), childCI.cksize);

		// 사운드 버퍼 생성 후 데이터 복사
		m_SoundBuffer = _CreateSoundBuffer(directSound, waveFormatEx, dataBuffer);
	}
	while (false);

	if (fileHandle != NULL)
	{
		mmioClose(fileHandle, 0);
	}
	return (m_SoundBuffer != NULL);
}

// vorbis call back
size_t _read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	MkInterfaceForDataReading* drInterface = reinterpret_cast<MkInterfaceForDataReading*>(datasource);
	size_t copySize = GetMin<size_t>(size * nmemb, drInterface->GetEndPosition() - drInterface->GetCurrentPosition());
	return drInterface->Read(reinterpret_cast<unsigned char*>(ptr), copySize) ? copySize : 0;
}

int _seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	MkInterfaceForDataReading* drInterface = reinterpret_cast<MkInterfaceForDataReading*>(datasource);
	switch (whence)
	{
	case SEEK_SET:
		drInterface->SetCurrentPosition(static_cast<unsigned int>(offset));
		break;
	case SEEK_CUR:
		drInterface->SetCurrentPosition(static_cast<unsigned int>(offset) + drInterface->GetCurrentPosition());
		break;
	case SEEK_END:
		drInterface->SetCurrentPosition(drInterface->GetEndPosition() - static_cast<unsigned int>(offset));
		break;
	}
	return 0;
}

int _close_func(void *datasource)
{
	return 0;
}

long _tell_func(void *datasource)
{
	MkInterfaceForDataReading* drInterface = reinterpret_cast<MkInterfaceForDataReading*>(datasource);
	return drInterface->GetCurrentPosition();
}

bool MkSoundBuffer::_CreateBufferFromOggFile(LPDIRECTSOUND directSound, const MkByteArray& srcData)
{
	OggVorbis_File vorbisFile;
	::ZeroMemory(&vorbisFile, sizeof(OggVorbis_File));

	do
	{
		MkInterfaceForDataReading drInterface;
		if (!drInterface.SetUp(srcData, 0))
			break;

		// vorbis file 오픈
		ov_callbacks ogg_callbacks = { _read_func, _seek_func, _close_func, _tell_func };
		if (ov_open_callbacks(reinterpret_cast<void*>(&drInterface), &vorbisFile, NULL, 0, ogg_callbacks) != 0)
			break;

		// 구조 정보
		vorbis_info* vorbisInfo = ov_info(&vorbisFile, -1);
		if (vorbisInfo == NULL)
			break;

		// 버퍼 확보
		int dataSize = vorbisInfo->channels * 16 / 8 * static_cast<int>(ov_pcm_total(&vorbisFile, -1));
		if (dataSize <= 0)
			break;

		MkByteArray dataBuffer;
		dataBuffer.Fill(static_cast<unsigned int>(dataSize));

		// PCM 블록 읽으며 디코딩
		int bitstream, readSize = 0, writeSize = 0;
		while (true)
		{
			writeSize = ov_read(&vorbisFile, reinterpret_cast<char*>(dataBuffer.GetPtr()) + readSize, dataSize - readSize, 0, 2, 1, &bitstream);

			if(writeSize == 0) // EOF
				break;

			if ((dataSize - readSize) >= 0) // continue
			{
				readSize += writeSize;
			}
			else // 디코딩 실패
			{
				writeSize = -1;
				break;
			}
		}

		if (writeSize < 0)
			break;

		// wave 정보 세팅
		WAVEFORMATEX waveFormatEx;
		waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
		waveFormatEx.nChannels = vorbisInfo->channels;
		waveFormatEx.nSamplesPerSec = vorbisInfo->rate;
		waveFormatEx.nAvgBytesPerSec = vorbisInfo->channels * vorbisInfo->rate * 16 / 8; // 채널 * 주파수 * 진폭 / 8(bit>byte)
		waveFormatEx.nBlockAlign = vorbisInfo->channels * 16 / 8;
		waveFormatEx.wBitsPerSample = 16;
		waveFormatEx.cbSize = 0;

		// 사운드 버퍼 생성 후 데이터 복사
		m_SoundBuffer = _CreateSoundBuffer(directSound, waveFormatEx, dataBuffer);
	}
	while (false);

	ov_clear(&vorbisFile);
	return (m_SoundBuffer != NULL);
}

LPDIRECTSOUNDBUFFER MkSoundBuffer::_CreateSoundBuffer(LPDIRECTSOUND directSound, const WAVEFORMATEX& waveFormatEx, const MkByteArray& dataBuffer)
{
	// 사운드 버퍼 생성
	DSBUFFERDESC desc;
	::ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
	desc.dwBufferBytes = dataBuffer.GetSize();
	desc.lpwfxFormat = const_cast<LPWAVEFORMATEX>(&waveFormatEx);

	LPDIRECTSOUNDBUFFER soundBuffer = NULL;
	do
	{
		if (FAILED(directSound->CreateSoundBuffer(&desc, &soundBuffer, NULL)))
			break;

		// 사운드 버퍼에 데이터 복사
		LPVOID primarySoundBuffer = NULL;
		LPVOID secondarySoundBuffer = NULL;
		DWORD primaryLength, secondaryLength;

		if (FAILED(soundBuffer->Lock(0, desc.dwBufferBytes, &primarySoundBuffer, &primaryLength, &secondarySoundBuffer, &secondaryLength, 0)))
			break;

		memcpy_s(primarySoundBuffer, primaryLength, dataBuffer.GetPtr(), primaryLength); // 순환 버퍼의 앞부분 복사
		memcpy_s(secondarySoundBuffer, secondaryLength, dataBuffer.GetPtr() + primaryLength, secondaryLength); // 순환 버퍼의 뒷부분 복사

		if (FAILED(soundBuffer->Unlock(primarySoundBuffer, primaryLength, secondarySoundBuffer, secondaryLength)))
			break;

		return soundBuffer;
	}
	while (false);

	if (soundBuffer != NULL)
	{
		soundBuffer->Release();
	}
	return NULL;
}

void MkSoundBuffer::_UpdateInformation(void)
{
	DWORD freq;
	m_SoundBuffer->GetFrequency(&freq);
	m_DefaultFrequency = (float)freq;

	WAVEFORMATEX waveFormatEx;
	m_SoundBuffer->GetFormat(&waveFormatEx, sizeof(WAVEFORMATEX), NULL);
	m_Channels = static_cast<int>(waveFormatEx.nChannels);
	m_BytesPerSec = static_cast<float>(waveFormatEx.nAvgBytesPerSec);

	DSBCAPS caps;
	caps.dwSize = sizeof(DSBCAPS);
	m_SoundBuffer->GetCaps(&caps);
	m_TotalPlayTime = (static_cast<float>(caps.dwBufferBytes) / m_BytesPerSec);

	m_Volume = 1.f;
}

//------------------------------------------------------------------------------------------------//
