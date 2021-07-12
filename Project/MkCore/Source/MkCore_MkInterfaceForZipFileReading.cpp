
#include "MkCore_MkCheck.h"
#include "MkCore_MkInterfaceForZipFileReading.h"

#pragma comment(lib, "minizip")


//------------------------------------------------------------------------------------------------//
// from unzip.c
//------------------------------------------------------------------------------------------------//

/* unz_file_info_interntal contain internal info about a file in zipfile*/
typedef struct unz_file_info_internal_s
{
    uLong offset_curfile;/* relative offset of local header 4 bytes */
} unz_file_info_internal;


/* file_in_zip_read_info_s contain internal information about a file in zipfile,
    when reading and decompress it */
typedef struct
{
    char  *read_buffer;         /* internal buffer for compressed data */
    z_stream stream;            /* zLib stream structure for inflate */

    uLong pos_in_zipfile;       /* position in byte on the zipfile, for fseek*/
    uLong stream_initialised;   /* flag set if stream structure is initialised*/

    uLong offset_local_extrafield;/* offset of the local extra field */
    uInt  size_local_extrafield;/* size of the local extra field */
    uLong pos_local_extrafield;   /* position in the local extra field in read*/

    uLong crc32;                /* crc32 of all data uncompressed */
    uLong crc32_wait;           /* crc32 we must obtain after decompress all */
    uLong rest_read_compressed; /* number of byte to be decompressed */
    uLong rest_read_uncompressed;/*number of byte to be obtained after decomp*/
    zlib_filefunc_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    uLong compression_method;   /* compression method (0==store) */
    uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    int   raw;
} file_in_zip_read_info_s;


/* unz_s contain internal information about the zipfile
*/
typedef struct
{
    zlib_filefunc_def z_filefunc;
    voidpf filestream;        /* io structore of the zipfile */
    unz_global_info gi;       /* public global information */
    uLong byte_before_the_zipfile;/* byte before the zipfile, (>0 for sfx)*/
    uLong num_file;             /* number of the current file in the zipfile*/
    uLong pos_in_central_dir;   /* pos of the current file in the central dir*/
    uLong current_file_ok;      /* flag about the usability of the current file*/
    uLong central_pos;          /* position of the beginning of the central dir*/

    uLong size_central_dir;     /* size of the central directory  */
    uLong offset_central_dir;   /* offset of start of central directory with
                                   respect to the starting disk number */

    unz_file_info cur_file_info; /* public info about the current file in zip*/
    unz_file_info_internal cur_file_info_internal; /* private info about it*/
    file_in_zip_read_info_s* pfile_in_zip_read; /* structure about the current
                                        file if we are decompressing it */
    int encrypted;
#    ifndef NOUNCRYPT
    unsigned long keys[3];     /* keys defining the pseudo-random sequence */
    const unsigned long* pcrc_32_tab;
#    endif
} unz_s;

int unzlocal_getByte(const zlib_filefunc_def* pzlib_filefunc_def, voidpf filestream, int *pi)
{
	unsigned char c;
    int err = (int)ZREAD(*pzlib_filefunc_def,filestream,&c,1);
    if (err==1)
    {
        *pi = (int)c;
        return UNZ_OK;
    }
    else
    {
        if (ZERROR(*pzlib_filefunc_def,filestream))
            return UNZ_ERRNO;
        else
            return UNZ_EOF;
    }
}

int unzlocal_getShort(const zlib_filefunc_def* pzlib_filefunc_def, voidpf filestream, uLong *pX)
{
	uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

int unzlocal_getLong(const zlib_filefunc_def* pzlib_filefunc_def, voidpf filestream, uLong *pX)
{
	uLong x ;
    int i;
    int err;

    err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x = (uLong)i;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<8;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<16;

    if (err==UNZ_OK)
        err = unzlocal_getByte(pzlib_filefunc_def,filestream,&i);
    x += ((uLong)i)<<24;

    if (err==UNZ_OK)
        *pX = x;
    else
        *pX = 0;
    return err;
}

void unzlocal_DosDateToTmuDate(uLong ulDosDate, tm_unz* ptm)
{
    uLong uDate;
    uDate = (uLong)(ulDosDate>>16);
    ptm->tm_mday = (uInt)(uDate&0x1f) ;
    ptm->tm_mon =  (uInt)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (uInt)(((uDate&0x0FE00)/0x0200)+1980) ;

    ptm->tm_hour = (uInt) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (uInt) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (uInt) (2*(ulDosDate&0x1f)) ;
}

int unzlocal_GetCurrentFileInfoInternal(unzFile file,
										unz_file_info *pfile_info,
										unz_file_info_internal
										*pfile_info_internal,
										char *szFileName,
										uLong fileNameBufferSize,
										void *extraField,
										uLong extraFieldBufferSize,
										char *szComment,
										uLong commentBufferSize)
{
    unz_s* s;
    unz_file_info file_info;
    unz_file_info_internal file_info_internal;
    int err=UNZ_OK;
    uLong uMagic;
    long lSeek=0;

    if (file==NULL)
        return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (ZSEEK(s->z_filefunc, s->filestream,
              s->pos_in_central_dir+s->byte_before_the_zipfile,
              ZLIB_FILEFUNC_SEEK_SET)!=0)
        err=UNZ_ERRNO;


    /* we check the magic */
    if (err==UNZ_OK)
        if (unzlocal_getLong(&s->z_filefunc, s->filestream,&uMagic) != UNZ_OK)
            err=UNZ_ERRNO;
        else if (uMagic!=0x02014b50)
            err=UNZ_BADZIPFILE;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.version) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.version_needed) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.flag) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.compression_method) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.dosDate) != UNZ_OK)
        err=UNZ_ERRNO;

    unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.crc) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.compressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.uncompressed_size) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_filename) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_file_extra) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.size_file_comment) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.disk_num_start) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getShort(&s->z_filefunc, s->filestream,&file_info.internal_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info.external_fa) != UNZ_OK)
        err=UNZ_ERRNO;

    if (unzlocal_getLong(&s->z_filefunc, s->filestream,&file_info_internal.offset_curfile) != UNZ_OK)
        err=UNZ_ERRNO;

    lSeek+=file_info.size_filename;
    if ((err==UNZ_OK) && (szFileName!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_filename<fileNameBufferSize)
        {
            *(szFileName+file_info.size_filename)='\0';
            uSizeRead = file_info.size_filename;
        }
        else
            uSizeRead = fileNameBufferSize;

        if ((file_info.size_filename>0) && (fileNameBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,szFileName,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        lSeek -= uSizeRead;
    }


    if ((err==UNZ_OK) && (extraField!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_extra<extraFieldBufferSize)
            uSizeRead = file_info.size_file_extra;
        else
            uSizeRead = extraFieldBufferSize;

        if (lSeek!=0)
            if (ZSEEK(s->z_filefunc, s->filestream,lSeek,ZLIB_FILEFUNC_SEEK_CUR)==0)
                lSeek=0;
            else
                err=UNZ_ERRNO;
        if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,extraField,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        lSeek += file_info.size_file_extra - uSizeRead;
    }
    else
        lSeek+=file_info.size_file_extra;


    if ((err==UNZ_OK) && (szComment!=NULL))
    {
        uLong uSizeRead ;
        if (file_info.size_file_comment<commentBufferSize)
        {
            *(szComment+file_info.size_file_comment)='\0';
            uSizeRead = file_info.size_file_comment;
        }
        else
            uSizeRead = commentBufferSize;

        if (lSeek!=0)
            if (ZSEEK(s->z_filefunc, s->filestream,lSeek,ZLIB_FILEFUNC_SEEK_CUR)==0)
                lSeek=0;
            else
                err=UNZ_ERRNO;
        if ((file_info.size_file_comment>0) && (commentBufferSize>0))
            if (ZREAD(s->z_filefunc, s->filestream,szComment,uSizeRead)!=uSizeRead)
                err=UNZ_ERRNO;
        lSeek+=file_info.size_file_comment - uSizeRead;
    }
    else
        lSeek+=file_info.size_file_comment;

    if ((err==UNZ_OK) && (pfile_info!=NULL))
        *pfile_info=file_info;

    if ((err==UNZ_OK) && (pfile_info_internal!=NULL))
        *pfile_info_internal=file_info_internal;

    return err;
}

//------------------------------------------------------------------------------------------------//

bool MkInterfaceForZipFileReading::SetUp(const MkPathName& filePath, bool addRelativePathToKey)
{
	Clear();

	// 경로 구성
	m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);
    if (m_CurrentFilePath.CheckPostfix(L"\\") || m_CurrentFilePath.CheckPostfix(L"/"))
        m_CurrentFilePath.BackSpace(1);
	bool ok = m_CurrentFilePath.CheckAvailable();
	MK_CHECK(ok, filePath + L" 경로에 파일이 존재하지 않음")
		return false;

	MkPathName relPath = m_CurrentFilePath;
	MkPathName dirPath;
	if (addRelativePathToKey && relPath.ConvertToRootBasisRelativePath())
	{
		dirPath = relPath.GetPath();
		dirPath.ToLower();
	}

	std::string charPath;
	m_CurrentFilePath.ExportMultiByteString(charPath);

	// 파일 오픈
	unzFile zipFile = unzOpen(charPath.c_str());
	MK_CHECK(zipFile != NULL, filePath + L" 경로 zip파일 열기 실패")
		return false;

	m_ZipFile = zipFile;

	int rlt = unzGoToFirstFile(zipFile);
	while (rlt == UNZ_OK)
	{
		char fileNameBuf[MAX_PATH] = {0,};
		unz_file_info currInfo;
		unzGetCurrentFileInfo(zipFile, &currInfo, fileNameBuf, MAX_PATH, NULL, 0, NULL, 0);
		
		MkPathName fileKey;
		fileKey.ImportMultiByteString(std::string(fileNameBuf));
		if (!fileKey.IsDirectoryPath()) // 디렉토리 경로는 무시
		{
			fileKey.ReplaceKeyword(L"/", L"\\");
			fileKey.ToLower();

			MkPathName pathFromRoot = dirPath;
			pathFromRoot += fileKey;

			_CompFileInfo& cfi = m_FileInfos.Create(pathFromRoot);

			unz_s* filePt = reinterpret_cast<unz_s*>(zipFile);
			cfi.numFile = filePt->num_file;
			cfi.posInCentralDir = filePt->pos_in_central_dir;
			cfi.needPassword = ((currInfo.flag & 0x1) == 0x1);
		}

		rlt = unzGoToNextFile(zipFile);
	}

	m_FileInfos.Rehash();
	return true;
}

unsigned int MkInterfaceForZipFileReading::Read(const MkPathName& filePath, MkByteArray& destBuffer)
{
	return _Read(filePath, MkStr::EMPTY, destBuffer);
}

unsigned int MkInterfaceForZipFileReading::Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer)
{
	return _Read(filePath, password, destBuffer);
}

void MkInterfaceForZipFileReading::Clear(void)
{
	if (m_ZipFile != NULL)
	{
		m_CurrentFilePath.Clear();
		m_FileInfos.Clear();

		unzClose(m_ZipFile);
		m_ZipFile = NULL;
	}
}

MkInterfaceForZipFileReading::MkInterfaceForZipFileReading()
{
	m_ZipFile = NULL;
}

unsigned int MkInterfaceForZipFileReading::_Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer)
{
	if (m_ZipFile == NULL)
		return 0;

	MkStr pathCopy = filePath;
	pathCopy.ToLower();
	MkHashStr pathKey = pathCopy;

	if (!m_FileInfos.Exist(pathKey))
		return 0;

	unzFile zipFile = m_ZipFile;
	const _CompFileInfo& cfi = m_FileInfos[pathKey];
	if (cfi.needPassword && password.Empty())
		return 0;

	unz_s* filePt = reinterpret_cast<unz_s*>(zipFile);
	filePt->num_file = cfi.numFile;
	filePt->pos_in_central_dir = cfi.posInCentralDir;
	filePt->current_file_ok = 1;

	unzlocal_GetCurrentFileInfoInternal(zipFile, &filePt->cur_file_info, &filePt->cur_file_info_internal, NULL, 0, NULL, 0, NULL, 0);

	if (cfi.needPassword)
	{
		std::string pw;
		password.ExportMultiByteString(pw);
		unzOpenCurrentFilePassword(zipFile, pw.c_str());
	}
	else
	{
		unzOpenCurrentFile(zipFile);
	}

	if (filePt->cur_file_info.uncompressed_size == 0)
		return 0;

	destBuffer.Fill(filePt->cur_file_info.uncompressed_size);
	unzReadCurrentFile(zipFile, destBuffer.GetPtr(), destBuffer.GetSize());
	unzCloseCurrentFile(zipFile);

	return destBuffer.GetSize();
}

//------------------------------------------------------------------------------------------------//
