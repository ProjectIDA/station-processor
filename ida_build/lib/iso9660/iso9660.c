#pragma ident "$Id: iso9660.c,v 1.1 2008/03/04 19:09:57 akimov Exp $"
#include "iso9660.h"
#include "util.h"

static const char *CDSignature = "CD001";
static const char *NRTSSignature = "NRTS";

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif 

#ifndef WIN32
static long filelength(int handle)
	{
	struct stat s;
	fstat(handle, &s);
	return s.st_size;
	}
#endif

static const char *getFileName(const char *pFullPath)
	{
	int ns = strlen(pFullPath)-1;

	for(;ns>-1;--ns)
		{
		if(pFullPath[ns]=='\\' || pFullPath[ns]=='/')
			{
			return &pFullPath[ns+1];
			}
		}

	return pFullPath;
	}

static void iso9660MakeTime(char *buff, struct tm *ptm)
	{
	sprintf(buff,"%04d%02d%02d%02d%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon + 1,ptm->tm_mday,  
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec, 0);

	}

static void iso9660MakeFileTime(char *buff, struct tm *ptm)
	{
	buff[ISO9660_DATETIME_YEAR]     = ptm->tm_year;
	buff[ISO9660_DATETIME_MONTH]    = ptm->tm_mon + 1;
	buff[ISO9660_DATETIME_DAY]      = ptm->tm_mday;
	buff[ISO9660_DATETIME_HOUR]     = ptm->tm_hour;
	buff[ISO9660_DATETIME_MINUTE]   = ptm->tm_min;
	buff[ISO9660_DATETIME_SECOND]   = ptm->tm_sec;
	buff[ISO9660_DATETIME_GMT_OFF]  = 0;

	}

long iso9660GetImageLength(ISO9660 *p)
	{
	if(p==NULL)		return 0;
	if(p->handle<0)	return 0;
	return filelength(p->handle);
	}

int iso9660Close(ISO9660 *p)
	{
	if(p==NULL) return ISO9660_OK;
	close(p->handle);
	free(p);
	return ISO9660_OK;
	}
static int iso9660UpdateVolumeSize(int handle)
	{
	long lCurrentVolumeSize;
	unsigned char sector[ISO9660_DEFAULT_BLOCK_SIZE];

	lCurrentVolumeSize = filelength(handle);

	lCurrentVolumeSize = lCurrentVolumeSize/ISO9660_DEFAULT_BLOCK_SIZE;

	lseek(handle, PRIMARY_VOLUME_DESCRIPTOR*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	memset(sector, 0, 2048);

	if(read(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
		return ISO9660_IMAGE_READ_ERROR;
		}

	memcpy(&sector[80], &lCurrentVolumeSize,  4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[80], 1);
	memcpy(&sector[84], &lCurrentVolumeSize,  4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[84], 1);

	lseek(handle, PATH_TABLE_SECTOR*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	if(write(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
/*      file writing error message      */
		return ISO9660_IMAGE_WRITE_ERROR;
		}
	return lCurrentVolumeSize;
	}
static long iso9660FindFreePositionInRootDirectory(int handle, int *position)
	{
	unsigned char sector[ISO9660_DEFAULT_BLOCK_SIZE];
	unsigned char len_dr;
	int nPos;
	long j;
	char directory_record[34+128];
	int nZPos=-1;
	long nZs;

	lseek(handle, (ROOT_DIRECTORY_EXTENT*ISO9660_DEFAULT_BLOCK_SIZE), SEEK_SET);

	for(j=0; j<ROOT_DIRECTORY_SIZE; ++j)
	{
	if(read(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
/*		ISO image read error. ISO file is bad  */
		return ISO9660_IMAGE_READ_ERROR;
		}

	nPos=0;
	if(sector[0]==0)
		{

		if(nZPos==-1)
			{
			*position = 0;
			return j+ROOT_DIRECTORY_EXTENT;
			}
		else
			{
			*position = nZPos;
			return (j+ROOT_DIRECTORY_EXTENT-1);
			}
		}
	nZPos = -1;


	while(nPos<ISO9660_DEFAULT_BLOCK_SIZE)
		{
		memcpy(directory_record, &sector[nPos], 34);
		len_dr = directory_record[0];
		if(len_dr == 0)
			{
/*          Empty place found                   */			
/*			*position = nPos;                   */ 
/*			return j+ROOT_DIRECTORY_EXTENT;     */
			nZPos = nPos;
			break;
			}

		nPos+=len_dr;
		}

	}
/*  Directory is full            */
	return ISO9660_DIRECTORY_FULL;
	}


int iso9660AddFile(ISO9660 *pISO, const char *pname, void *pdata, long nsize, struct tm filetm)
	{
	INT32 i32;
	int residue, k;
	long nNewFileNameLen = 0;
	int nPos=0;
	UINT32 Location_of_Extent=0;

	char timebuff[20];
	char directory_record[34+128];
	unsigned char sector[ISO9660_DEFAULT_BLOCK_SIZE];
	int handle;
	long lFirstSectorOfData;
	long nFileLen;
	long nDirSector;
	int position;
	int padding_byte;

	if(pISO==NULL) return ISO9660_IMAGE_WRITE_ERROR;
	handle  = pISO->handle;
	if(handle<=0 ) return ISO9660_IMAGE_WRITE_ERROR;


	nFileLen   = filelength(handle);
	lFirstSectorOfData = nFileLen/ISO9660_DEFAULT_BLOCK_SIZE;

	iso9660MakeFileTime(timebuff, &filetm);


	if((nFileLen%ISO9660_DEFAULT_BLOCK_SIZE)!=0)
		{
/*      corrupted file error message  */
/*      file length should be divisible by ISO9660_DEFAULT_BLOCK_SIZE (2048)  */
		return ISO9660_IMAGE_CORRUPTED;
		}

	nDirSector = iso9660FindFreePositionInRootDirectory(handle, &position);


	if(nDirSector<0)
		{
		return nDirSector; 
		}

	nNewFileNameLen = strlen(pname);

/*  padding if file name length is even */
	
	if( nNewFileNameLen & 1 )
		{
		padding_byte = 0;
		}
	else
		{
		padding_byte = 1;
		}

	if(position+33+nNewFileNameLen+padding_byte>ISO9660_DEFAULT_BLOCK_SIZE)
		{
		memset(sector, 0 , ISO9660_DEFAULT_BLOCK_SIZE);
		position = 0;
		++nDirSector;
		}

	lseek(handle, (nDirSector*ISO9660_DEFAULT_BLOCK_SIZE), SEEK_SET);
	if(read(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
/*		ISO image read error. ISO file is bad  */
		return ISO9660_IMAGE_READ_ERROR;
		}
	

	memset(directory_record, 0, sizeof(directory_record)); 
	directory_record[ISO9660_DIRECTORY_RECORD_LENGTH]   = (char)(33 + nNewFileNameLen + padding_byte);
	directory_record[ISO9660_DIRECTORY_RECORD_FLAGS]    =  0;
	directory_record[ISO9660_DIRECTORY_RECORD_NAME_LEN] =  (char)nNewFileNameLen;
	/*memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_NAME], pname, nNewFileNameLen);*/
	for(k=0; k<nNewFileNameLen; ++k) directory_record[ISO9660_DIRECTORY_RECORD_NAME+k] = toupper(pname[k]);	
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_DATE], timebuff, 7);

	i32 = nsize;
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_SIZE], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_SIZE], 1);
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], 1);


	i32 = lFirstSectorOfData;
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], 1);
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], 1);



	memcpy(&sector[position], directory_record,  33 + nNewFileNameLen+padding_byte);

	lseek(handle, nDirSector*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	if(write(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
		return ISO9660_IMAGE_WRITE_ERROR;
		}

	lseek(handle, 0 ,SEEK_END);

	if(write(handle, pdata, nsize)!=nsize)
		{
/*      file writing error message      */
		close(handle);
/*		unlink(pFileName);*/
		return ISO9660_IMAGE_WRITE_ERROR;
		}

	residue = nsize%ISO9660_DEFAULT_BLOCK_SIZE;

	if(nsize!=0)
		{
		residue = ISO9660_DEFAULT_BLOCK_SIZE - residue;
		memset(sector, 0 , residue);

		if(write(handle, sector, residue)!=residue)
			{
/*      file writing error message      */
			return ISO9660_IMAGE_WRITE_ERROR;
			}

		}



	iso9660UpdateVolumeSize(handle);

	return ISO9660_OK;
	}

static int MakeEmptyPathTable(int handle)
	{
	INT32 i32;
	INT16 i16;

	unsigned char sector[ISO9660_DEFAULT_BLOCK_SIZE];
	char directory_record[34];

	memset(sector, 0, ISO9660_DEFAULT_BLOCK_SIZE);
	memset(directory_record, 0, 34); 
	directory_record[ISO9660_DIRECTORY_RECORD_LENGTH]   = 34;
	directory_record[ISO9660_DIRECTORY_RECORD_FLAGS]    =  2;
	directory_record[ISO9660_DIRECTORY_RECORD_NAME_LEN] =  1;
	directory_record[ISO9660_DIRECTORY_RECORD_NAME]     =  0;

	
	i32 = PATH_TABLE_SECTOR;
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], 1);
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], 1);


	i32 = ROOT_DIRECTORY_SIZE * ISO9660_DEFAULT_BLOCK_SIZE;
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_SIZE], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_SIZE], 1);
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], 1);

	i16 = 1;
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER], &i16, 2);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER], 1);
	memcpy(&directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER+2], &i16, 2);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER+2], 1);

	memcpy(&sector[0], &directory_record, 34); /* copy info for "."  */

	directory_record[ISO9660_DIRECTORY_RECORD_NAME]     =  1;

	memcpy(&sector[34], &directory_record, 34); /* copy info for ".."  */

	lseek(handle, PATH_TABLE_SECTOR*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	if(write(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
/*      file writing error message      */
		return ISO9660_IMAGE_WRITE_ERROR;
		}
	return ISO9660_OK;
	}

ISO9660* iso9660Open(const char *pFileName, int *pError)
	{
	ISO9660 *iso9660;
	int handle;
	UINT8 sector[ISO9660_DEFAULT_BLOCK_SIZE];

	if(strlen(pFileName) > MAXPATHLEN)
		{
		*pError = ISO9660_IMAGE_OPEN_ERROR;
		return NULL;
		}

	handle = open(pFileName, O_BINARY| O_RDWR, S_IREAD|S_IWRITE );

	if(handle==-1)
		{
		*pError = ISO9660_IMAGE_OPEN_ERROR;
		return NULL;
		}


	memset(sector, 0, ISO9660_DEFAULT_BLOCK_SIZE);

	lseek(handle, PRIMARY_VOLUME_DESCRIPTOR*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	if(read(handle, sector, ISO9660_DEFAULT_BLOCK_SIZE)!=ISO9660_DEFAULT_BLOCK_SIZE)
		{
		*pError = ISO9660_IMAGE_READ_ERROR;
		return NULL;
		}

	if(memcmp(&sector[1], CDSignature, 5)!=0)
		{
		*pError = ISO9660_IMAGE_BAD_SIGNATURE;
		return NULL;
		}

	if(memcmp(&sector[8], NRTSSignature, 4)!=0)
		{
		*pError = ISO9660_IMAGE_BAD_SIGNATURE;
		return NULL;
		}

	iso9660 = (ISO9660 *)malloc(sizeof(ISO9660));
	if(iso9660==NULL)
		{
		close(handle);
		*pError = ISO9660_MEMORY_ALLOC_ERROR;
		return NULL;
		}

	strcpy(iso9660->path, pFileName);
	iso9660->handle = handle;

	*pError = ISO9660_OK;
	return iso9660;
	}

int iso9660Append(ISO9660 *p, const char *pFileName)
	{
	char filename[MAXPATHLEN];
	struct stat st; 
	struct tm file_time;
	int handle, nres;
	long flen, imagelen;
	void *buff;
	handle = open(pFileName, O_BINARY| O_RDONLY);

	strcpy(filename, getFileName(pFileName));

	if(handle==-1)
		{
/*      file open error message could be here   */
		return ISO9660_INPUT_FILE_OPEN_ERROR;
		}
	flen = filelength(handle);

	if(flen<0)
		{
		close(handle);
		return ISO9660_INPUT_FILE_READ_ERROR;
		}

	fstat( handle, &st);
	file_time = *gmtime(&st.st_mtime);

	imagelen = iso9660GetImageLength(p);

	if( imagelen+flen > CDMAXSIZE )
		{
		close(handle);
		return ISO9660_IMAGE_FULL;
		}

	buff = malloc(flen);
	if(buff==NULL)
		{
		close(handle);
		return ISO9660_MEMORY_ALLOC_ERROR;
		}

	if(read(handle, buff, flen)!=flen)
		{
		close(handle);
		free(buff);
		return ISO9660_INPUT_FILE_READ_ERROR;
		}

	close(handle);
	nres = iso9660AddFile(p, filename, buff, flen, file_time);

	free(buff);
	return nres;
	}

ISO9660* iso9660Create(const char *pFileName, const char *VolumeIdentifier)
	{
	char time_record[32];
	int handle;
	ISO9660 *iso;
	void *buff = NULL;
	unsigned char sector[2048];
	long total_number_of_sectors = ROOT_DIRECTORY_SIZE+ROOT_DIRECTORY_EXTENT;
	INT16 volume_set_size   = 1;
	INT16 sector_size       = 2048;
	INT32 path_table_length = ROOT_DIRECTORY_SIZE*ISO9660_DEFAULT_BLOCK_SIZE;
	char root_directory_record[34];
	char root_path_table[8];
	INT32 i32;
	INT32 number_of_first_sector_in_first_path_table  = 23;
	INT16 i16;
	long initial_image_size = (ROOT_DIRECTORY_EXTENT+ROOT_DIRECTORY_SIZE)*ISO9660_DEFAULT_BLOCK_SIZE;

	handle = open(pFileName, O_CREAT|O_BINARY| O_RDWR|O_TRUNC, S_IREAD|S_IWRITE );

	if(handle==-1)
		{
/*      file creation error message      */
		return NULL;
		}


	buff = calloc(initial_image_size,1);
	if(write(handle, buff, initial_image_size)!=initial_image_size)
		{
/*      file writing error message      */
		free(buff);
		close(handle);
		unlink(pFileName);
		return NULL;
		}
	free(buff);


	_lseek(handle, 16*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	memset(sector, 0, 2048);

	sector[0] = 1;
	memcpy(&sector[1], CDSignature, 5);
	sector[6] = 1;
	memcpy(&sector[8], NRTSSignature, 4);
	memcpy(&sector[40], VolumeIdentifier, min(32, strlen(VolumeIdentifier)));
	memcpy(&sector[80], &total_number_of_sectors,  4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[80], 1);
	memcpy(&sector[84], &total_number_of_sectors,  4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[84], 1);

	memcpy(&sector[120], &volume_set_size, 2);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[120], 1);
	memcpy(&sector[122], &volume_set_size, 2);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[122], 1);


	i16 = 1;
	memcpy(&sector[124], &i16, 2);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[124], 1);
	memcpy(&sector[126], &i16, 2);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[126], 1);



	memcpy(&sector[128], &sector_size,  2);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[128], 1);
	memcpy(&sector[130], &sector_size,  2);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&sector[130], 1);

//	memcpy(tmp4, &sector[128], 4);
	i32 = 10;
	memcpy(&sector[PRIMARY_DESCRIPTOR_PATH_TABLE_SIZE], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[132], 1);
	memcpy(&sector[PRIMARY_DESCRIPTOR_PATH_TABLE_SIZE+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&sector[136], 1);

	i32=19;
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&i32, 1);
	memcpy(&sector[140], &i32, 4);
	i32=20;
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&i32, 1);
	memcpy(&sector[148], &i32,  4);

//	memcpy(tmp4, &sector[148], 4);
//	memcpy(&root_directory_record, &sector[156], 34);
//	memcpy(&volume_set_identifier, &sector[186], 128);
//	memcpy(&publisher_identifier, &sector[314], 128);

	memset(root_directory_record, 0, 34); 
	root_directory_record[ISO9660_DIRECTORY_RECORD_LENGTH]   = 34;
	root_directory_record[ISO9660_DIRECTORY_RECORD_FLAGS]    =  2;
	root_directory_record[ISO9660_DIRECTORY_RECORD_NAME_LEN] =  1;
	root_directory_record[ISO9660_DIRECTORY_RECORD_NAME]     =  0;

	
	i32 = ROOT_DIRECTORY_EXTENT;
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_EXTENT], 1);
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_EXTENT+4], 1);


	i32 = ROOT_DIRECTORY_SIZE * ISO9660_DEFAULT_BLOCK_SIZE;
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_SIZE], &i32, 4);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_SIZE], 1);
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], &i32, 4);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_SIZE+4], 1);

	i16 = 1;
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER], &i16, 2);
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER], 1);
	memcpy(&root_directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER+2], &i16, 2);
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&root_directory_record[ISO9660_DIRECTORY_RECORD_VOLUME_SEQUENCE_NUMBER+2], 1);

	memcpy(&sector[PRIMARY_DESCRIPTOR_ROOT_DIRECTORY_RECORD_OFFSET], &root_directory_record, 34);
	sector[PRIMARY_DESCRIPTOR_FILE_STRUCTURE_VERSION]=1;

	struct tm ptm;
	time_t t;

	time(&t);
	ptm = *gmtime(&t);
	iso9660MakeTime(time_record, &ptm);
	memcpy(&sector[ISO9660_IMAGE_CREATION_DATE], time_record, ISO9660_DATE_FIELD_SIZE);


	if(write(handle, sector, sector_size)!=sector_size)
		{
/*      file writing error message      */
		close(handle);
		unlink(pFileName);
		return NULL;
		}
/* seek to number of first sector in first little endian path table  */
	lseek(handle, 19*2048, SEEK_SET);

	root_path_table[0] = 1; /* 1 for the root directory */
	root_path_table[1] = 0; /* number of sectors in extended attribute record */

	i32 = PATH_TABLE_SECTOR;
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&i32, 1);
	memcpy(&root_path_table[2], &i32, 4);

	i16 = 1;
	if(NATIVE_BYTE_ORDER==BIG_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&i16, 1);
	memcpy(&root_path_table[6], &i16, 2);


	if(write(handle, root_path_table, sizeof(root_path_table))!=sizeof(root_path_table))
		{
/*      file writing error message      */
		close(handle);
		unlink(pFileName);
		return NULL;
		}


/* seek to number of first sector in first big endian path table     */
	_lseek(handle, 20*2048, SEEK_SET);

	root_path_table[0] = 1; /* 1 for the root directory */
	root_path_table[1] = 0; /* number of sectors in extended attribute record */

	i32 = PATH_TABLE_SECTOR;
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT32((UINT32 *)&i32, 1);
	memcpy(&root_path_table[2], &i32, 4);

	i16 = 1;
	if(NATIVE_BYTE_ORDER==LTL_ENDIAN_BYTE_ORDER) utilSwapUINT16((UINT16 *)&i16, 1);
	memcpy(&root_path_table[6], &i16, 2);


	if(write(handle, root_path_table, sizeof(root_path_table))!=sizeof(root_path_table))
		{
/*      file writing error message      */
		close(handle);
		unlink(pFileName);
		return NULL;
		}

	if(MakeEmptyPathTable(handle)<0)
		{
		close(handle);
		unlink(pFileName);
		return NULL;
		}

	_lseek(handle, 23*2048, SEEK_SET);

	memset(sector, 0, 2048);
	memcpy(&sector[0], &root_directory_record, 34);

	root_directory_record[ISO9660_DIRECTORY_RECORD_NAME] =  1;
	memcpy(&sector[34], &root_directory_record, 34);

	if(write(handle, sector, sector_size)!=sector_size)
		{
/*      file writing error message      */
		close(handle);
		unlink(pFileName);
		return NULL;

		}




	_lseek(handle, 17*ISO9660_DEFAULT_BLOCK_SIZE, SEEK_SET);

	memset(sector, 0, 2048);

	sector[0] = 0xFF;
	memcpy(&sector[1], CDSignature, 5);
	sector[6] = 1;

	if(write(handle, sector, sector_size)!=sector_size)
		{
/*      file writing error message      */
		close(handle);
		unlink(pFileName);
		return NULL;
		}



	iso = (ISO9660 *)malloc(sizeof(ISO9660));
	if(iso==NULL)
		{
/*      memory allocation error message      */
		return NULL;
		}
	strcpy(iso->path, pFileName);
	iso->handle = handle;

	return iso;
	}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Andrei Akimov                                      |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Andrei Akimov would be        |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: iso9660.c,v $
 * Revision 1.1  2008/03/04 19:09:57  akimov
 * initial release
 *
 */
